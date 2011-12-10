/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "addserverdialog.h"
#include <datapackutils/server.h>

#include <translationutils/constanttranslations.h>

#include "ui_addserverdialog.h"

using namespace DataPack;
using namespace Internal;
using namespace Trans::ConstantTranslations;

AddServerDialog::AddServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Internal::Ui::AddServerDialog)
{
    ui->setupUi(this);
    ui->checkUpdate->addItems(checkUpdateLabels());
}

AddServerDialog::~AddServerDialog()
{
    delete ui;
}

void AddServerDialog::setServer(const Server &server)
{
    ui->serverUrl->setText(server.nativeUrl());
    ui->serverType->setCurrentIndex(server.urlStyle());

    ui->userLogin->setText("Not yet implemented");
    ui->userPassword->setText("Not yet implemented");
    ui->checkUpdate->setCurrentIndex(server.userUpdateFrequency());
}

void AddServerDialog::submitTo(Server *server)
{
    Q_ASSERT(server);
    if (!server)
        return;
    server->setUrl(ui->serverUrl->text());
    server->setUrlStyle(Server::UrlStyle(ui->serverType->currentIndex()));
//    server->setUserLogin(ui->userLogin->text());
//    server->setUserPassword(ui->userPassword->text());
    server->setUserUpdateFrequency(ui->checkUpdate->currentIndex());
}
