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
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <listviewplugin/fancytreeview.h>
#include <listviewplugin/simplecategorymodel.h>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QItemSelectionModel>

#include "ui_pmhmode.h"


using namespace PMH;
using namespace Internal;

static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

PmhMode::PmhMode(QObject *parent) :
        Core::BaseMode(parent), ui(new Ui::PmhMode)
{
    setName(tr("Medical History"));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTHISTORY, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_HISTORY);
    setUniqueModeName(Core::Constants::MODE_PATIENT_HISTORY);
//    const QList<int> &context;
//    setContext();

    QWidget *m_Widget = new QWidget;
    ui->setupUi(m_Widget);
    ui->treeView->setModel(pmhCore()->pmhCategoryModel(), PmhCategoryModel::EmptyColumn);
//    ui->treeView->setModel(new PmhModel(this), PmhModel::MH_EmptyColumn);
//    ui->treeView->setModel(new Views::SimpleCategoryModel("", this));
    ui->treeView->setButtonActions(Views::FancyTreeView::FTV_SaveModel |  Views::FancyTreeView::FTV_CreateNew);
    ui->treeView->useContextMenu(true);

    QPushButton *addButton = new QPushButton("Add", m_Widget);
    ui->gridLayout->addWidget(addButton);
    setWidget(m_Widget);

    connect(addButton, SIGNAL(clicked()), this, SLOT(on_addButton_clicked()));
    connect(ui->treeView->treeView()->selectionModel(), SIGNAL(currentChanged (QModelIndex, QModelIndex)),
            this, SLOT(currentChanged(QModelIndex, QModelIndex)));
}

PmhMode::~PmhMode()
{
    delete ui;
}

void PmhMode::on_addButton_clicked()
{
    // Create a viewer
    PmhCreatorDialog dlg;
    dlg.exec();
}

void PmhMode::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    ui->pmhViewer->setPmhData(pmhCore()->pmhCategoryModel()->pmhDataforIndex(current));
}
