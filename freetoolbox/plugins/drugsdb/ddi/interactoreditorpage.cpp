/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "interactoreditorpage.h"
#include "druginteractor.h"
#include "drugdruginteractioncore.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ftb_constants.h>

#include <biblio/bibliocore.h>

#include <drugsdb/tools.h>
#include <drugsdb/drugsdbcore.h>
#include <drugsdb/searchatcindatabasedialog.h>
#include <drugsdb/atc/atcmodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_menu.h>

#include "ui_interactoreditorwidget.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDir>
#include <QDataWidgetMapper>
#include <QStringListModel>
#include <QToolButton>
#include <QToolBar>
#include <QDesktopServices>
#include <QClipboard>
#include <QUrl>
#include <QApplication>
#include <QSortFilterProxyModel>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }
static inline DrugsDB::DrugsDBCore *dbCore() {return DrugsDB::DrugsDBCore::instance();}
static inline DrugsDB::DrugDrugInteractionCore *ddiCore() {return DrugsDB::DrugDrugInteractionCore::instance();}

static inline QString oldMolLinkFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_MOLECULE_LINK_FILENAME);}
static inline QString oldTreeXmlFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_CLASSTREE_FILENAME);}
static inline QString newInteractorsFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_INTERACTORS_FILENAME);}

QString InteractorEditorPage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + tkTr(Trans::Constants::INTERACTIONS_DATABASE);
}

QWidget *InteractorEditorPage::createPage(QWidget *parent)
{
    return new InteractorEditorWidget(parent);
}

namespace DrugsDB {
namespace Internal {
class TreeProxyModel : public QSortFilterProxyModel
{
public:
    TreeProxyModel(QObject *parent = 0)
        : QSortFilterProxyModel(parent)
    {
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        if(filterRegExp().isEmpty())
            return true;

        QModelIndex current(sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent));

        if(sourceModel()->hasChildren(current)) {
            bool atLeastOneValidChild = false;
            int i = 0;
            while(!atLeastOneValidChild) {
                const QModelIndex child(current.child(i, current.column()));
                if (!child.isValid())
                    // No valid child
                    break;

                atLeastOneValidChild = filterAcceptsRow(i, current);
                i++;
            }
            return atLeastOneValidChild;
        }

        return sourceModel()->data(current).toString().contains(filterRegExp());
    }
};

class InteractorEditorWidgetPrivate
{
public:
    InteractorEditorWidgetPrivate(InteractorEditorWidget *parent) :
        ui(0),
        _mapper(0),
        _atcCodesStringListModel(0),
        _childrenInteractorsStringListModel(0),
        _pmidStringListModel(0),
        _toolBar(0),
        aExpandAll(0),
        aCollapseAll(0),
        aSave(0),
        aEdit(0),
        aRemoveCurrent(0),
        aCreateNewClass(0),
        aCreateNewInteractor(0),
        aTranslateThis(0),
        aAddClassReviewMark(0),
        aNextUnreviewedOrUnlinked(0),
        aDownloadAllNeededPmids(0),
        _toolButton(0),
        aGoogle(0),
        aWho(0),
        aResip(0),
        aCopyClip(0),
        aAtcSearchDialog(0),
        _proxyClassModel(0),
        _proxyMoleculeModel(0),
        q(parent)
    {}

    ~InteractorEditorWidgetPrivate()
    {
    }

    void setupUi()
    {
        ui = new Ui::InteractorEditorWidget;
        ui->setupUi(q);
        ui->treeLayout->setMargin(0);
        ui->treeLayout->setSpacing(0);
        ui->widgetContainer->layout()->setMargin(0);
        ui->tabWidget->setCurrentIndex(0);
        ui->reformatOldSource->hide();
    }

    void createActionsAndToolBars()
    {
        aGoogle = new QAction(q);
        aResip = new QAction(q);
        aWho = new QAction(q);
        aCopyClip = new QAction(q);
        aAtcSearchDialog = new QAction(q);

        aExpandAll = new QAction(q);
        aCollapseAll = new QAction(q);
        aNextUnreviewedOrUnlinked = new QAction(q);
        aSave = new QAction(q);
        aRemoveCurrent = new QAction(q);
        aEdit = new QAction(q);
        aCreateNewClass = new QAction(q);
        aCreateNewInteractor = new QAction(q);
        aTranslateThis = new QAction(q);
        aAddClassReviewMark = new QAction(q);
        aDownloadAllNeededPmids = new QAction(q);
        aExpandAll->setIcon(theme()->icon(Core::Constants::ICONMOVEDOWNLIGHT));
        aCollapseAll->setIcon(theme()->icon(Core::Constants::ICONMOVEUPLIGHT));
        aAddClassReviewMark->setIcon(theme()->icon(Core::Constants::ICONBOOKMARK));
        aSave->setIcon(theme()->icon(Core::Constants::ICONSAVE));
        aEdit->setIcon(theme()->icon(Core::Constants::ICONEDIT));
        aCreateNewClass->setIcon(theme()->icon("black_dci.png")); // FIXME: create a new icon, remove magic number
        aCreateNewInteractor->setIcon(theme()->icon("molecule.png")); // FIXME: remove magic number
        aRemoveCurrent->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
        aTranslateThis->setIcon(theme()->icon(Core::Constants::ICONTRANSLATE));
        aNextUnreviewedOrUnlinked->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aDownloadAllNeededPmids->setIcon(theme()->icon(Core::Constants::ICON_PACKAGE)); // FIXME: add a download icon and correct me
        aSave->setEnabled(false);
        aEdit->setEnabled(false);
        aRemoveCurrent->setEnabled(false);
        aTranslateThis->setEnabled(false);

        _toolButton = new QToolButton(q);
        _toolButton->addAction(aAtcSearchDialog);
        _toolButton->addAction(aGoogle);
        _toolButton->addAction(aWho);
        _toolButton->addAction(aResip);
        _toolButton->addAction(aCopyClip);
        _toolButton->setDefaultAction(aAtcSearchDialog);
        _toolButton->setIcon(theme()->icon(Core::Constants::ICONHELP));
        _toolButton->setPopupMode(QToolButton::InstantPopup);
        _toolButton->setEnabled(false);

        _toolBar = new QToolBar(q);
        _toolBar->addAction(aDownloadAllNeededPmids);
        _toolBar->addAction(aAddClassReviewMark);
        _toolBar->addSeparator();
        _toolBar->addAction(aCreateNewClass);
        _toolBar->addAction(aCreateNewInteractor);
        _toolBar->addSeparator();
        _toolBar->addAction(aNextUnreviewedOrUnlinked);
        _toolBar->addSeparator();
        _toolBar->addAction(aRemoveCurrent);
        _toolBar->addAction(aEdit);
        _toolBar->addAction(aTranslateThis);
        _toolBar->addSeparator();
        _toolBar->addAction(aSave);
        _toolBar->addWidget(_toolButton);
        _toolBar->setIconSize(QSize(16,16));
        ui->toolbarLayout->addWidget(_toolBar);
    }

    void connectActions()
    {
        QObject::connect(aExpandAll, SIGNAL(triggered()), ui->classesTreeView, SLOT(expandAll()));
        QObject::connect(aCollapseAll, SIGNAL(triggered()), ui->classesTreeView, SLOT(collapseAll()));
        QObject::connect(aSave, SIGNAL(triggered()), q, SLOT(save()));
        QObject::connect(aRemoveCurrent, SIGNAL(triggered()), q, SLOT(removeCurrent()));
        QObject::connect(aEdit, SIGNAL(triggered()), q, SLOT(edit()));
//        QObject::connect(aTranslateThis, SIGNAL(triggered()), q, SLOT(translateCurrent()));
        QObject::connect(aAddClassReviewMark, SIGNAL(triggered()), q, SLOT(bookmarkClassesFromCurrent()));
        QObject::connect(aNextUnreviewedOrUnlinked, SIGNAL(triggered()), q, SLOT(nextUnreviewedOrUnlinked()));
        QObject::connect(aDownloadAllNeededPmids, SIGNAL(triggered()), ddiCore(), SLOT(downloadAllPmids()));
        QObject::connect(_toolButton, SIGNAL(triggered(QAction*)), q, SLOT(buttonActivated(QAction*)));
        QObject::connect(aCreateNewClass, SIGNAL(triggered()), q, SLOT(createActionTriggered()));
        QObject::connect(aCreateNewInteractor, SIGNAL(triggered()), q, SLOT(createActionTriggered()));
    }

    void prepareSearchLine()
    {
        ui->searchLine->setDelayedSignals(true);
        QToolButton *left = new QToolButton(q);
        left->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        ui->searchLine->setLeftButton(left);
        QToolButton *right = new QToolButton(q);
        right->addAction(aExpandAll);
        right->addAction(aCollapseAll);
        right->setDefaultAction(aExpandAll);
        ui->searchLine->setRightButton(right);
    }

    void prepareModelsAndViews()
    {
        // Models and views in the selector
        // Classes
        _proxyClassModel = new TreeProxyModel(q);
        _proxyClassModel->setSourceModel(ddiCore()->interactingClassesModel());
        _proxyClassModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        _proxyClassModel->setFilterKeyColumn(DrugInteractorModel::TrLabel);
        ui->classesTreeView->setModel(_proxyClassModel);
        ui->classesTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->classesTreeView->header()->hide();
        for(int i = 0; i < ddiCore()->interactingClassesModel()->columnCount(); ++i) {
            ui->classesTreeView->hideColumn(i);
        }
        ui->classesTreeView->showColumn(DrugInteractorModel::TrLabel);

        // Molecules
        _proxyMoleculeModel = new QSortFilterProxyModel(q);
        _proxyMoleculeModel->setSourceModel(ddiCore()->interactingMoleculesModel());
        _proxyMoleculeModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        _proxyMoleculeModel->setFilterKeyColumn(DrugInteractorModel::TrLabel);
        ui->molsListView->setModel(_proxyMoleculeModel);
        ui->molsListView->setModelColumn(DrugInteractorModel::TrLabel);
        ui->molsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Models and views in the editor
        // AtcCodes
        _atcCodesStringListModel = new QStringListModel(q);
        ui->atcTableView->setModel(_atcCodesStringListModel);
        ui->atcTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
        ui->atcTableView->setAlternatingRowColors(true);
        ui->atcTableView->horizontalHeader()->hide();
        ui->atcTableView->verticalHeader()->hide();
//        ui->atcTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        _childrenInteractorsStringListModel = new QStringListModel(q);
        ui->classChildrenTableView->setModel(_childrenInteractorsStringListModel);
        ui->classChildrenTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
        ui->classChildrenTableView->setAlternatingRowColors(true);
        ui->classChildrenTableView->horizontalHeader()->hide();
        ui->classChildrenTableView->verticalHeader()->hide();
        //ui->classChildrenTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        _pmidStringListModel = new QStringListModel(q);
        ui->pmidListView->setModel(_pmidStringListModel);
        //ui->pmidListView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
        ui->pmidListView->setAlternatingRowColors(true);
        //ui->pmidListView->horizontalHeader()->hide();
        //ui->pmidListView->verticalHeader()->hide();
    }

public:
    Ui::InteractorEditorWidget *ui;
    QDataWidgetMapper *_mapper;
    QStringListModel *_atcCodesStringListModel;
    QStringListModel *_childrenInteractorsStringListModel;
    QStringListModel *_pmidStringListModel;
    QPersistentModelIndex m_EditingIndex;

    QToolBar *_toolBar;
    QAction *aExpandAll, *aCollapseAll;
    QAction *aSave;
    QAction *aEdit;
    QAction *aRemoveCurrent;
    QAction *aCreateNewClass;
    QAction *aCreateNewInteractor;
    QAction *aTranslateThis;
    QAction *aAddClassReviewMark;
    QAction *aNextUnreviewedOrUnlinked;
    QAction *aDownloadAllNeededPmids;

    QToolButton *_toolButton;
    QAction *aGoogle;
    QAction *aWho;
    QAction *aResip;
    QAction *aCopyClip;
    QAction *aAtcSearchDialog;

    QSortFilterProxyModel *_proxyClassModel, *_proxyMoleculeModel;

private:
    InteractorEditorWidget *q;
};
}  // namespace Internal
}  // namespace DrugsDB

InteractorEditorWidget::InteractorEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new InteractorEditorWidgetPrivate(this))
{
    setObjectName("InteractorEditorWidget");

    d->setupUi();
    this->layout()->setMargin(0);
    this->layout()->setSpacing(0);

    d->createActionsAndToolBars();
    d->connectActions();
    d->prepareSearchLine();

    d->prepareModelsAndViews();

    d->_mapper = new QDataWidgetMapper(this);

    updateCounts();
    connect(ddiCore()->interactingClassesModel(), SIGNAL(unlinkedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->interactingClassesModel(), SIGNAL(unreviewedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->interactingMoleculesModel(), SIGNAL(unlinkedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->interactingMoleculesModel(), SIGNAL(unreviewedCountChanged()), this, SLOT(updateCounts()));

    setEditorsEnabled(false);
    connect(d->ui->reformatOldSource, SIGNAL(clicked()), this, SLOT(reformatOldSource()));
    connect(d->ui->classesTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactorActivated(QModelIndex)));
    connect(d->ui->molsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactorActivated(QModelIndex)));

    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(filterDrugInteractorModel(QString)));
}

InteractorEditorWidget::~InteractorEditorWidget()
{
    delete d->ui;
    delete d;
}

void InteractorEditorWidget::setEditorsEnabled(bool state)
{
    d->ui->atcTableView->setEnabled(state);
    d->ui->classChildrenTableView->setEnabled(state);
    d->ui->dateCreation->setEnabled(state);
    d->ui->dateUpdate->setEnabled(state);
    d->ui->isReviewed->setEnabled(state);
    d->ui->isAutoFound->setEnabled(state);
    d->ui->frLabel->setEnabled(state);
    d->ui->enLabel->setEnabled(state);
    d->ui->deLabel->setEnabled(state);
    d->ui->esLabel->setEnabled(state);
    d->ui->reference->setEnabled(state);
    d->ui->classInfoDe->setEnabled(state);
    d->ui->classInfoEn->setEnabled(state);
    d->ui->classInfoEs->setEnabled(state);
    d->ui->classInfoFr->setEnabled(state);
    d->ui->notWarnDuplicated->setEnabled(state);
    d->_toolButton->setEnabled(state);
}

void InteractorEditorWidget::reformatOldSource()
{
    // read the old XML molecule infos
    QDomDocument doc;
//    QFile file(oldMolLinkFile());
//    if (file.open(QIODevice::ReadOnly)) {
//        QString error;
//        int line, col;
//        if (!doc.setContent(&file, &error,&line,&col)) {
//            LOG_ERROR(tr("Can not read XML file content %1").arg(file.fileName()));
//            LOG_ERROR(QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
//        } else {
//            LOG(tr("Reading file: %1").arg(file.fileName()));
//        }
//        file.close();
//    } else {
//        LOG_ERROR(tr("Can not open XML file %1").arg(file.fileName()));
//    }

//    QDomElement rootNode = doc.firstChildElement("AfssapsLinkerModel");
//    QDomElement mainNode = rootNode.firstChildElement("Labels");
//    mainNode = mainNode.firstChildElement("Label");

//    // <Label de="" references="FreeMedForms" atcCodes="J05AG02" comments="" en="" review="true" id="" fr="DELAVIRDINE" reviewer="" es="" category="mols" autoFound="" dateofreview=""/>

//    QList<DrugInteractor> interactors;
//    QHash<QString, int> interactorsIds;  // All including mols and classes
//    QHash<QString, int> classesIds;
//    QTime chrono;
//    chrono.start();

//    // create the drugdruginteractions objects
//    while (!mainNode.isNull()) {
//        DrugInteractor interactor;
//        interactor.setData(DrugInteractor::IsValid, true);
//        interactor.setData(DrugInteractor::Id, ddiCore()->createInternalUuid());
//        interactor.setData(DrugInteractor::Reference, mainNode.attribute("references"));
//        interactor.setData(DrugInteractor::DeLabel, mainNode.attribute("de"));
//        interactor.setData(DrugInteractor::ATCCodeStringList, mainNode.attribute("atcCodes").split(","));
//        interactor.setData(DrugInteractor::Comment, mainNode.attribute("comments"));
//        interactor.setData(DrugInteractor::EnLabel, mainNode.attribute("en"));
//        if (mainNode.attribute("review")=="true")
//            interactor.setData(DrugInteractor::IsReviewed, true);
//        else
//            interactor.setData(DrugInteractor::IsReviewed, false);
//        interactor.setData(DrugInteractor::FrLabel, mainNode.attribute("fr").toUpper());
//        interactor.setData(DrugInteractor::InitialLabel, Utils::removeAccents(mainNode.attribute("fr")).toUpper());
//        interactor.setData(DrugInteractor::ReviewersStringList, mainNode.attribute("reviewer").split(";"));
//        interactor.setData(DrugInteractor::EsLabel, mainNode.attribute("es"));
//        if (mainNode.attribute("category")=="mols") {
//            interactor.setData(DrugInteractor::IsClass, false);
//        } else {
//            interactor.setData(DrugInteractor::IsClass, true);
//            classesIds.insert(interactor.data(DrugInteractor::InitialLabel).toString(), interactors.count());
//        }
//        interactor.setData(DrugInteractor::IsAutoFound, mainNode.attribute("autoFound"));
//        interactor.setData(DrugInteractor::DateOfReview, mainNode.attribute("dateofreview"));
//        interactor.setData(DrugInteractor::DateOfCreation, QDate(2010,01,01));
//        interactor.setData(DrugInteractor::IsDuplicated, false);

//        interactorsIds.insert(interactor.data(DrugInteractor::InitialLabel).toString(), interactors.count());
//        interactors << interactor;
//        // go next maininteractor
//        mainNode = mainNode.nextSiblingElement("Label");
//    }

//    Utils::Log::logTimeElapsed(chrono, "DDI_Interactors", QString("Sources reading (%1)").arg(interactors.count()));
//    chrono.restart();

    // read the old XML class tree
    QFile file(oldTreeXmlFile());
    if (file.open(QIODevice::ReadOnly)) {
        QString error;
        int line, col;
        if (!doc.setContent(&file, &error,&line,&col)) {
            LOG_ERROR(tr("Can not read XML file content %1").arg(file.fileName()));
            LOG_ERROR(QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
        } else {
            LOG(tr("Reading file: %1").arg(file.fileName()));
        }
        file.close();
    } else {
        LOG_ERROR(tr("Can not open XML file %1").arg(file.fileName()));
    }
    QDomElement rootNode = doc.firstChildElement("AfssapsTree");
    QDomElement mainNode = rootNode.firstChildElement("Class");
    QList<DrugInteractor *> interactors = ddiCore()->getDrugInteractors();
    while (!mainNode.isNull()) {
        QString classId = Utils::removeAccents(mainNode.attribute("name")).toUpper();
        qWarning() << "processing" << classId;
        DrugInteractor *interactingClass = 0;
        for(int i=0; i < interactors.count(); ++i) {
            if (interactors.at(i)->isClass() && interactors.at(i)->data(DrugInteractor::InitialLabel).toString()==classId) {
                interactingClass = interactors.at(i);
            }
        }
        if (!interactingClass) {
            qWarning() << "**** ERROR" << classId;
            mainNode = mainNode.nextSiblingElement("Class");
            continue;
        }

        QDomElement child = mainNode.firstChildElement("Molecule");
        while (!child.isNull()) {
            QString childId = Utils::removeAccents(child.attribute("name")).toUpper();
            // Get PMIDS
            //       <Source type="pubmed" link="http://www.ncbi.nlm.nih.gov/pubmed/14662625"/>
            QDomElement pmids = child.firstChildElement("Source");
            while (!pmids.isNull()) {
                if (pmids.attribute("type")=="pubmed") {
                    qWarning() << "hasSource" << childId ;
                    interactingClass->addChildClassificationPMID(childId, pmids.attribute("link").remove("http://www.ncbi.nlm.nih.gov/pubmed/"));
                } else {
                    qWarning() << "** unknown link" << interactingClass->data(DrugInteractor::InitialLabel).toString() << childId << pmids.attribute("link");
                }
                pmids = pmids.nextSiblingElement("Source");
            }

            child = child.nextSiblingElement("Molecule");
        }

        mainNode = mainNode.nextSiblingElement("Class");
    }

    qWarning() << "DONE";

    // save the resulting XML
    QString xml;
    for(int i=0; i < interactors.count(); ++i) {
        if (interactors.at(i)->data(DrugInteractor::IsDuplicated).toBool()) {
            continue;
        }
        xml += interactors.at(i)->toXml();
    }

    xml.prepend("<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- date format = yyyy-MM-dd -->\n"
            "<!--\n"
            "  Interaction levels :\n"
            "     P: Precaution\n"
            "     C: ContreIndication\n"
            "     D: Deconseille\n"
            "     T: APrendreEnCompte Take into Account\n"
            "     450: P450\n"
            "     I: Information\n"
            "     Y: Glycoprotein P\n"
            "-->\n"
            "<DDI_Interactors>\n");
    xml.append("</DDI_Interactors>\n");
    Utils::saveStringToFile(xml, newInteractorsFile());

}

/**
 * \internal
 * Save the current editing class/interactor.
 */
void InteractorEditorWidget::save()
{
    if (d->m_EditingIndex.isValid()) {
        d->ui->classesTreeView->setFocus();
        d->_mapper->submit();
        QAbstractItemModel *model = (QAbstractItemModel *)d->m_EditingIndex.model();
        // bug with mapper / checkbox in macos
        QModelIndex reviewed = model->index(d->m_EditingIndex.row(), DrugInteractorModel::IsReviewed, d->m_EditingIndex.parent());
        model->setData(reviewed, d->ui->isReviewed->isChecked());
        QModelIndex notWarnDuplication = model->index(d->m_EditingIndex.row(), DrugInteractorModel::DoNotWarnDuplicated, d->m_EditingIndex.parent());
        model->setData(notWarnDuplication, d->ui->notWarnDuplicated->isChecked());

        // manage ATC / PMIDs
        QModelIndex atc = model->index(d->m_EditingIndex.row(), DrugInteractorModel::ATCCodeStringList, d->m_EditingIndex.parent());
        QStringList codes;
        foreach(const QString &code, d->_atcCodesStringListModel->stringList()) {
            codes << code.left(code.indexOf(":"));
        }
        model->setData(atc, codes);
        QModelIndex children = model->index(d->m_EditingIndex.row(), DrugInteractorModel::ChildrenUuid, d->m_EditingIndex.parent());
        model->setData(children, d->_childrenInteractorsStringListModel->stringList());

        QModelIndex pmids = model->index(d->m_EditingIndex.row(), DrugInteractorModel::PMIDStringList, d->m_EditingIndex.parent());
        model->setData(pmids, d->_pmidStringListModel->stringList());

        DrugInteractorModel *interactorModel = qobject_cast<DrugInteractorModel *>(model);
        interactorModel->saveModel();
    }

    setEditorsEnabled(false);
}

/**
 * \internal
 * Filter all proxy models with searched string
 */
void InteractorEditorWidget::filterDrugInteractorModel(const QString &text)
{
    Q_UNUSED(text);
    d->_proxyClassModel->setFilterFixedString(text);
    d->_proxyMoleculeModel->setFilterFixedString(text);
    // TODO: code here ??? */
}

/**
 * \internal
 * Creates a new interacting class or interactor according to the sender()
 */
void InteractorEditorWidget::createActionTriggered()
{
    QAction *selected = qobject_cast<QAction*>(sender());
    if (!selected)
        return;
    QString id = Utils::askUser(tr("New item"), tr("What is the initial label (French only) ?"));
    if (selected==d->aCreateNewClass) {
        ddiCore()->createNewInteractor(id, true);
    } else if (selected==d->aCreateNewInteractor) {
        ddiCore()->createNewInteractor(id, false);
    }
    // Select created interactor
    d->ui->searchLine->setText(id);
    filterDrugInteractorModel(id);
    if (selected==d->aCreateNewClass) {
        interactorActivated(d->_proxyClassModel->index(0,0));
    } else if (selected==d->aCreateNewInteractor) {
        interactorActivated(d->_proxyMoleculeModel->index(0,0));
    }
}

void InteractorEditorWidget::removeCurrent()
{
//    if (d->m_EditingIndex.isValid()) {
//        d->m_DDIModel->removeRow(d->m_EditingIndex.row(), d->m_EditingIndex.parent());
//    }
}

void InteractorEditorWidget::edit()
{
    setEditorsEnabled(true);
}

void InteractorEditorWidget::interactorActivated(const QModelIndex &index)
{
    // submit / revert mapper ?
    if (d->ui->dateCreation->isEnabled()) {
        if (Utils::yesNoMessageBox(tr("Data changed but not saved."), tr("Do you want to save changes to the file ?"))) {
            save();
        } else {
            d->_mapper->revert();
        }
    }

    d->aEdit->setEnabled(true);
    d->aSave->setEnabled(true);
    d->aRemoveCurrent->setEnabled(true);
    d->aTranslateThis->setEnabled(true);

    setEditorsEnabled(false);

    if (!index.isValid())
        return;

    // Find the proxy & its source
    QSortFilterProxyModel *model = (QSortFilterProxyModel *)index.model();
    if (!model) {
        LOG_ERROR("No proxymodel found???");
        return;
    }
    if (model == d->_proxyMoleculeModel) {
        d->ui->tabWidget->setTabEnabled(d->ui->tabWidget->indexOf(d->ui->tabClassInfo), false);
        d->ui->tabWidget->setTabEnabled(d->ui->tabWidget->indexOf(d->ui->tabChildren), false);
    } else if (model == d->_proxyClassModel) {
        d->ui->tabWidget->setTabEnabled(d->ui->tabWidget->indexOf(d->ui->tabClassInfo), true);
        d->ui->tabWidget->setTabEnabled(d->ui->tabWidget->indexOf(d->ui->tabChildren), true);
    } else {
        LOG_ERROR("NO MODEL???");
    }

    // Set the mapper over the source model
    d->m_EditingIndex = model->mapToSource(index);
    d->_mapper->setModel(model->sourceModel());
    d->_mapper->addMapping(d->ui->interactorLabel, DrugInteractorModel::TrLabel, "text");
    d->_mapper->addMapping(d->ui->commentTextEdit, DrugInteractorModel::Comment, "plainText");
    d->_mapper->addMapping(d->ui->dateCreation, DrugInteractorModel::DateOfCreation, "date");
    d->_mapper->addMapping(d->ui->dateUpdate, DrugInteractorModel::DateLastUpdate, "date");
    d->_mapper->addMapping(d->ui->frLabel, DrugInteractorModel::FrLabel, "text");
    d->_mapper->addMapping(d->ui->enLabel, DrugInteractorModel::EnLabel, "text");
    d->_mapper->addMapping(d->ui->deLabel, DrugInteractorModel::DeLabel, "text");
    d->_mapper->addMapping(d->ui->esLabel, DrugInteractorModel::EsLabel, "text");
    d->_mapper->addMapping(d->ui->reference, DrugInteractorModel::Reference, "text");

    d->_mapper->addMapping(d->ui->classInfoFr, DrugInteractorModel::ClassInformationFr, "plainText");
    d->_mapper->addMapping(d->ui->classInfoEn, DrugInteractorModel::ClassInformationEn, "plainText");
    d->_mapper->addMapping(d->ui->classInfoDe, DrugInteractorModel::ClassInformationDe, "plainText");
//    d->_mapper->addMapping(d->ui->classInfoEs, DrugInteractorModel::ClassInformationEs, "plainText");

//    d->_mapper->addMapping(d->ui->atcTableView, DrugInteractorModel:, "");
//    d->_mapper->addMapping(d->ui->classChildrenTableView, DrugInteractorModel:, "");
//    d->_mapper->addMapping(d->ui, DrugInteractorModel:, "");
    d->_mapper->setRootIndex(d->m_EditingIndex.parent());
    d->_mapper->setCurrentModelIndex(d->m_EditingIndex);

    // set data
    // manage a OSX bug with checkboxes in mappers
    QModelIndex rev = model->index(index.row(), DrugInteractorModel::IsReviewed, index.parent());
    d->ui->isReviewed->setChecked(rev.data().toBool());
    QModelIndex autoFound = model->index(index.row(), DrugInteractorModel::IsAutoFound, index.parent());
    d->ui->isAutoFound->setChecked(autoFound.data().toBool());
    QModelIndex notWarnDuplication = model->index(index.row(), DrugInteractorModel::DoNotWarnDuplicated, index.parent());
    d->ui->notWarnDuplicated->setChecked(notWarnDuplication.data().toBool());

    // set data to (sub)models
    QModelIndex atcCodesIndex = model->index(index.row(), DrugInteractorModel::ATCCodeStringList, index.parent());
    QStringList atcCodes;
    foreach(const QString &code, atcCodesIndex.data().toStringList())
        atcCodes << code + ": " + dbCore()->atcModel()->getAtcLabel(QStringList() << code).join(";");
    d->_atcCodesStringListModel->setStringList(atcCodes);

    QModelIndex pmidsIndex = model->index(index.row(), DrugInteractorModel::PMIDStringList, index.parent());
    QStringList pmids = pmidsIndex.data().toStringList();
    d->_pmidStringListModel->setStringList(pmids);

    QModelIndex childrenIndex = model->index(index.row(), DrugInteractorModel::ChildrenUuid, index.parent());
    QStringList children = childrenIndex.data().toStringList();
    d->_childrenInteractorsStringListModel->setStringList(children);
}

void InteractorEditorWidget::buttonActivated(QAction *selected)
{
    QAbstractItemModel *model = (QAbstractItemModel *)d->m_EditingIndex.model();
    QModelIndex idx = model->index(d->m_EditingIndex.row(), DrugInteractorModel::TrLabel, d->m_EditingIndex.parent());
    QString label = idx.data().toString();
    if (selected == d->aAtcSearchDialog) {
        DrugsDB::SearchAtcInDatabaseDialog dlg(this, label);
        if (dlg.exec() == QDialog::Accepted) {
            QModelIndex atc = model->index(d->m_EditingIndex.row(), DrugInteractorModel::ATCCodeStringList, d->m_EditingIndex.parent());
            model->setData(atc, dlg.getSelectedCodes());
            d->_atcCodesStringListModel->setStringList(dlg.getSelectedCodes());
        }
    } else if (selected == d->aGoogle) {
        QDesktopServices::openUrl(QUrl(QString("http://www.google.fr/search?rls=en&q=%1+atc&ie=UTF-8&oe=UTF-8&redir_esc=").arg(label)));
    } else if (selected == d->aWho) {
        QDesktopServices::openUrl(QUrl(QString("http://www.whocc.no/atc_ddd_index/?name=%1").arg(label)));
    } else if (selected == d->aResip) {
        QApplication::clipboard()->setText(label);
        QDesktopServices::openUrl(QUrl("http://www.portailmedicaments.resip.fr/bcb_recherche/classes.asp?cc=1"));
    } else if (selected == d->aCopyClip) {
        QApplication::clipboard()->setText(label);
    }
}

void InteractorEditorWidget::bookmarkClassesFromCurrent()
{
    if (!d->ui->classesTreeView->selectionModel()->hasSelection()) {
        return;
    }
    QModelIndex idx = d->ui->classesTreeView->selectionModel()->currentIndex();
    QModelIndex classIdx = ddiCore()->interactingClassesModel()->index(idx.row(), DrugInteractorModel::IsInteractingClass, idx.parent());
    // find parent category
    while (!classIdx.data().toBool() || !classIdx.isValid()) {
        classIdx = classIdx.parent();
    }
    bool yes = Utils::yesNoMessageBox(tr("BookMark class review state"),
                                      tr("Do you want all classes under the current one: \n  * %1\n\n to be marked as unreviewed ?")
                                      .arg(ddiCore()->interactingClassesModel()->index(classIdx.row(), DrugInteractorModel::TrLabel, classIdx.parent()).data().toString()));
    if (yes) {
        for(int i = classIdx.row() + 1; i < ddiCore()->interactingClassesModel()->rowCount(classIdx.parent()); ++i) {
            idx = ddiCore()->interactingClassesModel()->index(i, DrugInteractorModel::IsReviewed);
            ddiCore()->interactingClassesModel()->setData(idx, false);
        }
    }

}

void InteractorEditorWidget::updateCounts()
{
    d->ui->sumLabel->setText(tr("0r: %1; 0l: %2")
                             .arg(ddiCore()->interactingMoleculesModel()->numberOfUnreviewed())
                             .arg(ddiCore()->interactingMoleculesModel()->numberOfUnlinked()));
}

void InteractorEditorWidget::nextUnreviewedOrUnlinked()
{
    QAbstractItemModel *model = ddiCore()->interactingClassesModel();
    QAbstractItemView *view = d->ui->classesTreeView;
    bool isTestingClasses = true;
    if (qApp->focusWidget()==d->ui->molsListView) {
        model = ddiCore()->interactingMoleculesModel();
        view = d->ui->molsListView;
        isTestingClasses = false;
    }
    QModelIndex selectMe;
    int startIdx = view->selectionModel()->currentIndex().row() + 1;
    for(int i = startIdx; i < model->rowCount(); ++i) {
        QModelIndex idx = model->index(i, DrugInteractorModel::IsReviewed);
        if (!idx.data().toBool()) {
            selectMe = model->index(idx.row(), DrugInteractorModel::TrLabel);
            break;
        }

        if (!isTestingClasses) {
            idx = model->index(i, DrugInteractorModel::ATCCodeStringList);
            if (idx.data().toStringList().isEmpty()) {
                selectMe = model->index(idx.row(), DrugInteractorModel::TrLabel);
                break;
            }
        }
    }
    if (selectMe.isValid()) {
        view->selectionModel()->clear();
        view->selectionModel()->select(selectMe, QItemSelectionModel::SelectCurrent);
        view->selectionModel()->setCurrentIndex(selectMe, QItemSelectionModel::Current);
        interactorActivated(selectMe);
    }
}

void InteractorEditorWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        // Texts
        d->aGoogle->setText(tr("Search Google (copy molecule to clipboard)"));
        d->aWho->setText(tr("Search WHO (copy molecule to clipboard)"));
        d->aResip->setText(tr("Search RESIP (copy molecule to clipboard)"));
        d->aCopyClip->setText(tr("Copy molecule name to clipboard"));
        d->aAtcSearchDialog->setText(tr("Open the ATC search dialog"));
        d->aSave->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
        d->aEdit->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
        d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
        d->aCreateNewClass->setText(tr("Create a new interacting class"));
        d->aCreateNewInteractor->setText(tr("Create a new interacting molecule"));
        d->aTranslateThis->setText(tr("Translate current"));
        d->aCollapseAll->setText(tr("Collapse all"));
        d->aExpandAll->setText(tr("Expand all"));
        d->aAddClassReviewMark->setText(tr("Mark all classes under the current as unreviewed"));
        d->aNextUnreviewedOrUnlinked->setText(tr("Go to next unreviewed or unlinked"));
        d->aDownloadAllNeededPmids->setText(tr("Download all needed publications"));
        // Tooltips
        d->aGoogle->setToolTip(d->aGoogle->text());
        d->aWho->setToolTip(d->aWho->text());
        d->aResip->setToolTip(d->aResip->text());
        d->aCopyClip->setToolTip(d->aCopyClip->text());
        d->aSave->setToolTip(d->aSave->text());
        d->aEdit->setToolTip(d->aEdit->text());
        d->aRemoveCurrent->setToolTip(d->aRemoveCurrent->text());
        d->aCreateNewClass->setToolTip(d->aCreateNewClass->text());
        d->aTranslateThis->setToolTip(d->aTranslateThis->text());
        d->aCreateNewInteractor->setToolTip(d->aCreateNewInteractor->text());
        d->aAddClassReviewMark->setToolTip(d->aAddClassReviewMark->text());
        d->aCollapseAll->setToolTip(d->aCollapseAll->text());
        d->aExpandAll->setToolTip(d->aExpandAll->text());
        d->aNextUnreviewedOrUnlinked->setToolTip(d->aNextUnreviewedOrUnlinked->text());
        d->aDownloadAllNeededPmids->setToolTip(d->aDownloadAllNeededPmids->text());
        d->ui->retranslateUi(this);
        updateCounts();
    }
}

