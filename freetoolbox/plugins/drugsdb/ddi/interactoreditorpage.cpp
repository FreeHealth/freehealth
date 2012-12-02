/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
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
#include <drugsdb/atcmodel.h>
#include <drugsdb/searchatcindatabasedialog.h>

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
class InteractorEditorWidgetPrivate
{
public:
    Ui::InteractorEditorWidget *ui;
    QDataWidgetMapper *m_Mapper;
    QStringListModel *m_AtcCodes, *m_ChildrenInteractors, *m_Pmids;
    QPersistentModelIndex m_EditingIndex;

    QToolBar *m_ToolBar;
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

    QToolButton *m_ToolButton, *m_CreateNewToolButton;
    QAction *google;
    QAction *who;
    QAction *resip;
    QAction *copyClip;
    QAction *atcSearchDialog;
};
}  // namespace Internal
}  // namespace DrugsDB

InteractorEditorWidget::InteractorEditorWidget(QWidget *parent) :
    QWidget(parent), d(new InteractorEditorWidgetPrivate)
{
    setObjectName("InteractorEditorWidget");
    d->ui = new Ui::InteractorEditorWidget;
    d->ui->setupUi(this);
    this->layout()->setMargin(0);
    this->layout()->setSpacing(0);
    d->ui->treeLayout->setMargin(0);
    d->ui->treeLayout->setSpacing(0);
    d->ui->widgetContainer->layout()->setMargin(0);
    d->ui->tabWidget->setCurrentIndex(0);
    d->ui->classesTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->molsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->reformatOldSource->hide();

    // manage interactors classesTreeView / molsListView
    d->ui->classesTreeView->setModel(ddiCore()->interactingClassesModel());
    d->ui->classesTreeView->header()->hide();
    for(int i = 0; i < ddiCore()->interactingClassesModel()->columnCount(); ++i) {
        d->ui->classesTreeView->hideColumn(i);
    }
    d->ui->classesTreeView->showColumn(DrugInteractorModel::TrLabel);

    d->ui->molsListView->setModel(ddiCore()->interactingMoleculesModel());
    d->ui->molsListView->setModelColumn(DrugInteractorModel::TrLabel);

    d->ui->sumLabel->setText(tr("0r: %1; 0l: %2")
                             .arg(ddiCore()->interactingMoleculesModel()->numberOfUnreviewed())
                             .arg(ddiCore()->interactingMoleculesModel()->numberOfUnlinked()));
    connect(ddiCore()->interactingClassesModel(), SIGNAL(unlinkedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->interactingClassesModel(), SIGNAL(unreviewedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->interactingMoleculesModel(), SIGNAL(unlinkedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->interactingMoleculesModel(), SIGNAL(unreviewedCountChanged()), this, SLOT(updateCounts()));

    // Create actions
    d->aExpandAll = new QAction(this);
    d->aCollapseAll = new QAction(this);
    d->aNextUnreviewedOrUnlinked = new QAction(this);
    d->aSave = new QAction(this);
    d->aRemoveCurrent = new QAction(this);
    d->aEdit = new QAction(this);
    d->aCreateNewClass = new QAction(this);
    d->aCreateNewInteractor = new QAction(this);
    d->aTranslateThis = new QAction(this);
    d->aAddClassReviewMark = new QAction(this);
    d->aDownloadAllNeededPmids = new QAction(this);
    d->aExpandAll->setIcon(theme()->icon(Core::Constants::ICONMOVEDOWNLIGHT));
    d->aCollapseAll->setIcon(theme()->icon(Core::Constants::ICONMOVEUPLIGHT));
    d->aAddClassReviewMark->setIcon(theme()->icon(Core::Constants::ICONBOOKMARK));
    d->aSave->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    d->aEdit->setIcon(theme()->icon(Core::Constants::ICONEDIT));
    d->aCreateNewClass->setIcon(theme()->icon(Core::Constants::ICONADD));
    d->aCreateNewInteractor->setIcon(theme()->icon(Core::Constants::ICONADD));
    d->aRemoveCurrent->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    d->aTranslateThis->setIcon(theme()->icon(Core::Constants::ICONTRANSLATE));
    d->aNextUnreviewedOrUnlinked->setIcon(theme()->icon(Core::Constants::ICONNEXT));
    d->aDownloadAllNeededPmids->setIcon(theme()->icon(Core::Constants::ICONDOCTOR));
    d->aSave->setEnabled(false);
    d->aEdit->setEnabled(false);
    d->aRemoveCurrent->setEnabled(false);
    d->aTranslateThis->setEnabled(false);

    d->m_ToolButton = new QToolButton(this);
    d->google = new QAction(tr("Search Google (copy molecule to clipboard)"), d->m_ToolButton);
    d->who = new QAction(tr("Search WHO (copy molecule to clipboard)"), d->m_ToolButton);
    d->resip = new QAction(tr("Search RESIP (copy molecule to clipboard)"), d->m_ToolButton);
    d->copyClip = new QAction(tr("Copy molecule name to clipboard"), d->m_ToolButton);
    d->atcSearchDialog = new QAction(tr("Open the ATC search dialog"), d->m_ToolButton);
    d->m_ToolButton->addAction(d->atcSearchDialog);
    d->m_ToolButton->addAction(d->google);
    d->m_ToolButton->addAction(d->who);
    d->m_ToolButton->addAction(d->resip);
    d->m_ToolButton->addAction(d->copyClip);
    d->m_ToolButton->setDefaultAction(d->atcSearchDialog);
    d->m_ToolButton->setIcon(theme()->icon(Core::Constants::ICONHELP));
    d->m_ToolButton->setPopupMode(QToolButton::InstantPopup);
    d->m_ToolButton->setEnabled(false);

    d->m_CreateNewToolButton = new QToolButton(this);
    d->m_CreateNewToolButton->addAction(d->aCreateNewClass);
    d->m_CreateNewToolButton->addAction(d->aCreateNewInteractor);
    d->m_CreateNewToolButton->setDefaultAction(d->aCreateNewClass);
    d->m_CreateNewToolButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    d->m_CreateNewToolButton->setPopupMode(QToolButton::InstantPopup);

    d->m_ToolBar = new QToolBar(this);
    d->m_ToolBar->addAction(d->aDownloadAllNeededPmids);
    d->m_ToolBar->addAction(d->aAddClassReviewMark);
    d->m_ToolBar->addWidget(d->m_CreateNewToolButton);
    d->m_ToolBar->addAction(d->aNextUnreviewedOrUnlinked);
    d->m_ToolBar->addAction(d->aExpandAll);
    d->m_ToolBar->addAction(d->aCollapseAll);
    d->m_ToolBar->addAction(d->aRemoveCurrent);
    d->m_ToolBar->addAction(d->aEdit);
    d->m_ToolBar->addAction(d->aTranslateThis);
    d->m_ToolBar->addAction(d->aSave);
    d->m_ToolBar->addWidget(d->m_ToolButton);
    d->m_ToolBar->setIconSize(QSize(16,16));
    d->ui->toolbarLayout->addWidget(d->m_ToolBar);

    connect(d->aExpandAll, SIGNAL(triggered()), d->ui->classesTreeView, SLOT(expandAll()));
    connect(d->aCollapseAll, SIGNAL(triggered()), d->ui->classesTreeView, SLOT(collapseAll()));
    connect(d->aSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(d->aRemoveCurrent, SIGNAL(triggered()), this, SLOT(removeCurrent()));
    connect(d->aEdit, SIGNAL(triggered()), this, SLOT(edit()));
    connect(d->aTranslateThis, SIGNAL(triggered()), this, SLOT(translateCurrent()));
    connect(d->aAddClassReviewMark, SIGNAL(triggered()), this, SLOT(bookmarkClassesFromCurrent()));
    connect(d->aNextUnreviewedOrUnlinked, SIGNAL(triggered()), this, SLOT(nextUnreviewedOrUnlinked()));
    connect(d->aDownloadAllNeededPmids, SIGNAL(triggered()), ddiCore(), SLOT(downloadAllPmids()));

    connect(d->m_ToolButton, SIGNAL(triggered(QAction*)), this, SLOT(buttonActivated(QAction*)));
    connect(d->m_CreateNewToolButton, SIGNAL(triggered(QAction*)), this, SLOT(createButtonActivated(QAction*)));

    // manage search line
    d->ui->searchLine->setDelayedSignals(true);
    QToolButton *left = new QToolButton(this);
    left->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    d->ui->searchLine->setLeftButton(left);
    QToolButton *right = new QToolButton(this);
    right->addAction(d->aExpandAll);
    right->addAction(d->aCollapseAll);
    right->setDefaultAction(d->aExpandAll);
    d->ui->searchLine->setRightButton(right);

    // Create models && mapper
    d->m_AtcCodes = new QStringListModel(this);
    d->ui->atcTableView->setModel(d->m_AtcCodes);
    d->ui->atcTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    d->ui->atcTableView->setAlternatingRowColors(true);
    d->ui->atcTableView->horizontalHeader()->hide();
    d->ui->atcTableView->verticalHeader()->hide();
    d->ui->atcTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    d->m_ChildrenInteractors = new QStringListModel(this);
    d->ui->classChildrenTableView->setModel(d->m_ChildrenInteractors);
    d->ui->classChildrenTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    d->ui->classChildrenTableView->setAlternatingRowColors(true);
    d->ui->classChildrenTableView->horizontalHeader()->hide();
    d->ui->classChildrenTableView->verticalHeader()->hide();
//    d->ui->classChildrenTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    d->m_Pmids = new QStringListModel(this);
    d->ui->pmidListView->setModel(d->m_Pmids);
//    d->ui->pmidListView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    d->ui->pmidListView->setAlternatingRowColors(true);
//    d->ui->pmidListView->horizontalHeader()->hide();
//    d->ui->pmidListView->verticalHeader()->hide();

    d->m_Mapper = new QDataWidgetMapper(this);

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
    d->m_ToolButton->setEnabled(state);
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

void InteractorEditorWidget::save()
{
    if (d->m_EditingIndex.isValid()) {
        d->ui->classesTreeView->setFocus();
        d->m_Mapper->submit();
        QAbstractItemModel *model = (QAbstractItemModel *)d->m_EditingIndex.model();
        // bug with mapper / checkbox in macos
        QModelIndex reviewed = model->index(d->m_EditingIndex.row(), DrugInteractorModel::IsReviewed, d->m_EditingIndex.parent());
        model->setData(reviewed, d->ui->isReviewed->isChecked());
        QModelIndex notWarnDuplication = model->index(d->m_EditingIndex.row(), DrugInteractorModel::DoNotWarnDuplicated, d->m_EditingIndex.parent());
        model->setData(notWarnDuplication, d->ui->notWarnDuplicated->isChecked());

        // manage ATC / PMIDs
        QModelIndex atc = model->index(d->m_EditingIndex.row(), DrugInteractorModel::ATCCodeStringList, d->m_EditingIndex.parent());
        QStringList codes;
        foreach(const QString &code, d->m_AtcCodes->stringList()) {
            codes << code.left(code.indexOf(":"));
        }
        model->setData(atc, codes);
        QModelIndex children = model->index(d->m_EditingIndex.row(), DrugInteractorModel::ChildrenUuid, d->m_EditingIndex.parent());
        model->setData(children, d->m_ChildrenInteractors->stringList());

        QModelIndex pmids = model->index(d->m_EditingIndex.row(), DrugInteractorModel::PMIDStringList, d->m_EditingIndex.parent());
        model->setData(pmids, d->m_Pmids->stringList());

        DrugInteractorModel *interactorModel = qobject_cast<DrugInteractorModel *>(model);
        interactorModel->saveModel();
    }

    setEditorsEnabled(false);
}

void InteractorEditorWidget::filterDrugInteractorModel(const QString &text)
{
    Q_UNUSED(text);
    // TODO: code here ??? */
}

void InteractorEditorWidget::createButtonActivated(QAction *selected)
{
    QString id = Utils::askUser(tr("New item"), tr("What is the initial label (French only) ?"));
    if (selected==d->aCreateNewClass) {
        ddiCore()->createNewInteractor(id, true);
    } else if (selected==d->aCreateNewInteractor) {
        ddiCore()->createNewInteractor(id, false);
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
            d->m_Mapper->revert();
        }
    }

    d->aEdit->setEnabled(true);
    d->aSave->setEnabled(true);
    d->aRemoveCurrent->setEnabled(true);
    d->aTranslateThis->setEnabled(true);

    setEditorsEnabled(false);

    if (!index.isValid())
        return;

    QAbstractItemModel *model = (QAbstractItemModel *)index.model();
    d->m_Mapper->setModel(model);
    d->m_Mapper->addMapping(d->ui->interactorLabel, DrugInteractorModel::TrLabel, "text");
    d->m_Mapper->addMapping(d->ui->commentTextEdit, DrugInteractorModel::Comment, "plainText");
    d->m_Mapper->addMapping(d->ui->dateCreation, DrugInteractorModel::DateOfCreation, "date");
    d->m_Mapper->addMapping(d->ui->dateUpdate, DrugInteractorModel::DateLastUpdate, "date");
    d->m_Mapper->addMapping(d->ui->frLabel, DrugInteractorModel::FrLabel, "text");
    d->m_Mapper->addMapping(d->ui->enLabel, DrugInteractorModel::EnLabel, "text");
    d->m_Mapper->addMapping(d->ui->deLabel, DrugInteractorModel::DeLabel, "text");
    d->m_Mapper->addMapping(d->ui->esLabel, DrugInteractorModel::EsLabel, "text");
    d->m_Mapper->addMapping(d->ui->reference, DrugInteractorModel::Reference, "text");

    d->m_Mapper->addMapping(d->ui->classInfoFr, DrugInteractorModel::ClassInformationFr, "plainText");
    d->m_Mapper->addMapping(d->ui->classInfoEn, DrugInteractorModel::ClassInformationEn, "plainText");
    d->m_Mapper->addMapping(d->ui->classInfoDe, DrugInteractorModel::ClassInformationDe, "plainText");
//    d->m_Mapper->addMapping(d->ui->classInfoEs, DrugInteractorModel::ClassInformationEs, "plainText");

//    d->m_Mapper->addMapping(d->ui->atcTableView, DrugInteractorModel:, "");
//    d->m_Mapper->addMapping(d->ui->classChildrenTableView, DrugInteractorModel:, "");
//    d->m_Mapper->addMapping(d->ui, DrugInteractorModel:, "");
    d->m_Mapper->setRootIndex(index.parent());
    d->m_Mapper->setCurrentModelIndex(index);

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
    d->m_AtcCodes->setStringList(atcCodes);

    QModelIndex pmidsIndex = model->index(index.row(), DrugInteractorModel::PMIDStringList, index.parent());
    QStringList pmids = pmidsIndex.data().toStringList();
    d->m_Pmids->setStringList(pmids);

    QModelIndex childrenIndex = model->index(index.row(), DrugInteractorModel::ChildrenUuid, index.parent());
    QStringList children = childrenIndex.data().toStringList();
    d->m_ChildrenInteractors->setStringList(children);

    d->m_EditingIndex = index;
}

void InteractorEditorWidget::buttonActivated(QAction *selected)
{
    QAbstractItemModel *model = (QAbstractItemModel *)d->m_EditingIndex.model();
    QModelIndex idx = model->index(d->m_EditingIndex.row(), DrugInteractorModel::TrLabel, d->m_EditingIndex.parent());
    QString label = idx.data().toString();
    if (selected == d->atcSearchDialog) {
        DrugsDB::SearchAtcInDatabaseDialog dlg(this, label);
        if (dlg.exec() == QDialog::Accepted) {
            QModelIndex atc = model->index(d->m_EditingIndex.row(), DrugInteractorModel::ATCCodeStringList, d->m_EditingIndex.parent());
            model->setData(atc, dlg.getSelectedCodes());
            d->m_AtcCodes->setStringList(dlg.getSelectedCodes());
        }
    } else if (selected == d->google) {
        QDesktopServices::openUrl(QUrl(QString("http://www.google.fr/search?rls=en&q=%1+atc&ie=UTF-8&oe=UTF-8&redir_esc=").arg(label)));
    } else if (selected == d->who) {
        QDesktopServices::openUrl(QUrl(QString("http://www.whocc.no/atc_ddd_index/?name=%1").arg(label)));
    } else if (selected == d->resip) {
        QApplication::clipboard()->setText(label);
        QDesktopServices::openUrl(QUrl("http://www.portailmedicaments.resip.fr/bcb_recherche/classes.asp?cc=1"));
    } else if (selected == d->copyClip) {
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

//bool InteractorEditorWidget::event(QEvent *e)
//{
//    if (e->type()==QEvent::Show) {
//        // Add actions to mainWindow toolbar
//        qWarning() << "ADD";
//        if (!d->toolbarIncluded) {
//            mainWindow()->addToolBar(d->m_ToolBar);
//            d->m_ToolBar->setVisible(true);
//        }
//    } else if (e->type()==QEvent::Hide) {
//        qWarning() << "REMOVE";
//        if (d->toolbarIncluded) {
//            mainWindow()->removeToolBar(d->m_ToolBar);
//            d->m_ToolBar->setVisible(false);
//            mainWindow()->update();
//        }
//    }
//    return QWidget::event(e);
//}

//void InteractorEditorWidget::showEvent(QShowEvent *e)
//{
//    qWarning() << "SHOW" << d->m_ToolBar << d->m_ToolBar->isVisible();
//    if (!d->m_ToolBar->isVisible())
//        mainWindow()->addToolBar(d->m_ToolBar);
//    QWidget::showEvent(e);
//}

//void InteractorEditorWidget::hideEvent(QHideEvent *e)
//{
//    qWarning() << "HIDE" << d->m_ToolBar << d->m_ToolBar->isVisible();
//    if (d->m_ToolBar->isVisible())
//        mainWindow()->removeToolBar(d->m_ToolBar);
//    QWidget::hideEvent(e);
//}
