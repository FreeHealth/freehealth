/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class PMH::PmhMode
  \brief Core::IMode to show in the MainWindow
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
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <listviewplugin/fancytreeview.h>
#include <listviewplugin/simplecategorymodel.h>

#include <categoryplugin/categorydialog.h>

#include <translationutils/constanttranslations.h>

#include <QItemSelectionModel>
#include <QToolBar>
#include <QPushButton>

#include "ui_pmhmodewidget.h"


using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}


PmhModeWidget::PmhModeWidget(QWidget *parent) :
        PmhContextualWidget(parent), ui(new Ui::PmhModeWidget), m_EditButton(0)
{
    ui->setupUi(this);
    ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);

    ui->treeView->setActions(0);
    ui->treeView->setCommands(QStringList()
                              << Constants::A_PMH_NEW
                              << Constants::A_PMH_REMOVE
                              << Constants::A_PMH_CATEGORYMANAGER);
    ui->treeView->addContexts(contexts());
    ui->treeView->setModel(pmhCore()->pmhCategoryModel());
    ui->treeView->header()->hide();

    // Actions connected in local widget context
    Core::Command *cmd = 0;
    cmd = actionManager()->command(Constants::A_PMH_REMOVE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(removeItem()));

    // Populate ToolBar and create specific buttons
    m_EditButton = new QPushButton(ui->buttonBox);
    m_EditButton->setText(tr("Edit"));
    ui->buttonBox->addButton(m_EditButton, QDialogButtonBox::YesRole);
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    ui->treeView->hideColumn(PmhCategoryModel::Id);
    ui->treeView->hideColumn(PmhCategoryModel::Type);
    ui->treeView->hideColumn(PmhCategoryModel::EmptyColumn);
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->header()->setResizeMode(PmhCategoryModel::Label, QHeaderView::Stretch);

    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged (QModelIndex, QModelIndex)),
            this, SLOT(currentChanged(QModelIndex, QModelIndex)));
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

PmhModeWidget::~PmhModeWidget()
{
    delete ui;
}

void PmhModeWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid())
        return;

    if (!pmhCore()->pmhCategoryModel()->isCategory(current))
        ui->pmhViewer->setPmhData(pmhCore()->pmhCategoryModel()->pmhDataforIndex(current));
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
            pmhCore()->pmhCategoryModel()->addPmhData(pmh);
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

void PmhModeWidget::createCategory()
{
    Category::CategoryDialog dlg(this);
    dlg.setModal(true);
    dlg.setCategoryModel(pmhCore()->pmhCategoryModel(), PmhCategoryModel::Label);
    dlg.exec();
}

void PmhModeWidget::removeItem()
{
    if (!ui->treeView->selectionModel()->hasSelection())
        return;
    QModelIndex item = ui->treeView->selectionModel()->currentIndex();
    pmhCore()->pmhCategoryModel()->removeRow(item.row(), item.parent());
}

void PmhModeWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        m_EditButton->setText(tr("Edit"));
        break;
    default:
        break;
    }
}




PmhMode::PmhMode(QObject *parent) :
        Core::BaseMode(parent)
{
    setName(tkTr(Trans::Constants::PMHX));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTHISTORY, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_HISTORY);
    setUniqueModeName(Core::Constants::MODE_PATIENT_HISTORY);
//    const QList<int> &context;
//    setContext();

    m_Widget = new PmhModeWidget;
    setWidget(m_Widget);
}

PmhMode::~PmhMode()
{
}

