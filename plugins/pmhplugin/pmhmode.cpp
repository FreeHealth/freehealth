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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Authors:                                                               *
 *  Eric MAEKER <eric.maeker@gmail.com>                                    *
 *  Jerome PINGUET <jerome@jerome.cc                                       *
 ***************************************************************************/
/**
  \class PMH::PmhMode
  \brief Core::IMode to show in the MainWindow for the PMHx plugin.
*/

/**
 * \class PMH::Internal::PmhModeWidget
 * Widget used in the PMH::Internal::PmhMode.
 * Presents a PMHx category tree view and a central widget
 * with a PMH::Internal::PmhViewer and/or a Form::FormPlaceHolder.
 */

#include "pmhbase.h"
#include "pmhmode.h"
#include "pmhmodel.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhcreatordialog.h"
#include "pmhviewer.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/idocumentprinter.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/ipatientbar.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorydialog.h>

#include <formmanagerplugin/constants_db.h>
#include <formmanagerplugin/constants_settings.h>
#include <formmanagerplugin/formcontextualwidget.h>
#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/episodemanager.h>


#include <utils/global.h>
#include <utils/log.h>
#include <utils/widgets/datetimedelegate.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QItemSelectionModel>
#include <QToolBar>
#include <QPushButton>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QFileDialog>

#include "ui_pmhmodewidget.h"

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::EpisodeManager &episodeManager() {return Form::FormCore::instance().episodeManager();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline PmhCategoryModel *catModel() {return PmhCore::instance()->pmhCategoryModel();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline PmhBase *base() {return PmhBase::instance();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

namespace {

const char * const TREEVIEW_SHEET =
        " QTreeView {"
        "    show-decoration-selected: 1;"
        "}"

        "QTreeView::item {"
        "    border: 0px solid #d9d9d9;"
//        "    border-top-color: transparent;"
//        "    border-bottom-color: transparent;"
        "}"

        "QTreeView::item:hover {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 1px solid #bfcde4;"
        "}"

//        "QTreeView::branch:hover {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 0px solid #bfcde4;"
//        "}"

//        "QTreeView::item:selected {"
//        "    border: 1px solid #567dbc;"
//        "}"

        "QTreeView::item:selected {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"

//        "QTreeView::branch:selected {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
//        "}"

//        "QTreeView::item:selected:!active {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
//        "}"
        ;
}

PmhModeWidget::PmhModeWidget(QWidget *parent) :
        PmhContextualWidget(parent),
        ui(new Ui::PmhModeWidget),
        m_EditButton(0),
        m_currentEpisodeModel(0),
        m_proxyModel(0),
        m_episodeToolBar(0),
        m_currentFormIndex(QModelIndex())
{
    ui->setupUi(this);
    ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);
    if (layout()) {
        layout()->setMargin(0);
    }

    ui->formDataMapper->initialize();
    ui->treeView->setActions(0);
    ui->treeView->setCommands(QStringList()
                              << Constants::A_PMH_NEW
                              << Constants::A_PMH_REMOVE
                              /*<< Constants::A_PMH_CATEGORYMANAGER*/);
    ui->treeView->addContext(Core::Context(Constants::C_PMH_PLUGINS));
    ui->treeView->setModel(catModel());
    ui->treeView->header()->hide();
    ui->treeView->setStyleSheet(::TREEVIEW_SHEET);

    ui->episodeView->verticalHeader()->hide();
    ui->episodeView->setFrameStyle(QFrame::NoFrame);
    ui->episodeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->episodeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->episodeView->setItemDelegateForColumn(Form::EpisodeModel::UserDateTime, new Utils::DateTimeDelegate(this, false));
    ui->episodeView->setItemDelegateForColumn(Form::EpisodeModel::CreationDateTime, new Utils::DateTimeDelegate(this, false));

    // Actions connected in local widget context
    Core::Command *cmd = 0;
    cmd = actionManager()->command(Constants::A_PMH_REMOVE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(removeItem()));

    // Populate ToolBar and create specific buttons
    m_EditButton = new QPushButton(ui->buttonBox);
    m_EditButton->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
    ui->buttonBox->addButton(m_EditButton, QDialogButtonBox::YesRole);
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    // Create episode toolbar
    createEpisodeToolbar();

    for(int i=0; i < catModel()->columnCount(); ++i)
        ui->treeView->hideColumn(i);
    ui->treeView->showColumn(PmhCategoryModel::Label);
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->header()->setSectionResizeMode(PmhCategoryModel::Label, QHeaderView::Stretch);

    // connect the aAddPmh action
    cmd = actionManager()->command(Constants::A_PMH_NEW);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(createPmh()));

    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this, SLOT(currentChanged(QModelIndex, QModelIndex)));

    connect(ui->treeView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(pmhModelRowsInserted(QModelIndex,int,int)));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    // by default show synthesis when entering PMH
    ui->stackedWidget->setCurrentWidget(ui->pageSynthesis);

    // TODO: record the user QSplitter settings see issue
    QList<int> splitter;
    splitter << 10 << 300;
    ui->episodeFormSplitter->setSizes(splitter);
}

PmhModeWidget::~PmhModeWidget()
{
    delete ui;
}

/** Return the current selected category id in the category view */
int PmhModeWidget::currentSelectedCategory() const
{
    if (!ui->treeView->selectionModel()->hasSelection())
        return -1;
    QModelIndex item = ui->treeView->selectionModel()->currentIndex();
    while (!catModel()->isCategory(item)) {
        item = item.parent();
    }
    Category::CategoryItem *cat = catModel()->categoryForIndex(item);
    if (cat) {
        return cat->id();
    }
    return -1;
}

/** Reacts on the currentChanged signal of the category tree view */
void PmhModeWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (previous.isValid()) {
        if (catModel()->isForm(previous)) {
            if (ui->formDataMapper->isDirty()) {
                saveCurrentEditingEpisode();
                ui->formDataMapper->submit();
                ui->formDataMapper->clear();
                catModel()->refreshSynthesis();
            }
        }
    }
    if (!current.isValid()) {
        return;
    }
    // No active patient ?
    if (patient()->uuid().isEmpty()) {
        return;
    }

    ui->formDataMapper->setCurrentForm(0);

    if (catModel()->isSynthesis(current)) {
        ui->pmhSynthesisBrowser->setHtml(catModel()->synthesis());
        ui->stackedWidget->setCurrentWidget(ui->pageSynthesis);
    } else if (catModel()->isCategory(current)) {
        ui->pmhSynthesisBrowser->setHtml(catModel()->synthesis(current));
        ui->stackedWidget->setCurrentWidget(ui->pageSynthesis);
    } else if (catModel()->isForm(current)) {
        m_currentFormIndex = current;
        if (ui->episodeView->selectionModel())
            QObject::disconnect(ui->episodeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(episodeChanged(QModelIndex, QModelIndex)));
        // Show the form's widget
        const QString &formUid = catModel()->index(current.row(), PmhCategoryModel::Id, current.parent()).data().toString();
        ui->stackedWidget->setCurrentWidget(ui->formPage);
        ui->formDataMapper->setCurrentForm(formUid);
        //ui->formDataMapper->setLastEpisodeAsCurrent();
        if (m_currentEpisodeModel) {
            QObject::disconnect(m_currentEpisodeModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(refreshPmhSynthesis()));
            //QObject::disconnect(m_currentEpisodeModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(refreshCategoryTreeView()));
        }
        m_currentEpisodeModel = episodeManager().episodeModel(formUid);
        QObject::connect(m_currentEpisodeModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(refreshPmhSynthesis()));
        // as no row is removed from the model (see EpisodeModel::removeEpisode)
        // the signal rowsRemoved will never be emitted:
        //QObject::connect(m_currentEpisodeModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(refreshCategoryTreeView()));
        // create the episodemodel proxymodel
        if (m_proxyModel)
            delete m_proxyModel;
        m_proxyModel = new QSortFilterProxyModel(this);
        m_proxyModel->setSourceModel(m_currentEpisodeModel);
        m_proxyModel->setDynamicSortFilter(true);
        ui->episodeView->setModel(m_proxyModel);


        // set the view columns
        for(int i=0; i < Form::EpisodeModel::MaxData; ++i)
            ui->episodeView->hideColumn(i);
        ui->episodeView->showColumn(Form::EpisodeModel::ValidationStateIcon);
        ui->episodeView->showColumn(Form::EpisodeModel::PriorityIcon);
        ui->episodeView->showColumn(Form::EpisodeModel::UserDateTime);
        ui->episodeView->showColumn(Form::EpisodeModel::CreationDateTime);
        ui->episodeView->showColumn(Form::EpisodeModel::Label);
        ui->episodeView->showColumn(Form::EpisodeModel::UserCreatorName);

        ui->episodeView->horizontalHeader()->setSectionResizeMode(Form::EpisodeModel::ValidationStateIcon,
                                                                  QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(Form::EpisodeModel::PriorityIcon, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(Form::EpisodeModel::UserDateTime, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(Form::EpisodeModel::CreationDateTime, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(Form::EpisodeModel::Label, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(Form::EpisodeModel::UserCreatorName, QHeaderView::Stretch);
        ui->episodeView->horizontalHeader()->setSectionsMovable(true);

        QFont small;
        if (Utils::isRunningOnWin() || Utils::isRunningOnLinux())
            small.setPointSize(small.pointSize() - 1);
        else
            small.setPointSize(small.pointSize() - 4);
        ui->episodeView->horizontalHeader()->setFont(small);
        ui->episodeView->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                                           //                                                           "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                                                           //                                                           "                                  stop:0 #eeeeee, stop: 0.5 #e0e0e0,"
                                                           //                                                           "                                  stop: 0.6 #dadada, stop:1 #f2f2f2);"
                                                           "padding: 2px;"
                                                           //                                                           "border: 1px solid black;"
                                                           "}");

        ui->episodeView->selectionModel()->clearSelection();

        // set the sort order & column to the view/proxymodel
        ui->episodeView->sortByColumn(settings()->value(Form::Constants::S_EPISODEVIEW_SORTEDCOLUMN,
                                                        Form::EpisodeModel::UserDateTime).toInt(),
                                      Qt::SortOrder(settings()->value(Form::Constants::S_EPISODEVIEW_SORTORDER,
                                                                      Qt::DescendingOrder).toInt()));
        ui->episodeView->setSortingEnabled(true);

        checkCurrentEpisodeViewVisibility();
        QObject::connect(ui->episodeView,
                         SIGNAL(activated(QModelIndex)),
                         this,
                         SIGNAL(actionsEnabledStateChanged()));

        QObject::connect(ui->episodeView->selectionModel(),
                         SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                         this,
                         SLOT(episodeChanged(QModelIndex, QModelIndex)));
        QObject::connect(ui->episodeView->selectionModel(),
                         SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                         this,
                         SLOT(actionsEnabledStateChanged()));

        selectAndActivateFirstEpisode();
        Q_EMIT actionsEnabledStateChanged();
    } else if (catModel()->isPmhx(current)) {
        ui->stackedWidget->setCurrentWidget(ui->pageEditor);
        ui->pmhViewer->setPmhData(catModel()->pmhDataforIndex(current));
    }

    ui->scrollArea->horizontalScrollBar()->setValue(0);
    ui->scrollArea->verticalScrollBar()->setValue(0);
}

void PmhModeWidget::onButtonClicked(QAbstractButton *button)
{
    if (button==m_EditButton) {
        ui->pmhViewer->setEditMode(PmhViewer::ReadWriteMode);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
        ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
        return;
    }

    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        {
            // Get the modified PmhData
            Internal::PmhData *pmh = ui->pmhViewer->modifiedPmhData();
            // Inform the model (twice... ??? WTF)
            pmhCore()->pmhCategoryModel()->addPmhData(pmh);
            pmhCore()->pmhCategoryModel()->addPmhData(pmh);
            // Expand the first level of the PMHx
            QModelIndex idx = catModel()->indexForPmhData(pmh);
            ui->treeView->expand(idx);
//            for(int i=0; i < catModel()->rowCount(idx);++i) {
//                ui->treeView->expand(catModel()->index(i,0,idx));
//            }
            ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
            ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
            break;
        }
    case QDialogButtonBox::Cancel:
        ui->pmhViewer->revert();
        ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);
        ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
        break;
    default: break;
    }
}

/** Create a new category in the PMHx using the Category::CategoryDialog */
void PmhModeWidget::createCategory()
{
    Category::CategoryDialog dlg(this);
    dlg.setModal(true);
    dlg.setCategoryModel(catModel(), PmhCategoryModel::Label);
    dlg.exec();
}

void PmhModeWidget::removeItem()
{
    if (!ui->treeView->selectionModel()->hasSelection())
        return;
    QModelIndex item = ui->treeView->selectionModel()->currentIndex();

    // Do not delete categories this way (use the CategoryManagerDialog)
    if (catModel()->isCategory(item) || catModel()->isForm(item))
        return;

    // Get the Root PMHx index
    QModelIndex category;
    while (true) {
        if (catModel()->isCategory(item.parent())) {
            category = item.parent();
            break;
        }
    }
    // Ask user
    bool yes = Utils::yesNoMessageBox(tr("Remove PMHx"),
                                      tr("Do you really want to remove the PMHx called <br />&nbsp;&nbsp;&nbsp;<b>%1</b>?").arg(item.data().toString()));
    if (yes) {
        catModel()->removeRow(item.row(), item.parent());
    }
}

/** When a new patient is selected, select the synthesis item and re-expand the category view */
void PmhModeWidget::onCurrentPatientChanged()
{
    if (ui->formDataMapper->isDirty()) {
        saveCurrentEditingEpisode();
        ui->formDataMapper->submit();
        ui->formDataMapper->clear();
        catModel()->refreshSynthesis();
    }
    m_currentFormIndex = QModelIndex();
    ui->formDataMapper->clear();
    ui->treeView->selectionModel()->select(catModel()->index(0,0), QItemSelectionModel::SelectCurrent);
    ui->treeView->expandAll();
    ui->stackedWidget->setCurrentWidget(ui->pageSynthesis);
}

/** Create a new PMHx using the PMH::PmhCreatorDialog */
void PmhModeWidget::createPmh()
{
    PmhCreatorDialog dlg(this);
    if (ui->treeView->selectionModel()->hasSelection()) {
        QModelIndex item = ui->treeView->selectionModel()->currentIndex();
        while (!catModel()->isCategory(item)) {
            item = item.parent();
        }
        dlg.setCategory(catModel()->categoryForIndex(item));
    }
    Utils::resizeAndCenter(&dlg, Core::ICore::instance()->mainWindow());
    dlg.exec();
}

/** When a PMHx is added to the PmhCategoryModel re-expand all items of the category view */
void PmhModeWidget::pmhModelRowsInserted(const QModelIndex &parent, int start, int end)
{
    ui->treeView->expand(parent);
    for(int i=start; i != end+1; ++i) {
        ui->treeView->expand(catModel()->index(i, PmhCategoryModel::Label, parent));
    }
    catModel()->refreshSynthesis();
    if (catModel()->isSynthesis(ui->treeView->currentIndex())) {
        ui->pmhSynthesisBrowser->setHtml(catModel()->synthesis());
        ui->stackedWidget->setCurrentWidget(ui->pageSynthesis);
    }

}

void PmhModeWidget::showEvent(QShowEvent *event)
{
    LOG(QString("ShowEvent: %1").arg(isVisible() ? "isVisible" : "isHidden"));
    if (ui->formDataMapper->isDirty()) {
        LOG_ERROR("**** During ShowEvent FormDataMapper is Dirty: " + catModel()->formForIndex(m_currentFormIndex)->uuid());
        saveCurrentEditingEpisode();
    }
    selectAndActivateFirstEpisode();
    Q_EMIT actionsEnabledStateChanged();
    QWidget::showEvent(event);
}

void PmhModeWidget::hideEvent(QHideEvent *event)
{
    saveCurrentEditingEpisode();
    QWidget::hideEvent(event);
}

void PmhModeWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        m_EditButton->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
        break;
    default:
        break;
    }
}

void PmhModeWidget::createEpisodeToolbar()
{
    m_episodeToolBar = new QToolBar();
    m_episodeToolBar->setIconSize(QSize(16,16));

    QStringList actions;
    actions << Form::Constants::A_ADDEPISODE
            << Form::Constants::A_RENEWEPISODE
            << "--"
            << Form::Constants::A_REMOVEEPISODE
            << "--"
            << Form::Constants::A_VALIDATEEPISODE
            << "--"
            << Core::Constants::A_FILE_SAVE
            << Core::Constants::A_FILE_PRINT
            << "--"
            << Form::Constants::A_TAKESCREENSHOT;

    Core::Command *cmd = 0;

    foreach(const QString &action, actions) {
        // Actions are created, managed and connected in the Form::Internal::FormActionHandler
        // We just need to add the user visible actions in the toolbar
        if (action=="--") {
            m_episodeToolBar->addSeparator();
            continue;
        }
        cmd = actionManager()->command(Core::Id(action));
        m_episodeToolBar->addAction(cmd->action());
    }

    ui->toolbarLayout->addWidget(m_episodeToolBar);
}

bool PmhModeWidget::saveCurrentEditingEpisode()
{
    if (!ui->formDataMapper->currentEditingEpisodeIndex().isValid()) {
        LOG_FOR(this, "Episode not saved, no current editing episode");
        return true;
    }
    // Something to save?
    if (!ui->formDataMapper->isDirty()) {
        LOG_FOR(this, "Episode not saved, episode is not dirty");
        return true;
    }

    // Autosave or ask user?
    if (!settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, false).toBool()) {
        // Ask user
        bool save = Utils::yesNoMessageBox(QApplication::translate("Form::FormPlaceHolder",
                                                                   "Save episode?"),
                                           QApplication::translate("Form::FormPlaceHolder",
                                                                   "The actual episode has been modified. Do you want to save changes in your database?\n"
                                                                   "Answering 'No' will cause deftialtve data loss."),
                                           "", QApplication::translate("Form::FormPlaceHolder", "Save episode"));
        if (!save)
            return false;
    }
    patient()->patientBar()->showMessage(QApplication::translate("Form::FormPlaceHolder", "Saving episode (%1) from form (%2)")
                                         .arg(ui->formDataMapper->currentEpisodeLabel())
                                         .arg(ui->formDataMapper->currentFormName()));
    bool ok = ui->formDataMapper->submit();
    if (!ok) {
        patient()->patientBar()->showMessage(QApplication::translate("Form::FormPlaceHolder", "WARNING: Episode (%1) from form (%2) can not be saved")
                                             .arg(ui->formDataMapper->currentEpisodeLabel())
                                             .arg(ui->formDataMapper->currentFormName()));
    }
    return ok;
}

/** Return the enabled state of an action. \sa Form::Internal::FormActionHandler */
bool PmhModeWidget::enableAction(WidgetAction action) const
{
    if (!m_currentEpisodeModel) {
        return false;
    }
    if(!catModel())
        return false;
    if(!m_currentFormIndex.isValid())
        return false;

    switch (action) {
    case Action_Clear:
        // Clear only if a form && an episode are selected
        return (ui->episodeView->selectionModel()->hasSelection()
                && catModel()->isForm(ui->treeView->currentIndex()));
    case Action_CreateEpisode:
    {
        // No form
        if (!catModel()->formForIndex(m_currentFormIndex))
            return false;
        if (catModel()->formForIndex(m_currentFormIndex)->episodePossibilities() == Form::FormMain::NoEpisode) {
            return false;
        }
        // Unique episode, already 1 episode
        if ((catModel()->formForIndex(m_currentFormIndex)->episodePossibilities() == Form::FormMain::UniqueEpisode)
                && (m_currentEpisodeModel->rowCount() > 0)) {
            return false;
        }
        return true;
    }
    case Action_ValidateCurrentEpisode:
    {
        // Validate episode only if
        // - an episode is selected
        // - && episode not already validated
        // - && form is not NoEpisode
        return (ui->episodeView->selectionModel()->hasSelection()
                && !m_currentEpisodeModel->isEpisodeValidated(ui->formDataMapper->currentEditingEpisodeIndex())
                && !(catModel()->formForIndex(m_currentFormIndex)->episodePossibilities() == Form::FormMain::NoEpisode));
    }
    case Action_SaveCurrentEpisode:
        // Save episode only if
        // - an episode is selected
        // - episode is not validated
        return (ui->episodeView->selectionModel()->hasSelection()
                && !m_currentEpisodeModel->isEpisodeValidated(ui->formDataMapper->currentEditingEpisodeIndex()));
    case Action_RemoveCurrentEpisode:
    {
        // Remove episode only if
        // - not NoEpisode
        // - an episode is selected
        return (ui->episodeView->selectionModel()->hasSelection());
    }
    case Action_RenewCurrentEpisode:
    {
        // Renew an episode only if
        // - form is multi-episode
        // - an episode is selected
        return ((catModel()->formForIndex(m_currentFormIndex)->episodePossibilities() == Form::FormMain::MultiEpisode)
                && ui->episodeView->selectionModel()->hasSelection());
    }
    case Action_TakeScreenShot:
    {
        // Take screenshot
        // - an episode is selected
        return (ui->episodeView->selectionModel()->hasSelection());
    }
    case Action_PrintCurrentFormEpisode:
        // Print episode only if an episode is selected
        return ui->episodeView->selectionModel()->hasSelection();
    }  // end switch
    return false;
}

/** Clear the form content. The current episode (if one was selected) is not submitted to the model. */
bool PmhModeWidget::clear()
{
    ui->formDataMapper->clear();
    //    d->_currentEditingForm = QModelIndex();
    //    Q_EMIT actionsEnabledStateChanged();
    return true;
}

/**
 * Creates a new episode for the current selected form.
 * Returns true in case of success.
 * Connected to Form::Internal::FormActionHandler
 * \sa Form::EpisodeModel::insertRow()
 */
bool PmhModeWidget::createEpisode()
{
    QModelIndex currentForm = ui->treeView->currentIndex();
    if (!catModel()->isForm(currentForm))
        return false;

    // autosave feature
    if (m_currentEpisodeModel) {
        if (!saveCurrentEditingEpisode()) {
            LOG_ERROR("Unable to save current episode");
            return false;
        }
    } else {
        Q_ASSERT(false);
    }

    // get the form
    //QModelIndex index = d->ui->formView->selectionModel()->selectedIndexes().at(0);
    if (catModel()->formForIndex(m_currentFormIndex)->episodePossibilities() == Form::FormMain::NoEpisode) {
        LOG_ERROR("Can not create an episode on NoEpisode forms");
        return false;
    }

    if ((catModel()->formForIndex(m_currentFormIndex)->episodePossibilities() == Form::FormMain::UniqueEpisode)
            && (m_currentEpisodeModel->rowCount() > 0)) {
        LOG_ERROR("Cannot create more than one episode on IsUniqueEpisode forms");
        return false;
    }
    //setCurrentForm(m_currentFormIndex);

    // create a new episode the selected form and its children
    // FIXME: see bool EpisodeModel::validateEpisode(const QModelIndex &index)
    m_currentEpisodeModel->setReadOnly(false);
    if (!m_currentEpisodeModel->insertRow(m_currentEpisodeModel->rowCount())) {
        LOG_ERROR("Unable to create new episode");
        return false;
    }

    // activate the newly created main episode
    QModelIndex source = m_currentEpisodeModel->index(m_currentEpisodeModel->rowCount() - 1, Form::EpisodeModel::Label);
    QModelIndex proxy = m_proxyModel->mapFromSource(source);

    ui->episodeView->selectRow(proxy.row());
    ui->formDataMapper->setCurrentEpisode(source);

    Q_EMIT actionsEnabledStateChanged();
    return true;
}

bool PmhModeWidget::validateCurrentEpisode()
{
    if (!ui->episodeView->selectionModel()->hasSelection())
        return false;

    // message box
    bool yes = Utils::yesNoMessageBox(tr("Validate the current episode"),
                                      tr("When you validate an episode, you"
                                         " freeze it permanently. The episode"
                                         " will still be visible but no"
                                         " modification will be possible.\n"
                                         "Do you really want to validate the current episode?"));
    if (!yes)
        return false;

    // get the episodeModel corresponding to the currently selected form
    if (!m_currentEpisodeModel) {
        return false;
    }
    if (!saveCurrentEditingEpisode()) {
        LOG_ERROR("Unable to save current episode");
        return false;
    }

    // validate episode
    bool ok = m_currentEpisodeModel->validateEpisode(ui->formDataMapper->currentEditingEpisodeIndex());
    if (ok) {
        patient()->patientBar()->showMessage(tr("Episode (%1) from form (%2) signed")
                                             .arg(ui->formDataMapper->currentEpisodeLabel())
                                             .arg(ui->formDataMapper->currentFormName()));
    }
    Q_EMIT actionsEnabledStateChanged();
    return ok;
}

/*!
  Renew the currently selected episode. This member takes the content of
  the current episode and use it to create a new one. Only the date, episode label
  and user creator changes.
  \sa EpisodeModel::renewEpisode
  \sa FormPlaceHolder::renewEpisode
  */
bool PmhModeWidget::renewEpisode()
{
    if (!ui->episodeView->selectionModel()->hasSelection())
        return false;

    // message box
    bool yes = Utils::yesNoMessageBox(tr("Renew the current episode"),
                                      tr("Create a new episode with today's date"
                                         " and the same content as the"
                                         " selected episode.<br />"
                                         "Do you want to renew the selected episode?"));
    if (!yes)
        return false;


    // get the episodeModel corresponding to the currently selected form
    if (!m_currentEpisodeModel)
        return false;
    if (!saveCurrentEditingEpisode()) {
        LOG_ERROR("Unable to save current episode");
        return false;
    }

    // renew an episode
    QModelIndex newEpisode = m_currentEpisodeModel->renewEpisode(ui->formDataMapper->currentEditingEpisodeIndex());
    if (newEpisode.isValid()) {
        // message
        patient()->patientBar()->showMessage(tr("Episode (%1) from form (%2) renewed")
                                             .arg(ui->formDataMapper->currentEpisodeLabel())
                                             .arg(ui->formDataMapper->currentFormName()));

        // select the newly created episode
        QModelIndex proxy = m_proxyModel->mapFromSource(newEpisode);
        ui->episodeView->selectRow(proxy.row());
        //d->_formTreeModel->updateFormCount(d->_currentEditingForm);
    }
    Q_EMIT actionsEnabledStateChanged();
    return newEpisode.isValid();
}

/*!
  Save the currently selected episode (episode content is submitted to its Form::EpisodeModel).
  Are saved:
  - the user datetime
  - the user label
  - the priority
  - the XML content
  Return false in case of error.
  Connected to Form::Internal::FormActionHandler.
  \sa Form::EpisodeModel::submit()
  */
bool PmhModeWidget::saveCurrentEpisode()
{
    bool ok = saveCurrentEditingEpisode();
    Q_EMIT actionsEnabledStateChanged();
    return ok;
}

/*!
  Remove the currently selected episode
  Return false in case of error.
  Connected to Form::Internal::FormActionHandler.
  \sa Form::EpisodeModel::removeEpisode()
  */
bool PmhModeWidget::removeCurrentEpisode()
{
#ifdef WITH_EPISODE_REMOVAL
    // message box
    bool yes = Utils::yesNoMessageBox(tr("Remove the current episode"),
                                      tr("You cannot completely destroy an episode, "
                                         "but you can remove it from the views.\n"
                                         "The episode will not be shown anymore, but will still be "
                                         "recorded in the database.\n"
                                         "Do you really want to remove the current episode?"));
    if (!yes)
        return false;
    bool ok = m_currentEpisodeModel->removeEpisode(ui->formDataMapper->currentEditingEpisodeIndex());
    if (ok)
        patient()->patientBar()->showMessage(tr("Episode (%1) from form (%2) removed")
                                             .arg(ui->formDataMapper->currentEpisodeLabel())
                                             .arg(ui->formDataMapper->currentFormName()));

    //d->_formTreeModel->updateFormCount(d->_currentEditingForm);
    ui->formDataMapper->clear();
    ui->formDataMapper->setEnabled(false);
    Q_EMIT actionsEnabledStateChanged();
    refreshPmhSynthesis();
    selectAndActivateFirstEpisode();
    return ok;
#else
    return false;
#endif
}

void PmhModeWidget::checkCurrentEpisodeViewVisibility()
{
    if (!catModel())
        return;
    // Assuming the _currentEditingForm is defined and the episodeView model is set
    bool visible = true;

    if (catModel()->formForIndex(m_currentFormIndex)->isNoEpisode())
        visible = false;
    ui->episodeView->setVisible(visible);
}

/*!
  Take a screenshot of the currently selected form and episode.
  Return false in case of error.
  Connected to Form::Internal::FormActionHandler.
  */
bool PmhModeWidget::takeScreenshotOfCurrentEpisode()
{
    QString fileName = QFileDialog::getSaveFileName(this, tkTr(Trans::Constants::SAVE_FILE),
                                                    settings()->path(Core::ISettings::UserDocumentsPath),
                                                    tr("Images (*.png)"));
    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        if (info.completeSuffix().isEmpty())
            fileName.append(".png");
        QPixmap pix = ui->formDataMapper->screenshot();
        return pix.save(fileName);
    }
    return false;
}

/*!
  Print the current editing episode. Return false in case of error.
  In order to print an episode (not an empty form), this member uses the
  Form::FormItemSpec::Spec_HtmlPrintMask of the current editing form. \n
  Connected to Form::Internal::FormActionHandler
  */
bool PmhModeWidget::printFormOrEpisode()
{
    if (!m_currentFormIndex.isValid())
        return false;
    Form::FormMain *formMain = catModel()->formForIndex(m_currentFormIndex);
    if (!formMain)
        return false;
    Core::IDocumentPrinter *p = printer();
    if (!p) {
        LOG_ERROR("No IDocumentPrinter found");
        return false;
    }

    QString title = formMain->spec()->label();
    QString htmlToPrint = formManager().formPrintHtmlOutput(formMain);

    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, title);
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);

    // print
    if (formMain->isNoHeader()) {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Empty_User, false);
    } else if (formMain->isAdministrativeHeader()) {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Administrative_User, false);
    } else if (formMain->isPrescriptionHeader()) {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Prescription_User, false);
    } else {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Generic_User, false);
    }
    return true;
}

void PmhModeWidget::selectAndActivateFirstEpisode()
{
    // Assuming the _currentEditingForm is defined and the episodeView model is set
    if (ui->episodeView->selectionModel()
            && !ui->episodeView->selectionModel()->hasSelection()) {
        ui->formDataMapper->setCurrentEpisode(QModelIndex());
        if (ui->episodeView->model()->rowCount() > 0) {
            ui->episodeView->selectRow(0);
            // force episodeChanged
            episodeChanged(ui->episodeView->currentIndex(), QModelIndex());
            ui->formDataMapper->setFormWidgetEnabled(true);
        } else {
            ui->formDataMapper->setFormWidgetEnabled(false);
        }
    }
}

void PmhModeWidget::refreshPmhSynthesis()
{
    catModel()->refreshSynthesis();
}

PmhMode::PmhMode(QObject *parent) :
    Core::IMode(parent),
    m_inPluginManager(false)
{
    setDisplayName(tkTr(Trans::Constants::PMHX));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTHISTORY, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_HISTORY);
    setId(Core::Constants::MODE_PATIENT_HISTORY);
    setPatientBarVisibility(true);
    setEnabledOnlyWithCurrentPatient(true);

//    const QList<int> &context;
//    setContext();

    m_Widget = new PmhModeWidget;
    setWidget(m_Widget);
    onCurrentPatientChanged();
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
}

void PmhModeWidget::episodeChanged(const QModelIndex &current, const QModelIndex &previous)
{
    LOG(QString("episodeChanged: current(valid:%1) ; previous(valid:%2)").arg(current.isValid()).arg(previous.isValid()));
    // Autosave is problematic when patient changed
    QModelIndex sourceCurrent = m_proxyModel->mapToSource(current);
    QModelIndex sourcePrevious = m_proxyModel->mapToSource(previous);
    if (sourcePrevious.isValid())
        saveCurrentEditingEpisode();
    clear();
    if (sourceCurrent.isValid()) {
        ui->formDataMapper->setCurrentEpisode(sourceCurrent);
        ui->formDataMapper->setEnabled(true);
    } else {
        ui->formDataMapper->clear();
        ui->formDataMapper->setEnabled(false);
    }
    //    warnPatient();
    Q_EMIT actionsEnabledStateChanged();
}

/** Return true is the Form::Internal::FormDataWidgetMapper included in the view is dirty */
bool PmhModeWidget::isDirty() const
{
    if (m_currentFormIndex.isValid())
            //&& ui->formDataMapper->currentEditingEpisodeIndex().isValid())
        return ui->formDataMapper->isDirty();
    return false;
}

/**
 * Returns the label of the currently selected form (mainly used for debugging purpose).
 * \sa Forms::Internal::FormPlaceHolderCoreListener
 */
QString PmhModeWidget::currentFormLabel() const
{
/*    if (catModel() && m_currentFormIndex.isValid()) {
        QModelIndex index = catModel()->index(d->_currentEditingForm.row(), FormTreeModel::Label, d->_currentEditingForm.parent());
        return d->_formTreeModel->data(index).toString();
    }*/
    return QString::null;
}

PmhMode::~PmhMode()
{
    if (m_inPluginManager) {
        pluginManager()->removeObject(this);
    }
}

QString PmhMode::name() const
{
    return tkTr(Trans::Constants::PMHX);
}

void PmhMode::onCurrentPatientChanged()
{
    if (!m_inPluginManager) {
        pluginManager()->addObject(this);
        m_inPluginManager = true;
    }
}

PmhModeWidgetCoreListener::PmhModeWidgetCoreListener(PmhModeWidget *parent) :
    Core::ICoreListener(parent),
    _pmhModeWidget(parent)
{
    Q_ASSERT(parent);
    setObjectName("PmhModeWidgetCoreListener");
}

PmhModeWidgetCoreListener::~PmhModeWidgetCoreListener() {}

bool PmhModeWidgetCoreListener::coreAboutToClose()
{
    _errorMsg.clear();
    if (_pmhModeWidget->isDirty()) {
        bool ok = _pmhModeWidget->saveCurrentEpisode();
        if (!ok) {
            _errorMsg = tr("Unable to save current episode, form: %1")
                    /*.arg(_pmhModeWidget->currentFormLabel())*/;
            LOG_ERROR(_errorMsg);
        }
        return ok;
    }
    return true;
}
