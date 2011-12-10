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

#include <translationutils/constants.h>

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
    // Do not change the order
    ui->serverType->addItem(tr("Default FreeMedForms server (mirror)")); // 0
    ui->serverType->addItem(tr("Local file"));                           // 1
    ui->serverType->addItem(tr("HTTP (standard mode)"));                 // 2
    ui->serverType->addItem(tr("FTP (standard mode)"));                  // 3
    ui->serverType->addItem(tr("Protected HTTP with zipped content"));   // 4
    ui->serverType->addItem(tr("Protected HTTP non-zipped"));            // 5
    ui->serverType->addItem(tr("FTP with zipped content"));              // 6
}

AddServerDialog::~AddServerDialog()
{
    delete ui;
}

static Server::UrlStyle urlType(QComboBox *box)
{
    Q_ASSERT(box);
    switch (box->currentIndex()) {
    case 0: return Server::HttpPseudoSecuredAndZipped;
    case 1: return Server::NoStyle;
    case 2: return Server::Http;
    case 3: return Server::Ftp;
    case 4: return Server::HttpPseudoSecuredAndZipped;
    case 5: return Server::HttpPseudoSecuredNotZipped;
    case 6: return Server::FtpZipped;
    }
    return Server::NoStyle;
}

static void setUrlType(QComboBox *box, int urlType)
{
    Q_ASSERT(box);
    switch (urlType) {
    case Server::NoStyle: box->setCurrentIndex(1); break;
    case Server::HttpPseudoSecuredAndZipped: box->setCurrentIndex(4); break;
    case Server::HttpPseudoSecuredNotZipped: box->setCurrentIndex(5); break;
    case Server::Http: box->setCurrentIndex(2); break;
    case Server::FtpZipped: box->setCurrentIndex(6); break;
    case Server::Ftp: box->setCurrentIndex(3); break;
    }
}

void AddServerDialog::setServer(const Server &server)
{
    ui->serverUrl->setText(server.nativeUrl());
    setUrlType(ui->serverType, server.urlStyle());

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
    server->setUrlStyle(urlType(ui->serverType));
//    server->setUserLogin(ui->userLogin->text());
//    server->setUserPassword(ui->userPassword->text());
    server->setUserUpdateFrequency(ui->checkUpdate->currentIndex());
}
