/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "interactoreditorwidget.h"
#include "druginteractor.h"
#include "druginteractortablemodel.h"
#include "druginteractortreeproxymodel.h"
#include "druginteractorsortfilterproxymodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/atc/searchatcindatabasedialog.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

#include "ui_interactoreditorwidget.h"

#include <QSortFilterProxyModel>
#include <QDataWidgetMapper>
#include <QDesktopServices>
#include <QStringListModel>
#include <QToolButton>
#include <QClipboard>
#include <QToolBar>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DDI::DDICore *ddiCore()  { return DDI::DDICore::instance(); }

namespace DDI {
namespace Internal {
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
        aSearchClassOnly(0), aSearchMolsAndClass(0), aSearchMolsOnly(0),
        aSave(0),
        aEdit(0),
        aRevert(0),
        aRemoveCurrent(0),
        aCreateNewClass(0),
        aCreateNewInteractor(0),
        aTranslateThis(0),
        aNextUnreviewedOrUnlinked(0),
        aDownloadAllNeededPmids(0),
        _searchToolButton(0),
        aGoogle(0),
        aWho(0),
        aResip(0),
        aCopyClip(0),
        aAtcSearchDialog(0),
        _proxyMoleculeModel(0),
        _testRunning(false),
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

        aSearchClassOnly = new QAction(q);
        aSearchMolsAndClass = new QAction(q);
        aSearchMolsOnly = new QAction(q);

        aNextUnreviewedOrUnlinked = new QAction(q);
        aSave = new QAction(q);
        aRemoveCurrent = new QAction(q);
        aEdit = new QAction(q);
        aRevert = new QAction(q);
        aCreateNewClass = new QAction(q);
        aCreateNewInteractor = new QAction(q);
        aTranslateThis = new QAction(q);
        aDownloadAllNeededPmids = new QAction(q);
        aSearchClassOnly->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        aSearchMolsAndClass->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        aSearchMolsOnly->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        aGoogle->setIcon(theme()->icon(Core::Constants::ICONINTERNET));
        aResip->setIcon(theme()->icon(Core::Constants::ICONINTERNET));
        aWho->setIcon(theme()->icon(Core::Constants::ICONINTERNET));
        aCopyClip->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        aAtcSearchDialog->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        aSave->setIcon(theme()->icon(Core::Constants::ICONSAVE));
        aEdit->setIcon(theme()->icon(Core::Constants::ICONEDIT));
        aRevert->setIcon(theme()->icon(Core::Constants::ICONREVERT));
        aCreateNewClass->setIcon(theme()->icon("black_dci.png")); // FIXME: create a new icon, remove magic number
        aCreateNewInteractor->setIcon(theme()->icon("molecule.png")); // FIXME: remove magic number
        aRemoveCurrent->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
        aTranslateThis->setIcon(theme()->icon(Core::Constants::ICONTRANSLATE));
        aNextUnreviewedOrUnlinked->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aDownloadAllNeededPmids->setIcon(theme()->icon(Core::Constants::ICON_PACKAGE)); // FIXME: add a download icon and correct me

        aSave->setEnabled(false);
        aEdit->setEnabled(false);
        aRevert->setEnabled(false);
        aRemoveCurrent->setEnabled(false);
        aTranslateThis->setEnabled(false);

        _searchToolButton = new QToolButton(q);
        _searchToolButton->setIconSize(QSize(32,32));
        _searchToolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        _searchToolButton->addAction(aAtcSearchDialog);
        _searchToolButton->addAction(aGoogle);
        _searchToolButton->addAction(aWho);
        _searchToolButton->addAction(aResip);
        _searchToolButton->addAction(aCopyClip);
        _searchToolButton->setDefaultAction(aAtcSearchDialog);
        _searchToolButton->setIcon(theme()->icon(Core::Constants::ICONHELP));
        _searchToolButton->setPopupMode(QToolButton::InstantPopup);

        _toolBar = new QToolBar(q);

        // TEST
        QAction *a = new QAction(q);
        a->setText("Class HTML");
        _toolBar->addAction(a);
        QObject::connect(a, SIGNAL(triggered()), q, SLOT(onCreatePdfOutputRequested()));
        //


        // _toolBar->addAction(aDownloadAllNeededPmids);
        // _toolBar->addSeparator();
        _toolBar->addAction(aCreateNewClass);
        _toolBar->addAction(aCreateNewInteractor);
        _toolBar->addSeparator();
        _toolBar->addAction(aNextUnreviewedOrUnlinked);
        _toolBar->addSeparator();
        _toolBar->addAction(aRemoveCurrent);
        _toolBar->addAction(aEdit);
        // _toolBar->addAction(aTranslateThis);
        _toolBar->addSeparator();
        _toolBar->addAction(aRevert);
        _toolBar->addAction(aSave);
        _toolBar->addWidget(_searchToolButton);
        _toolBar->setIconSize(QSize(32, 32));
        _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->toolbarLayout->addWidget(_toolBar);
    }

    void connectActions()
    {
        QObject::connect(aSave, SIGNAL(triggered()), q, SLOT(save()));
        QObject::connect(aRemoveCurrent, SIGNAL(triggered()), q, SLOT(removeCurrent()));
        QObject::connect(aEdit, SIGNAL(triggered()), q, SLOT(editCurrent()));
        QObject::connect(aRevert, SIGNAL(triggered()), q, SLOT(revertEdition()));
        // QObject::connect(aTranslateThis, SIGNAL(triggered()), q, SLOT(translateCurrent()));
        QObject::connect(aNextUnreviewedOrUnlinked, SIGNAL(triggered()), q, SLOT(nextUnreviewedOrUnlinked()));
        // QObject::connect(aDownloadAllNeededPmids, SIGNAL(triggered()), ddiCore(), SLOT(downloadAllPmids()));
        QObject::connect(_searchToolButton, SIGNAL(triggered(QAction*)), q, SLOT(searchButtonActivated(QAction*)));
        QObject::connect(aCreateNewClass, SIGNAL(triggered()), q, SLOT(onNewInteractorRequested()));
        QObject::connect(aCreateNewInteractor, SIGNAL(triggered()), q, SLOT(onNewInteractorRequested()));
    }

    void prepareSearchLine()
    {
        ui->searchLine->setDelayedSignals(true);
        QToolButton *left = new QToolButton(q);
        left->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        ui->searchLine->setLeftButton(left);
        QToolButton *right = new QToolButton(q);
        right->addAction(aSearchMolsAndClass);
        right->addAction(aSearchClassOnly);
        right->addAction(aSearchMolsOnly);
        right->setDefaultAction(aSearchMolsAndClass);
        ui->searchLine->setRightButton(right);
        QObject::connect(right, SIGNAL(triggered(QAction*)), q, SLOT(toggleClassMolsFilter(QAction*)));
    }

    void prepareModelsAndViews()
    {
        // Models and views in the selector

        // Molecules
        _proxyMoleculeModel = new DrugInteractorSortFilterProxyModel(q);
        _proxyMoleculeModel->setSourceModel(ddiCore()->drugInteractorTableModel());
        _proxyMoleculeModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        _proxyMoleculeModel->setFilterKeyColumn(DrugInteractorTableModel::TranslatedLabel);
        _proxyMoleculeModel->setSortRole(Qt::DisplayRole);
        _proxyMoleculeModel->setDynamicSortFilter(true);
        _proxyMoleculeModel->sort(DrugInteractorTableModel::TranslatedLabel);
        ui->molsListView->setModel(_proxyMoleculeModel);
        ui->molsListView->setModelColumn(DrugInteractorTableModel::TranslatedLabel);
        ui->molsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Models and views in the editor
        // AtcCodes
        _atcCodesStringListModel = new QStringListModel(q);
        ui->atcTableView->setModel(_atcCodesStringListModel);
        ui->atcTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
        ui->atcTableView->setAlternatingRowColors(true);
        ui->atcTableView->horizontalHeader()->hide();
        ui->atcTableView->verticalHeader()->hide();
        ui->atcTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

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

    void createMapper()
    {
        _mapper = new QDataWidgetMapper(q);
        _mapper->setModel(_proxyMoleculeModel->sourceModel());
        _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit); // AutoSubmit  ManualSubmit
        _mapper->addMapping(ui->interactorLabel, DrugInteractorTableModel::TranslatedLabel, "text");
        _mapper->addMapping(ui->commentTextEdit, DrugInteractorTableModel::Comment, "plainText");
        _mapper->addMapping(ui->dateCreation, DrugInteractorTableModel::DateOfCreation, "date");
        _mapper->addMapping(ui->dateUpdate, DrugInteractorTableModel::DateLastUpdate, "date");
        _mapper->addMapping(ui->frLabel, DrugInteractorTableModel::FrLabel, "text");
        _mapper->addMapping(ui->enLabel, DrugInteractorTableModel::EnLabel, "text");
        _mapper->addMapping(ui->deLabel, DrugInteractorTableModel::DeLabel, "text");
        _mapper->addMapping(ui->reference, DrugInteractorTableModel::Reference, "text");
        _mapper->addMapping(ui->uid, DrugInteractorTableModel::Uuid, "text");

        _mapper->addMapping(ui->classInfoFr, DrugInteractorTableModel::ClassInformationFr, "plainText");
        _mapper->addMapping(ui->classInfoEn, DrugInteractorTableModel::ClassInformationEn, "plainText");
        _mapper->addMapping(ui->classInfoDe, DrugInteractorTableModel::ClassInformationDe, "plainText");

        _mapper->addMapping(ui->isClass, DrugInteractorTableModel::IsInteractingClass, "checked");
        _mapper->addMapping(ui->isReviewed, DrugInteractorTableModel::IsReviewed, "checked");
        _mapper->addMapping(ui->isAutoFound, DrugInteractorTableModel::IsAutoFound, "checked");
        _mapper->addMapping(ui->valid, DrugInteractorTableModel::IsValid, "checked");
        _mapper->addMapping(ui->notWarnDuplicated, DrugInteractorTableModel::DoNotWarnDuplicated, "checked");
    }

public:
    Ui::InteractorEditorWidget *ui;
    QDataWidgetMapper *_mapper;
    QStringListModel *_atcCodesStringListModel;
    QStringListModel *_childrenInteractorsStringListModel;
    QStringListModel *_pmidStringListModel;
    QPersistentModelIndex m_EditingIndex;

    QToolBar *_toolBar;
    QAction *aSearchClassOnly, *aSearchMolsAndClass, *aSearchMolsOnly;
    QAction *aSave;
    QAction *aEdit;
    QAction *aRevert;
    QAction *aRemoveCurrent;
    QAction *aCreateNewClass;
    QAction *aCreateNewInteractor;
    QAction *aTranslateThis;
    QAction *aNextUnreviewedOrUnlinked;
    QAction *aDownloadAllNeededPmids;

    QToolButton *_searchToolButton;
    QAction *aGoogle;
    QAction *aWho;
    QAction *aResip;
    QAction *aCopyClip;
    QAction *aAtcSearchDialog;

    DrugInteractorSortFilterProxyModel *_proxyMoleculeModel;
    bool _testRunning;

#ifdef WITH_TESTS
    QString _testingLabel1, _testingLabel2; // used to test item creation
#endif

private:
    InteractorEditorWidget *q;
};
}  // namespace Internal
}  // namespace DDI

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
    d->createMapper();

    updateCounts();
    connect(ddiCore()->drugInteractorTableModel(), SIGNAL(unlinkedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->drugInteractorTableModel(), SIGNAL(unreviewedCountChanged()), this, SLOT(updateCounts()));
    connect(d->ui->molsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactorActivated(QModelIndex)));
    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(filterDrugInteractorModel(QString)));
    retranslateUi();
    setEditorsEnabled(false);
    connect(ddiCore(), SIGNAL(databaseChanged()), this, SLOT(onCoreDatabaseChanged()));
}

InteractorEditorWidget::~InteractorEditorWidget()
{
    if (d) {
        delete d->ui;
        delete d;
    }
    d = 0;
}

void InteractorEditorWidget::onCoreDatabaseChanged()
{
    qWarning() << "----------------------- DBCHANGED";
    setEditorsEnabled(false);
    interactorActivated(QModelIndex());
}

/** Enabled/Disabled the editor */
void InteractorEditorWidget::setEditorsEnabled(bool state)
{
    d->ui->uid->setEnabled(state);
    d->ui->atcTableView->setEnabled(state);
    d->ui->classChildrenTableView->setEnabled(state);
    d->ui->dateCreation->setEnabled(state);
    d->ui->dateUpdate->setEnabled(state);
    d->ui->isReviewed->setEnabled(state);
    d->ui->isAutoFound->setEnabled(state);
    d->ui->isClass->setEnabled(state);
    d->ui->notWarnDuplicated->setEnabled(state);
    d->ui->frLabel->setEnabled(state);
    d->ui->enLabel->setEnabled(state);
    d->ui->deLabel->setEnabled(state);
    d->ui->reference->setEnabled(state);
    d->ui->classInfoDe->setEnabled(state);
    d->ui->classInfoEn->setEnabled(state);
    d->ui->classInfoFr->setEnabled(state);
    d->ui->commentTextEdit->setEnabled(state);
}

/** Submit the current editing class/interactor to the model and submit the model to the database */
void InteractorEditorWidget::save()
{
    if (d->m_EditingIndex.isValid()) {
        QAbstractItemModel *model = ddiCore()->drugInteractorTableModel();

        // Manage ATC
        QModelIndex atc = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::ATCCodeStringList, d->m_EditingIndex.parent());
        model->setData(atc, d->_atcCodesStringListModel->stringList().join(";"));

        // Manage class children
        QModelIndex children = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::ChildrenUuid, d->m_EditingIndex.parent());
        model->setData(children, d->_childrenInteractorsStringListModel->stringList().join(";"));

        // Manage PMIDs
        QModelIndex pmids = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::PMIDStringList, d->m_EditingIndex.parent());
        model->setData(pmids, d->_pmidStringListModel->stringList().join(";"));

        // Submit mapper at the end as the model will be resetted on submit
        if (!d->_mapper->submit())
            LOG_ERROR("Unable to submit mapper");
    }

    setEditorsEnabled(false);
    d->aSave->setEnabled(false);
    d->aRevert->setEnabled(false);
    d->_proxyMoleculeModel->invalidate();
    d->_proxyMoleculeModel->setFilterFixedString(d->ui->searchLine->text());
    updateCounts();
}

/** Filter all proxy models with searched string */
void InteractorEditorWidget::filterDrugInteractorModel(const QString &text)
{
    d->_proxyMoleculeModel->invalidate();
    d->_proxyMoleculeModel->setFilterFixedString(text);
}

/** Creates a new interacting class or interactor according to the sender() */
void InteractorEditorWidget::onNewInteractorRequested()
{
    QAction *selected = qobject_cast<QAction*>(sender());
    if (!selected)
        return;
    QString id;
    if (d->_testRunning)
        id = d->_testingLabel1;
    else
        id = Utils::askUser(tr("New drug interactor"), tr("What is the label?"));
    // TODO: make some checking with the label

    id = id.toUpper();
    DrugInteractorTableModel *model = ddiCore()->drugInteractorTableModel();

    // Insert a row to the drug interactor model
    int row = 0;
    if (!model->insertRow(row)) {
        LOG_ERROR("Unable to create a new interactor");
        return;
    }

    // Set the translations & uid of the item
    QModelIndex valid = model->index(row, DrugInteractorTableModel::IsValid);
    QModelIndex fr = model->index(row, DrugInteractorTableModel::FrLabel);
    QModelIndex en = model->index(row, DrugInteractorTableModel::EnLabel);
    QModelIndex de = model->index(row, DrugInteractorTableModel::DeLabel);
    QModelIndex uid = model->index(row, DrugInteractorTableModel::Uuid);
    QModelIndex isClass = model->index(row, DrugInteractorTableModel::IsInteractingClass);
    model->setData(valid, 1);
    model->setData(fr, id);
    model->setData(en, id);
    model->setData(de, id);
    model->setData(uid, id);
    model->setData(isClass, (selected==d->aCreateNewClass)?1:0);

    // Submit the new item
    model->submit();

    // the last inserted row is the last row of the unsorted the model
    QModelIndex trLabel = model->index(model->rowCount() - 1, DrugInteractorTableModel::TranslatedLabel);

    // Open it in the editor
    d->ui->searchLine->setText("");
    toggleClassMolsFilter(d->aSearchMolsAndClass);
    filterDrugInteractorModel("");
    QModelIndex proxyIndex = d->_proxyMoleculeModel->mapFromSource(trLabel);
    d->ui->molsListView->selectionModel()->select(proxyIndex, QItemSelectionModel::SelectCurrent);
    d->ui->molsListView->setCurrentIndex(proxyIndex);
    d->ui->molsListView->scrollTo(proxyIndex, QTableView::EnsureVisible);
    interactorActivated(proxyIndex);
    updateCounts();
    editCurrent();
}

void InteractorEditorWidget::removeCurrent()
{
    if (d->ui->molsListView->currentIndex().isValid()) {
        ddiCore()->drugInteractorTableModel()->removeRow(d->m_EditingIndex.row(), d->m_EditingIndex.parent());
        updateCounts();
    }
}

/** Edit the current selected item */
void InteractorEditorWidget::editCurrent()
{
    if (d->ui->molsListView->selectionModel()->hasSelection()) {
        setEditorsEnabled(true);
        d->aSave->setEnabled(true);
        d->aRevert->setEnabled(true);
    }
}

void InteractorEditorWidget::revertEdition()
{
    if (d->ui->molsListView->selectionModel()->hasSelection()) {
        // Undo mapper
        d->_mapper->revert();
        updateCounts();
        // Enable edition
        setEditorsEnabled(false);
    }
}

/** Update UI and wrapper when an interactor is selected by the user */
void InteractorEditorWidget::interactorActivated(const QModelIndex &index)
{
    QModelIndex sourceIndex = d->_proxyMoleculeModel->mapToSource(index);
    // submit / revert mapper ?
    if (d->ui->frLabel->isEnabled()
            && d->m_EditingIndex != sourceIndex) {
        if (Utils::yesNoMessageBox(tr("Data changed but not saved."), tr("Do you want to save changes to the file ?"))) {
            save();
        } else {
            d->_mapper->revert();
        }
    }

    d->aEdit->setEnabled(true);
    d->aRemoveCurrent->setEnabled(true);
    d->aTranslateThis->setEnabled(true);

    setEditorsEnabled(false);

    if (!index.isValid())
        return;

    // Get the current source model editing index
    d->m_EditingIndex = sourceIndex;

    // Adapt UI to editing index -> Class?
    bool isClass = ddiCore()->drugInteractorTableModel()->index(d->m_EditingIndex.row(), DrugInteractorTableModel::IsInteractingClass).data().toBool();
    d->ui->classChildrenTableView->setVisible(isClass);
    d->ui->atcTableView->setVisible(!isClass);

    // Check errors
    // Not reviewed?
    QStringList msg;
    QModelIndex isRev = ddiCore()->drugInteractorTableModel()->index(d->m_EditingIndex.row(), DrugInteractorTableModel::IsReviewed);
    if (!isRev.data().toBool()) {
        msg += tr("Item is not reviewed");
    }
    // Class wihtout children
    if (isClass) {
        QModelIndex children = ddiCore()->drugInteractorTableModel()->index(d->m_EditingIndex.row(), DrugInteractorTableModel::ChildrenUuid);
        if (children.data().isNull()) {
            msg += tr("Item is a class but does not own any child");
        }

        // Children all exist
        const QStringList &list = children.data().toString().split(";", QString::SkipEmptyParts);
        foreach(const QString &child, list) {
            if (!ddiCore()->drugInteractorTableModel()->interactorUidExists(child))
                msg += tr("Child interactor %1 does not exist").arg(child);
        }

    } else {
        // No ATC link?
        QModelIndex atc = ddiCore()->drugInteractorTableModel()->index(d->m_EditingIndex.row(), DrugInteractorTableModel::ATCCodeStringList);
        if (atc.data().isNull()) {
            msg += tr("Item is not linked to any ATC code");
        }
        // TODO: Check if all ATC codes exist
    }
    d->ui->messages->setVisible(!msg.isEmpty());
    d->ui->messages->setPlainText(msg.join("\n"));

    // Set the mapper over the source model
    d->_mapper->setCurrentModelIndex(d->m_EditingIndex);

    QModelIndex atcCodesIndex = d->_proxyMoleculeModel->index(index.row(), DrugInteractorTableModel::ATCCodeStringList, index.parent());
    d->_atcCodesStringListModel->setStringList(atcCodesIndex.data().toString().split(";"));

    QModelIndex pmidsIndex = d->_proxyMoleculeModel->index(index.row(), DrugInteractorTableModel::PMIDStringList, index.parent());
    d->_pmidStringListModel->setStringList(pmidsIndex.data().toString().split(";"));

    QModelIndex childrenIndex = d->_proxyMoleculeModel->index(index.row(), DrugInteractorTableModel::ChildrenUuid, index.parent());
    d->_childrenInteractorsStringListModel->setStringList(childrenIndex.data().toString().split(";"));
}

/** React when a search action (WHO, google...) is clicked */
void InteractorEditorWidget::searchButtonActivated(QAction *selected)
{
    QModelIndex idx = ddiCore()->drugInteractorTableModel()->index(d->m_EditingIndex.row(), DrugInteractorTableModel::TranslatedLabel, d->m_EditingIndex.parent());
    QString label = idx.data().toString();
    if (selected == d->aAtcSearchDialog) {
        DDI::SearchAtcInDatabaseDialog dlg(this, label);
        if (dlg.exec() == QDialog::Accepted) {
            QModelIndex atc = ddiCore()->drugInteractorTableModel()->index(d->m_EditingIndex.row(), DrugInteractorTableModel::ATCCodeStringList, d->m_EditingIndex.parent());
            ddiCore()->drugInteractorTableModel()->setData(atc, dlg.getSelectedCodes());
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

/** Update the DrugInteractor search filter (search by molecules, by classes, by both) */
void InteractorEditorWidget::toggleClassMolsFilter(QAction *a)
{
    if (a==d->aSearchClassOnly) {
        d->_proxyMoleculeModel->setClassMolFilter(DrugInteractorSortFilterProxyModel::ClassesOnly);
    } else if (a==d->aSearchMolsAndClass) {
        d->_proxyMoleculeModel->setClassMolFilter(DrugInteractorSortFilterProxyModel::ClassesAndMolecules);
    } else if (a==d->aSearchMolsOnly) {
        d->_proxyMoleculeModel->setClassMolFilter(DrugInteractorSortFilterProxyModel::MoleculesOnly);
    }
    d->_proxyMoleculeModel->invalidate();
}

/** Update the model count */
void InteractorEditorWidget::updateCounts()
{
    d->ui->sumLabel->setText(tr("Unreviewed: %1; Without ATC: %2")
                             .arg(ddiCore()->drugInteractorTableModel()->numberOfUnreviewed())
                             .arg(ddiCore()->drugInteractorTableModel()->numberOfUnlinked()));
}

/** Find the next problematic DrugInteractor in the model. This will reset the view filters */
void InteractorEditorWidget::nextUnreviewedOrUnlinked()
{
    // Reset the filters
    filterDrugInteractorModel("");
    toggleClassMolsFilter(d->aSearchMolsAndClass);

    // Browse the model starting from the current index
    int startIdx = d->ui->molsListView->selectionModel()->currentIndex().row();
    int rowCount = d->_proxyMoleculeModel->rowCount();
    ++startIdx;
    QModelIndex selectMe;
    for(int i = startIdx; i < rowCount; ++i) {
        // Not reviewed?
        QModelIndex isRev = d->_proxyMoleculeModel->index(i, DrugInteractorTableModel::IsReviewed);
        if (!isRev.data().toBool()) {
            selectMe = d->_proxyMoleculeModel->index(i, DrugInteractorTableModel::TranslatedLabel);
            break;
        }
        // Class wihtout children
        QModelIndex isClass = d->_proxyMoleculeModel->index(i, DrugInteractorTableModel::IsInteractingClass);
        if (isClass.data().toBool()) {
            QModelIndex children = d->_proxyMoleculeModel->index(i, DrugInteractorTableModel::ChildrenUuid);
            if (children.data().isNull()) {
                selectMe = d->_proxyMoleculeModel->index(i, DrugInteractorTableModel::TranslatedLabel);
                break;
            }
        } else {
            // No ATC link?
            QModelIndex atc = d->_proxyMoleculeModel->index(i, DrugInteractorTableModel::ATCCodeStringList);
            if (atc.data().isNull()) {
                selectMe = d->_proxyMoleculeModel->index(i, DrugInteractorTableModel::TranslatedLabel);
                break;
            }
        }
    }

    // If a problematic item is found -> select it
    if (selectMe.isValid()) {
        d->ui->molsListView->setCurrentIndex(selectMe);
        interactorActivated(selectMe);
    }
}

void InteractorEditorWidget::onCreatePdfOutputRequested()
{
    ddiCore()->drugInteractorTableModel()->toPdfFile();
}



/** Retranslate UI and actions */
void InteractorEditorWidget::retranslateUi()
{
    // Texts
    d->aGoogle->setText(tr("Search Google (copy molecule to clipboard)"));
    d->aWho->setText(tr("Search WHO (copy molecule to clipboard)"));
    d->aResip->setText(tr("Search RESIP (copy molecule to clipboard)"));
    d->aCopyClip->setText(tr("Copy molecule name to clipboard"));
    d->aAtcSearchDialog->setText(tr("Open the ATC search dialog"));
    d->aSave->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
    d->aEdit->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
    d->aRevert->setText(tkTr(Trans::Constants::REVERT));
    d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
    d->aCreateNewClass->setText(tr("Create class"));
    d->aCreateNewInteractor->setText(tr("Create molecule"));
    d->aTranslateThis->setText(tr("Translate current"));
    d->aSearchClassOnly->setText(tr("Search only classes"));
    d->aSearchMolsAndClass->setText(tr("Search all"));
    d->aSearchMolsOnly->setText(tr("Search only molecules"));
    d->aNextUnreviewedOrUnlinked->setText(tr("Next problematic"));
    d->aDownloadAllNeededPmids->setText(tr("Download publications"));
    // Tooltips
    d->aGoogle->setToolTip(d->aGoogle->text());
    d->aWho->setToolTip(d->aWho->text());
    d->aResip->setToolTip(d->aResip->text());
    d->aCopyClip->setToolTip(d->aCopyClip->text());
    d->aSave->setToolTip(d->aSave->text());
    d->aEdit->setToolTip(d->aEdit->text());
    d->aRevert->setToolTip(d->aRevert->text());
    d->aRemoveCurrent->setToolTip(d->aRemoveCurrent->text());
    d->aCreateNewClass->setToolTip(d->aCreateNewClass->text());
    d->aTranslateThis->setToolTip(d->aTranslateThis->text());
    d->aCreateNewInteractor->setToolTip(d->aCreateNewInteractor->text());
    d->aSearchClassOnly->setToolTip(d->aSearchClassOnly->text());
    d->aSearchMolsAndClass->setToolTip(d->aSearchMolsAndClass->text());
    d->aSearchMolsOnly->setToolTip(d->aSearchMolsOnly->text());
    d->aNextUnreviewedOrUnlinked->setToolTip(d->aNextUnreviewedOrUnlinked->text());
    d->aDownloadAllNeededPmids->setToolTip(d->aDownloadAllNeededPmids->text());
    d->ui->retranslateUi(this);
    updateCounts();
}

void InteractorEditorWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        retranslateUi();
    }
}

#ifdef WITH_TESTS
#include <coreplugin/imainwindow.h>
#include <utils/randomizer.h>
#include <QTest>

static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }

void InteractorEditorWidget::test_runAllTests()
{
    d->_testRunning = true;
//    test_views();
//    test_actions();
//    test_itemCreation();
    test_edition();
}

void InteractorEditorWidget::test_views()
{
    // Just test if views are populated with the right model
    QCOMPARE(d->ui->molsListView->model(), d->_proxyMoleculeModel);
    // QCOMPARE(d->_mapper->model(), d->_proxyMoleculeModel->sourceModel());
}

void InteractorEditorWidget::test_actions()
{
    // First state
    // no interactor selected
    QCOMPARE(d->ui->molsListView->currentIndex(), QModelIndex());
    // no editing available
    QCOMPARE(d->ui->classChildrenTableView->isEnabled(), false);
    QCOMPARE(d->ui->dateCreation->isEnabled(), false);
    QCOMPARE(d->ui->dateUpdate->isEnabled(), false);
    QCOMPARE(d->ui->isReviewed->isEnabled(), false);
    QCOMPARE(d->ui->isAutoFound->isEnabled(), false);
    QCOMPARE(d->ui->isClass->isEnabled(), false);
    QCOMPARE(d->ui->notWarnDuplicated->isEnabled(), false);
    QCOMPARE(d->ui->frLabel->isEnabled(), false);
    QCOMPARE(d->ui->enLabel->isEnabled(), false);
    QCOMPARE(d->ui->deLabel->isEnabled(), false);
    QCOMPARE(d->ui->reference->isEnabled(), false);
    QCOMPARE(d->ui->classInfoDe->isEnabled(), false);
    QCOMPARE(d->ui->classInfoEn->isEnabled(), false);
    QCOMPARE(d->ui->classInfoFr->isEnabled(), false);
    QCOMPARE(d->ui->commentTextEdit->isEnabled(), false);

    // Tigger Edition without selected item -> nothing changes
    d->aEdit->trigger();
    // no editing available
    QCOMPARE(d->ui->classChildrenTableView->isEnabled(), false);
    QCOMPARE(d->ui->dateCreation->isEnabled(), false);
    QCOMPARE(d->ui->dateUpdate->isEnabled(), false);
    QCOMPARE(d->ui->isReviewed->isEnabled(), false);
    QCOMPARE(d->ui->isAutoFound->isEnabled(), false);
    QCOMPARE(d->ui->isClass->isEnabled(), false);
    QCOMPARE(d->ui->notWarnDuplicated->isEnabled(), false);
    QCOMPARE(d->ui->frLabel->isEnabled(), false);
    QCOMPARE(d->ui->enLabel->isEnabled(), false);
    QCOMPARE(d->ui->deLabel->isEnabled(), false);
    QCOMPARE(d->ui->reference->isEnabled(), false);
    QCOMPARE(d->ui->classInfoDe->isEnabled(), false);
    QCOMPARE(d->ui->classInfoEn->isEnabled(), false);
    QCOMPARE(d->ui->classInfoFr->isEnabled(), false);
    QCOMPARE(d->ui->commentTextEdit->isEnabled(), false);

    // Tigger Edition with an item -> editor is now available
    int column = d->ui->molsListView->modelColumn();
    d->ui->molsListView->selectionModel()->select(d->_proxyMoleculeModel->index(0, column), QItemSelectionModel::SelectCurrent);
    d->aEdit->trigger();
    QCOMPARE(d->ui->classChildrenTableView->isEnabled(), true);
    QCOMPARE(d->ui->dateCreation->isEnabled(), true);
    QCOMPARE(d->ui->dateUpdate->isEnabled(), true);
    QCOMPARE(d->ui->isReviewed->isEnabled(), true);
    QCOMPARE(d->ui->isAutoFound->isEnabled(), true);
    QCOMPARE(d->ui->isClass->isEnabled(), true);
    QCOMPARE(d->ui->notWarnDuplicated->isEnabled(), true);
    QCOMPARE(d->ui->frLabel->isEnabled(), true);
    QCOMPARE(d->ui->enLabel->isEnabled(), true);
    QCOMPARE(d->ui->deLabel->isEnabled(), true);
    QCOMPARE(d->ui->reference->isEnabled(), true);
    QCOMPARE(d->ui->classInfoDe->isEnabled(), true);
    QCOMPARE(d->ui->classInfoEn->isEnabled(), true);
    QCOMPARE(d->ui->classInfoFr->isEnabled(), true);
    QCOMPARE(d->ui->commentTextEdit->isEnabled(), true);
    d->aRevert->trigger();
    QCOMPARE(d->ui->classChildrenTableView->isEnabled(), false);
    QCOMPARE(d->ui->dateCreation->isEnabled(), false);
    QCOMPARE(d->ui->dateUpdate->isEnabled(), false);
    QCOMPARE(d->ui->isReviewed->isEnabled(), false);
    QCOMPARE(d->ui->isAutoFound->isEnabled(), false);
    QCOMPARE(d->ui->isClass->isEnabled(), false);
    QCOMPARE(d->ui->notWarnDuplicated->isEnabled(), false);
    QCOMPARE(d->ui->frLabel->isEnabled(), false);
    QCOMPARE(d->ui->enLabel->isEnabled(), false);
    QCOMPARE(d->ui->deLabel->isEnabled(), false);
    QCOMPARE(d->ui->reference->isEnabled(), false);
    QCOMPARE(d->ui->classInfoDe->isEnabled(), false);
    QCOMPARE(d->ui->classInfoEn->isEnabled(), false);
    QCOMPARE(d->ui->classInfoFr->isEnabled(), false);
    QCOMPARE(d->ui->commentTextEdit->isEnabled(), false);

    // d->aTranslateThis->setText(tr("Translate current"));

    // d->aNextUnreviewedOrUnlinked->setText(tr("Next problematic"));
    // d->aDownloadAllNeededPmids->setText(tr("Download publications"));
}

void InteractorEditorWidget::test_itemCreation()
{
    // Test1: Create a new interactor (ensure no duplicates)
    d->_testingLabel1 = QString("_TEST_ingLabel(%1)").arg(Utils::createUid());
    d->aCreateNewInteractor->trigger();
    // We should have this item as search text, and the selected item should be this one
    QCOMPARE(d->ui->searchLine->text(), d->_testingLabel1);
    QCOMPARE(d->ui->molsListView->currentIndex().data().toString(), d->_testingLabel1);
    // Editor should be available
    QCOMPARE(d->ui->classChildrenTableView->isEnabled(), true);
    QCOMPARE(d->ui->dateCreation->isEnabled(), true);
    QCOMPARE(d->ui->dateUpdate->isEnabled(), true);
    QCOMPARE(d->ui->isReviewed->isEnabled(), true);
    QCOMPARE(d->ui->isAutoFound->isEnabled(), true);
    QCOMPARE(d->ui->isClass->isEnabled(), true);
    QCOMPARE(d->ui->notWarnDuplicated->isEnabled(), true);
    QCOMPARE(d->ui->frLabel->isEnabled(), true);
    QCOMPARE(d->ui->enLabel->isEnabled(), true);
    QCOMPARE(d->ui->deLabel->isEnabled(), true);
    QCOMPARE(d->ui->reference->isEnabled(), true);
    QCOMPARE(d->ui->classInfoDe->isEnabled(), true);
    QCOMPARE(d->ui->classInfoEn->isEnabled(), true);
    QCOMPARE(d->ui->classInfoFr->isEnabled(), true);
    QCOMPARE(d->ui->commentTextEdit->isEnabled(), true);
    // Mapper should point to this newly created item
    QModelIndex source = d->_proxyMoleculeModel->mapToSource(d->ui->molsListView->currentIndex());
    QCOMPARE(d->_mapper->currentIndex(),
             source.row());
    // Current index should point to a molecule (not an interacting class)
    QModelIndex isClass = d->_proxyMoleculeModel->mapToSource(d->_proxyMoleculeModel->index(d->ui->molsListView->currentIndex().row(), DrugInteractorTableModel::IsInteractingClass));
    QCOMPARE(isClass.data().toBool(), false);

    // Test2: Create a new interacting class
    d->aCreateNewClass->trigger();
    d->_testingLabel1 = QString("_TEST_ingLabel(%1)").arg(Utils::createUid());
    d->aCreateNewInteractor->trigger();
    // We should have this item as search text, and the selected item should be this one
    QCOMPARE(d->ui->searchLine->text(), d->_testingLabel1);
    QCOMPARE(d->ui->molsListView->currentIndex().data().toString(), d->_testingLabel1);
    // Editor should be available
    QCOMPARE(d->ui->classChildrenTableView->isEnabled(), true);
    QCOMPARE(d->ui->dateCreation->isEnabled(), true);
    QCOMPARE(d->ui->dateUpdate->isEnabled(), true);
    QCOMPARE(d->ui->isReviewed->isEnabled(), true);
    QCOMPARE(d->ui->isAutoFound->isEnabled(), true);
    QCOMPARE(d->ui->isClass->isEnabled(), true);
    QCOMPARE(d->ui->notWarnDuplicated->isEnabled(), true);
    QCOMPARE(d->ui->frLabel->isEnabled(), true);
    QCOMPARE(d->ui->enLabel->isEnabled(), true);
    QCOMPARE(d->ui->deLabel->isEnabled(), true);
    QCOMPARE(d->ui->reference->isEnabled(), true);
    QCOMPARE(d->ui->classInfoDe->isEnabled(), true);
    QCOMPARE(d->ui->classInfoEn->isEnabled(), true);
    QCOMPARE(d->ui->classInfoFr->isEnabled(), true);
    QCOMPARE(d->ui->commentTextEdit->isEnabled(), true);
    // Mapper should point to this newly created item
    QCOMPARE(d->_mapper->currentIndex(),
             d->_proxyMoleculeModel->mapToSource(d->ui->molsListView->currentIndex()).row());
    // Mapper should point to this newly created item
    source = d->_proxyMoleculeModel->mapToSource(d->ui->molsListView->currentIndex());
    QCOMPARE(d->_mapper->currentIndex(),
             source.row());
    // Current index should point to a molecule (not an interacting class)
    isClass = d->_proxyMoleculeModel->mapToSource(d->_proxyMoleculeModel->index(d->ui->molsListView->currentIndex().row(), DrugInteractorTableModel::IsInteractingClass));
    QCOMPARE(isClass.data().toBool(), true);

    // TODO: Test3: Try to create a duplicate interactor
    // TODO: Test4: Try to create a duplicate interacting class
}

void InteractorEditorWidget::test_edition()
{
    qWarning() << "\n" << Q_FUNC_INFO << "\n";
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // Test1: Edit, modify, save, test values
    // Select an item
    int selectedRow = r.randomInt(0, d->_proxyMoleculeModel->rowCount() - 1);
    QString selectedId = d->_proxyMoleculeModel->index(selectedRow, 0).data().toString();
    d->m_EditingIndex = QModelIndex(); // no dialog when activating the interactor
    d->ui->molsListView->selectionModel()->select(d->_proxyMoleculeModel->index(selectedRow, 0), QItemSelectionModel::SelectCurrent);
    interactorActivated(d->_proxyMoleculeModel->index(selectedRow, 0));
    qWarning() << "Running test on interactor Id: " << d->_proxyMoleculeModel->index(selectedRow, 0).data().toString() << "m_EditingIndex" << d->m_EditingIndex.data().toString() << "RowCount" << ddiCore()->drugInteractorTableModel()->rowCount();

    // Trigger Edit action
    d->aEdit->trigger();

    // Edit all values
    QString fr = QString("_TEST_Fr_%1").arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    // select * from interactors where fr like "_TEST_%"
    QDate creationDate = QDate::currentDate().addDays(-999);
    d->ui->dateCreation->setDate(creationDate);

    d->ui->isReviewed->setChecked(true);
    d->ui->isAutoFound->setChecked(true);
    d->ui->isClass->setChecked(true);
    d->ui->notWarnDuplicated->setChecked(true);

    d->ui->frLabel->clear();
    d->ui->enLabel->clear();
    d->ui->deLabel->clear();
    d->ui->reference->clear();
    d->ui->classInfoDe->clear();
    d->ui->classInfoEn->clear();
    d->ui->classInfoFr->clear();
    d->ui->commentTextEdit->clear();
    QTest::keyClicks(d->ui->frLabel, fr, Qt::NoModifier, 10);
    QTest::keyClicks(d->ui->enLabel, "_TEST_En", Qt::NoModifier, 10);
    QTest::keyClicks(d->ui->deLabel, "_TEST_De", Qt::NoModifier, 10);
    QTest::keyClicks(d->ui->reference, "_TEST_Ref", Qt::NoModifier, 10);
    QTest::keyClicks(d->ui->classInfoDe, "_TEST_InfoDe", Qt::NoModifier, 10);
    QTest::keyClicks(d->ui->classInfoEn, "_TEST_InfoEn", Qt::NoModifier, 10);
    QTest::keyClicks(d->ui->classInfoFr, "_TEST_InfoFr", Qt::NoModifier, 10);
    QTest::keyClicks(d->ui->commentTextEdit, "_TEST_ Comment", Qt::NoModifier, 10);
    QTest::mouseClick(d->ui->frLabel, Qt::LeftButton);
    QTest::mouseClick(d->ui->enLabel, Qt::LeftButton);
    QTest::mouseClick(d->ui->deLabel, Qt::LeftButton);
    QTest::mouseClick(d->ui->reference, Qt::LeftButton);
    QTest::mouseClick(d->ui->classInfoDe, Qt::LeftButton);
    QTest::mouseClick(d->ui->classInfoEn, Qt::LeftButton);
    QTest::mouseClick(d->ui->classInfoFr, Qt::LeftButton);
    QTest::mouseClick(d->ui->commentTextEdit, Qt::LeftButton);
    QTest::mouseClick(d->ui->frLabel, Qt::LeftButton);

    // Trigger Save action
    d->aSave->trigger();
    // When submitting model -> model reset ==> losing indexes, we need to find the previous selected ddi using the mapper
    selectedRow = d->_mapper->currentIndex();
    QCOMPARE(ddiCore()->drugInteractorTableModel()->index(selectedRow, 0).data().toString(), selectedId);
    QDate lastUpdate = QDate::currentDate();

    // Test model values
    QAbstractItemModel *model = d->_mapper->model();
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::IsValid).data().toBool(), true);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::IsInteractingClass).data().toBool(), true);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::IsReviewed).data().toBool(), true);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::IsAutoFound).data().toBool(), true);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::DoNotWarnDuplicated).data().toBool(), true);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::EnLabel).data().toString(), QString("_TEST_En"));
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::FrLabel).data().toString(), fr);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::DeLabel).data().toString(), QString("_TEST_De"));
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::ClassInformationFr).data().toString(), QString("_TEST_InfoFr"));
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::ClassInformationEn).data().toString(), QString("_TEST_InfoEn"));
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::ClassInformationDe).data().toString(), QString("_TEST_InfoDe"));
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::DateOfCreation).data().toDate(), creationDate);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::DateLastUpdate).data().toDate(), lastUpdate);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::DateReview).data().toDate(), lastUpdate);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::Reference).data().toString(), QString("_TEST_Ref"));
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::Comment).data().toString(), QString("_TEST_ Comment"));

    // Check atc codes
    d->aEdit->trigger();
    QStringList atc;
    atc << "_TEST_COIN" << "_TEST_COINCOIN";
    d->_atcCodesStringListModel->setStringList(atc);
    d->aSave->trigger();
    // When submitting model -> model reset ==> losing indexes, we need to find the previous selected ddi using the mapper
    selectedRow = d->_mapper->currentIndex();
    QCOMPARE(ddiCore()->drugInteractorTableModel()->index(selectedRow, 0).data().toString(), selectedId);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::ATCCodeStringList).data().toString().split(";"), atc);

    // Check children
    d->aEdit->trigger();
    QStringList children;
    children << "_TEST_CHILD1" << "_TEST_CHILD2" << "_TEST_CHILD3";
    d->_childrenInteractorsStringListModel->setStringList(children);
    d->aSave->trigger();
    // When submitting model -> model reset ==> losing indexes, we need to find the previous selected ddi using the mapper
    selectedRow = d->_mapper->currentIndex();
    QCOMPARE(ddiCore()->drugInteractorTableModel()->index(selectedRow, 0).data().toString(), selectedId);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::ChildrenUuid).data().toString().split(";"), children);

    // Check pmids
    d->aEdit->trigger();
    QStringList pmids;
    pmids << "_TEST_PMID1" << "_TEST_PMID2" << "_TEST_PMID3";
    d->_pmidStringListModel->setStringList(pmids);
    d->aSave->trigger();
    // When submitting model -> model reset ==> losing indexes, we need to find the previous selected ddi using the mapper
    selectedRow = d->_mapper->currentIndex();
    QCOMPARE(ddiCore()->drugInteractorTableModel()->index(selectedRow, 0).data().toString(), selectedId);
    QCOMPARE(model->index(selectedRow, DrugInteractorTableModel::PMIDStringList).data().toString().split(";"), pmids);

    // TODO: test remove current
    //    d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
}

#endif // WITH_TESTS
