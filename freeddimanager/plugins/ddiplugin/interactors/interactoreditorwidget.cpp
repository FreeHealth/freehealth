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
#include "interactoreditorwidget.h"
#include "druginteractor.h"
#include "druginteractortablemodel.h"
#include "druginteractortreeproxymodel.h"
#include "druginteractorsortfilterproxymodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/atc/searchatcindatabasedialog.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
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
        aRemoveCurrent(0),
        aCreateNewClass(0),
        aCreateNewInteractor(0),
        aTranslateThis(0),
        aNextUnreviewedOrUnlinked(0),
        aDownloadAllNeededPmids(0),
        _toolButton(0),
        aGoogle(0),
        aWho(0),
        aResip(0),
        aCopyClip(0),
        aAtcSearchDialog(0),
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

        aSearchClassOnly = new QAction(q);
        aSearchMolsAndClass = new QAction(q);
        aSearchMolsOnly = new QAction(q);

        aNextUnreviewedOrUnlinked = new QAction(q);
        aSave = new QAction(q);
        aRemoveCurrent = new QAction(q);
        aEdit = new QAction(q);
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
        _toolButton->setIconSize(QSize(32,32));
        _toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
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
        _toolBar->setIconSize(QSize(32, 32));
        _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->toolbarLayout->addWidget(_toolBar);
    }

    void connectActions()
    {
        QObject::connect(aSave, SIGNAL(triggered()), q, SLOT(save()));
        QObject::connect(aRemoveCurrent, SIGNAL(triggered()), q, SLOT(removeCurrent()));
        QObject::connect(aEdit, SIGNAL(triggered()), q, SLOT(edit()));
        // QObject::connect(aTranslateThis, SIGNAL(triggered()), q, SLOT(translateCurrent()));
        QObject::connect(aNextUnreviewedOrUnlinked, SIGNAL(triggered()), q, SLOT(nextUnreviewedOrUnlinked()));
        // QObject::connect(aDownloadAllNeededPmids, SIGNAL(triggered()), ddiCore(), SLOT(downloadAllPmids()));
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
        ui->classesTreeView->hide();

        // Molecules
        _proxyMoleculeModel = new DrugInteractorSortFilterProxyModel(q);
        _proxyMoleculeModel->setSourceModel(ddiCore()->drugInteractorTableModel());
        _proxyMoleculeModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        _proxyMoleculeModel->setFilterKeyColumn(DrugInteractorTableModel::TranslatedLabel);
        _proxyMoleculeModel->setSortRole(Qt::DisplayRole);
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
    QAction *aRemoveCurrent;
    QAction *aCreateNewClass;
    QAction *aCreateNewInteractor;
    QAction *aTranslateThis;
    QAction *aNextUnreviewedOrUnlinked;
    QAction *aDownloadAllNeededPmids;

    QToolButton *_toolButton;
    QAction *aGoogle;
    QAction *aWho;
    QAction *aResip;
    QAction *aCopyClip;
    QAction *aAtcSearchDialog;

    DrugInteractorSortFilterProxyModel *_proxyMoleculeModel;

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

    d->_mapper = new QDataWidgetMapper(this);

    updateCounts();
    connect(ddiCore()->drugInteractorTableModel(), SIGNAL(unlinkedCountChanged()), this, SLOT(updateCounts()));
    connect(ddiCore()->drugInteractorTableModel(), SIGNAL(unreviewedCountChanged()), this, SLOT(updateCounts()));

    // connect(d->ui->classesTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactorActivated(QModelIndex)));
    connect(d->ui->molsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactorActivated(QModelIndex)));

    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(filterDrugInteractorModel(QString)));
    retranslateUi();

    setEditorsEnabled(false);
}

InteractorEditorWidget::~InteractorEditorWidget()
{
    if (d) {
        delete d->ui;
        delete d;
    }
    d = 0;
}

/** Enabled/Disabled the editor */
void InteractorEditorWidget::setEditorsEnabled(bool state)
{
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
    d->_toolButton->setEnabled(state);
}

/** Submit the current editing class/interactor to the model and submit the model to the database */
void InteractorEditorWidget::save()
{
    if (d->m_EditingIndex.isValid()) {
        d->ui->molsListView->setFocus();

        QAbstractItemModel *model = (QAbstractItemModel *)d->m_EditingIndex.model();
//        // bug with mapper / checkbox in macos
//        QModelIndex reviewed = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::IsReviewed, d->m_EditingIndex.parent());
//        model->setData(reviewed, d->ui->isReviewed->isChecked());
//        QModelIndex notWarnDuplication = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::DoNotWarnDuplicated, d->m_EditingIndex.parent());
//        model->setData(notWarnDuplication, d->ui->notWarnDuplicated->isChecked());

        // manage ATC / PMIDs
        QModelIndex atc = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::ATCCodeStringList, d->m_EditingIndex.parent());
        model->setData(atc, d->_atcCodesStringListModel->stringList().join(";"));

        QModelIndex children = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::ChildrenUuid, d->m_EditingIndex.parent());
        model->setData(children, d->_childrenInteractorsStringListModel->stringList().join(";"));

        QModelIndex pmids = model->index(d->m_EditingIndex.row(), DrugInteractorTableModel::PMIDStringList, d->m_EditingIndex.parent());
        model->setData(pmids, d->_pmidStringListModel->stringList().join(";"));

        d->_mapper->submit();
    }

    setEditorsEnabled(false);
    d->aSave->setEnabled(false);
}

/** Filter all proxy models with searched string */
void InteractorEditorWidget::filterDrugInteractorModel(const QString &text)
{
    d->_proxyMoleculeModel->setFilterFixedString(text);
}

/** Creates a new interacting class or interactor according to the sender() */
void InteractorEditorWidget::createActionTriggered()
{
    QAction *selected = qobject_cast<QAction*>(sender());
    if (!selected)
        return;
    QString id = Utils::askUser(tr("New drug interactor"), tr("What is the label?"));

    // TODO: make some checking with the label


    // Insert a row to the drug interactor model
    ddiCore()->drugInteractorTableModel()->insertRow(0);

//    // Submit the new item
//    ddiCore()->drugInteractorTableModel()->submit();

    // Set the translations & uid of the item
    QModelIndex valid = ddiCore()->drugInteractorTableModel()->index(0, DrugInteractorTableModel::IsValid);
    QModelIndex fr = ddiCore()->drugInteractorTableModel()->index(0, DrugInteractorTableModel::FrLabel);
    QModelIndex en = ddiCore()->drugInteractorTableModel()->index(0, DrugInteractorTableModel::EnLabel);
    QModelIndex de = ddiCore()->drugInteractorTableModel()->index(0, DrugInteractorTableModel::DeLabel);
    QModelIndex uid = ddiCore()->drugInteractorTableModel()->index(0, DrugInteractorTableModel::Uuid);
    QModelIndex isClass = ddiCore()->drugInteractorTableModel()->index(0, DrugInteractorTableModel::IsInteractingClass);
    ddiCore()->drugInteractorTableModel()->setData(valid, 1);
    ddiCore()->drugInteractorTableModel()->setData(fr, id);
    ddiCore()->drugInteractorTableModel()->setData(en, id);
    ddiCore()->drugInteractorTableModel()->setData(de, id);
    ddiCore()->drugInteractorTableModel()->setData(uid, id);
    ddiCore()->drugInteractorTableModel()->setData(isClass, (selected==d->aCreateNewClass)?0:1);

    // Open it in the editor
    d->ui->searchLine->setText(id);
    toggleClassMolsFilter(d->aSearchMolsAndClass);
    filterDrugInteractorModel(id);
    for(int i=0; i < d->_proxyMoleculeModel->rowCount(); ++i) {
        QModelIndex trLabel = d->_proxyMoleculeModel->index(0, DrugInteractorTableModel::TranslatedLabel);
        if (trLabel.data().toString().compare(id,  Qt::CaseInsensitive)==0) {
            interactorActivated(trLabel);
            d->ui->molsListView->selectionModel()->select(trLabel, QItemSelectionModel::SelectCurrent);
            d->ui->molsListView->setCurrentIndex(trLabel);
            break;
        }
    }

    // Submit the new item
    ddiCore()->drugInteractorTableModel()->submit();
}

void InteractorEditorWidget::removeCurrent()
{
//    if (d->m_EditingIndex.isValid()) {
//        d->m_DDIModel->removeRow(d->m_EditingIndex.row(), d->m_EditingIndex.parent());
//    }
}

/** Edit the current selected item */
void InteractorEditorWidget::edit()
{
    setEditorsEnabled(true);
    d->aSave->setEnabled(true);
}

/** Update UI and wrapper when an interactor is selected by the user */
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
    d->aRemoveCurrent->setEnabled(true);
    d->aTranslateThis->setEnabled(true);

    setEditorsEnabled(false);

    if (!index.isValid())
        return;

    // Get the current source model editing index
    d->m_EditingIndex = d->_proxyMoleculeModel->mapToSource(index);

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
    d->_mapper->setModel(d->_proxyMoleculeModel->sourceModel());
    d->_mapper->addMapping(d->ui->interactorLabel, DrugInteractorTableModel::TranslatedLabel, "text");
    d->_mapper->addMapping(d->ui->commentTextEdit, DrugInteractorTableModel::Comment, "plainText");
    d->_mapper->addMapping(d->ui->dateCreation, DrugInteractorTableModel::DateOfCreation, "date");
    d->_mapper->addMapping(d->ui->dateUpdate, DrugInteractorTableModel::DateLastUpdate, "date");
    d->_mapper->addMapping(d->ui->frLabel, DrugInteractorTableModel::FrLabel, "text");
    d->_mapper->addMapping(d->ui->enLabel, DrugInteractorTableModel::EnLabel, "text");
    d->_mapper->addMapping(d->ui->deLabel, DrugInteractorTableModel::DeLabel, "text");
    d->_mapper->addMapping(d->ui->reference, DrugInteractorTableModel::Reference, "text");

    d->_mapper->addMapping(d->ui->classInfoFr, DrugInteractorTableModel::ClassInformationFr, "plainText");
    d->_mapper->addMapping(d->ui->classInfoEn, DrugInteractorTableModel::ClassInformationEn, "plainText");
    d->_mapper->addMapping(d->ui->classInfoDe, DrugInteractorTableModel::ClassInformationDe, "plainText");

    d->_mapper->addMapping(d->ui->isClass, DrugInteractorTableModel::IsInteractingClass, "checked");
    d->_mapper->addMapping(d->ui->isReviewed, DrugInteractorTableModel::IsReviewed, "checked");
    d->_mapper->addMapping(d->ui->isAutoFound, DrugInteractorTableModel::IsAutoFound, "checked");
    d->_mapper->addMapping(d->ui->notWarnDuplicated, DrugInteractorTableModel::DoNotWarnDuplicated, "checked");

    d->_mapper->setRootIndex(d->m_EditingIndex.parent());
    d->_mapper->setCurrentModelIndex(d->m_EditingIndex);

    QModelIndex atcCodesIndex = d->_proxyMoleculeModel->index(index.row(), DrugInteractorTableModel::ATCCodeStringList, index.parent());
    d->_atcCodesStringListModel->setStringList(atcCodesIndex.data().toString().split(";"));

    QModelIndex pmidsIndex = d->_proxyMoleculeModel->index(index.row(), DrugInteractorTableModel::PMIDStringList, index.parent());
    d->_pmidStringListModel->setStringList(pmidsIndex.data().toString().split(";"));

    QModelIndex childrenIndex = d->_proxyMoleculeModel->index(index.row(), DrugInteractorTableModel::ChildrenUuid, index.parent());
    d->_childrenInteractorsStringListModel->setStringList(childrenIndex.data().toString().split(";"));
}

/** React when a search action (WHO, google...) is clicked */
void InteractorEditorWidget::buttonActivated(QAction *selected)
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
