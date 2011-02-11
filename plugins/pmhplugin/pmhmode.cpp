/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "pmhmode.h"
#include "pmhmodel.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhcreatordialog.h"
#include "pmhviewer.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <listviewplugin/fancytreeview.h>
#include <listviewplugin/simplecategorymodel.h>

#include <QItemSelectionModel>

#include "ui_pmhmode.h"


using namespace PMH;
using namespace Internal;

static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

PmhMode::PmhMode(QObject *parent) :
        Core::BaseMode(parent), ui(new Ui::PmhMode)
{
    setName(tr("Medical History"));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTHISTORY, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_HISTORY);
    setUniqueModeName(Core::Constants::MODE_PATIENT_HISTORY);
//    const QList<int> &context;
//    setContext();

    m_Widget = new QWidget;
    ui->setupUi(m_Widget);
    ui->pmhViewer->setEditMode(PmhViewer::ReadWriteMode);

    ui->patientBarLayout->addWidget(patient()->newPatientBar(m_Widget));
    ui->treeView->setModel(pmhCore()->pmhCategoryModel(), PmhCategoryModel::EmptyColumn);
//    ui->treeView->setModel(new PmhModel(this), PmhModel::MH_EmptyColumn);
//    ui->treeView->setModel(new Views::SimpleCategoryModel("", this));
    ui->treeView->setButtonActions(Views::FancyTreeView::FTV_CreateNew);
    ui->treeView->useContextMenu(true);
    ui->treeView->treeView()->hideColumn(PmhCategoryModel::Id);
    ui->treeView->treeView()->hideColumn(PmhCategoryModel::Type);
    ui->treeView->treeView()->header()->setResizeMode(PmhCategoryModel::Label, QHeaderView::Stretch);

//    QPushButton *addButton = new QPushButton("Add", m_Widget);
//    ui->gridLayout->addWidget(addButton);
    setWidget(m_Widget);

    connect(ui->treeView, SIGNAL(addItemRequested()), this, SLOT(on_addButton_clicked()));
    connect(ui->treeView->treeView()->selectionModel(), SIGNAL(currentChanged (QModelIndex, QModelIndex)),
            this, SLOT(currentChanged(QModelIndex, QModelIndex)));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

PmhMode::~PmhMode()
{
    delete ui;
}

void PmhMode::on_addButton_clicked()
{
    // Create a new Pmh in the creator dialog
    PmhCreatorDialog dlg;
    dlg.exec();
}

void PmhMode::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    ui->pmhViewer->setPmhData(pmhCore()->pmhCategoryModel()->pmhDataforIndex(current));
}

void PmhMode::onButtonClicked(QAbstractButton *button)
{
    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        {
            // Get the modified PmhData
            PmhData *pmh = ui->pmhViewer->modifiedPmhData();
            // Inform the model
            pmhCore()->pmhCategoryModel()->addPmhData(pmh);
            break;
        }
    case QDialogButtonBox::Cancel: ui->pmhViewer->revert(); break;
    default: break;
    }
}
