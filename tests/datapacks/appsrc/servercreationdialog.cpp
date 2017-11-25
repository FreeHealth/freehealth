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
 *  Main Developers:                                                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "servercreationdialog.h"
#include "ui_servercreationdialog.h"

#include <datapackutils/servercreation/packcreationmodel.h>

#include <QPushButton>
#include <QDebug>

ServerCreationDialog::ServerCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerCreationDialog)
{
    ui->setupUi(this);
    _packCreationModel = new DataPack::PackCreationModel(this);
    // _packCreationModel->initialize();
    ui->treeView->setModel(_packCreationModel);

    ui->pathChooser->setPath("/home/eric/freedddimanager/Documents/DataPacks");
    // Connect UI
    QPushButton *apply = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(apply, SIGNAL(clicked()), this, SLOT(screenNewPath()));

    QPushButton *checkedToConsole = ui->buttonBox->addButton(tr("checked to console"), QDialogButtonBox::ActionRole);
    connect(checkedToConsole, SIGNAL(clicked()), this, SLOT(onCheckedToConsole()));

}

ServerCreationDialog::~ServerCreationDialog()
{
    delete ui;
}

void ServerCreationDialog::screenNewPath()
{
    _packCreationModel->addScreeningPath(ui->pathChooser->path());
}

void ServerCreationDialog::onCheckedToConsole()
{
    qDebug() << _packCreationModel->getCheckedPacks();
}
