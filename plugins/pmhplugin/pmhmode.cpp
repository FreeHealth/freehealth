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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class PMH::PmhMode
  \brief Core::IMode to show in the MainWindow for the PMHx plugin.
*/

/**
 * \class PMH::Internal::PmhModeWidget
 * Widget used in the PMH::Internal::PmhMode.
 * Presents a pmhx category tree view and a central widget
 * with a PMH::Internal::PmhViewer and/or a Form::FormPlaceHolder.
 */

#include "pmhmode.h"
#include "pmhmodel.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhcreatordialog.h"
#include "pmhviewer.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorydialog.h>

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QItemSelectionModel>
#include <QToolBar>
#include <QPushButton>
#include <QScrollBar>

#include "ui_pmhmodewidget.h"

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }

static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline PmhCategoryModel *catModel() {return PmhCore::instance()->pmhCategoryModel();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}

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
        m_EditButton(0)
{
    ui->setupUi(this);
    ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);
    ui->treeViewLayout->setMargin(0);
    layout()->setMargin(0);

    ui->formDataMapper->initialize();
    ui->treeView->setActions(0);
    ui->treeView->setCommands(QStringList()
                              << Constants::A_PMH_NEW
                              << Constants::A_PMH_REMOVE
                              << Constants::A_PMH_CATEGORYMANAGER);
    ui->treeView->addContext(Core::Context(Constants::C_PMH_PLUGINS));
    ui->treeView->setModel(catModel());
    ui->treeView->header()->hide();
    ui->treeView->setStyleSheet(::TREEVIEW_SHEET);

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

    for(int i=0; i < catModel()->columnCount(); ++i)
        ui->treeView->hideColumn(i);
    ui->treeView->showColumn(PmhCategoryModel::Label);
    ui->treeView->header()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    ui->treeView->header()->setResizeMode(PmhCategoryModel::Label, QHeaderView::Stretch);
#else
    // Qt5
    ui->treeView->header()->setSectionResizeMode(PmhCategoryModel::Label, QHeaderView::Stretch);
#endif

    // connect the aAddPmh action
    cmd = actionManager()->command(Constants::A_PMH_NEW);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(createPmh()));

    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this, SLOT(currentChanged(QModelIndex, QModelIndex)));

    connect(ui->treeView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(pmhModelRowsInserted(QModelIndex,int,int)));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
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
    // Auto-saved form content
    if (previous.isValid()) {
        if (catModel()->isForm(previous)) {
            if (ui->formDataMapper->isDirty())
                ui->formDataMapper->submit();
            ui->formDataMapper->clear();
            catModel()->refreshSynthesis();
        }
    }
    if (!current.isValid())
        return;
    // No active patient ?
    if (patient()->uuid().isEmpty())
        return;

    ui->formDataMapper->setCurrentForm(0);

    if (catModel()->isSynthesis(current)) {
        ui->pmhSynthesisBrowser->setHtml(catModel()->synthesis());
        ui->stackedWidget->setCurrentWidget(ui->pageSynthesis);
    } else if (catModel()->isCategory(current)) {
        ui->pmhSynthesisBrowser->setHtml(catModel()->synthesis(current));
        ui->stackedWidget->setCurrentWidget(ui->pageSynthesis);
    } else if (catModel()->isForm(current)) {
        // Show the form's widget
        const QString &formUid = catModel()->index(current.row(), PmhCategoryModel::Id, current.parent()).data().toString();
        ui->stackedWidget->setCurrentWidget(ui->formPage);
        ui->formDataMapper->setCurrentForm(formUid);
        ui->formDataMapper->setLastEpisodeAsCurrent();
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
            PmhData *pmh = ui->pmhViewer->modifiedPmhData();
            // Inform the model
            catModel()->addPmhData(pmh);
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
    while (true) {
        if (catModel()->isCategory(item.parent()))
            break;
        item = item.parent();
    }

    // Ask user
    bool yes = Utils::yesNoMessageBox(tr("Remove PMHx"),
                                      tr("Do you really want to remove the PMHx called <br />&nbsp;&nbsp;&nbsp;<b>%1</b>?").arg(item.data().toString()));
    if (yes)
        catModel()->removeRow(item.row(), item.parent());
}

/** When a new patient is selected, select the synthesis item and re-expand the category view */
void PmhModeWidget::onCurrentPatientChanged()
{
    // Auto-saved form content
    if (ui->formDataMapper->isDirty())
        ui->formDataMapper->submit();
    ui->formDataMapper->clear();
    catModel()->refreshSynthesis();
    ui->treeView->selectionModel()->select(catModel()->index(0,0), QItemSelectionModel::SelectCurrent);
    ui->treeView->expandAll();
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
}

void PmhModeWidget::hideEvent(QHideEvent *event)
{
    // Auto-saved form content
    if (isVisible() && ui->formDataMapper->isDirty()) {
        ui->formDataMapper->submit();
        catModel()->refreshSynthesis();
    }
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

