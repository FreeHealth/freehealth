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

#include "servermanager_p.h"
#include "servermanager.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QNetworkConfigurationManager>
#include <QNetworkRequest>
#include <QDir>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

ServerManager::ServerManager(const QString &filesCachePath, QObject *parent) :
    QObject(parent),
	m_d(new ServerManagerPrivate)
{
    setObjectName("ServerManager");
    if (!QDir(filesCachePath).exists()) {
        LOG_ERROR(tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(filesCachePath));
    }
	m_d->filesCachePath = QDir::cleanPath(filesCachePath);
}

bool ServerManager::isInternetConnexionAvailable()
{
//    foreach(const QNetworkConfiguration &conf, QNetworkConfigurationManager().allConfigurations()) {
//        qWarning() << conf.bearerName() << conf.bearerTypeName() << conf.state() << conf.identifier() << conf.name();
//    }
    // TODO
    return true;
}

void ServerManager::connectServer(const Server &server, const ServerIdentification &ident)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(ident);
    if (server.isLocalPath()) {
//        server.setConnected(true);
        return;
    }
}

ServerDescription ServerManager::downloadServerDescription(const Server &server)
{
    // TODO
    if (server.isLocalPath()) {

    }
    ServerDescription desc;
    return desc;
}

QList<PackDescription> ServerManager::downloadPackDescription(const Server &server, const Pack &pack)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    QList<PackDescription> list;
    return list;
}

Pack ServerManager::downloadAndUnzipPack(const Server &server, const Pack &pack)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    return pack;
}

bool ServerManager::installDataPack(const Server &server, const Pack &pack)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    return false;
}

bool ServerManager::addServer(const QUrl &url)
{
	return m_d->addServer(url);
}

Server *ServerManager::getServerAt(int index) const
{
	return qobject_cast<Server*>(m_d->children()[index]);
//    return m_d->servers[index];
}

int ServerManager::getServerIndex(const QUrl &url) const
{
	return m_d->getServerIndex(url);
}

void ServerManager::removeServerAt(int index)
{
	delete m_d->children()[index];
}

void ServerManager::connectAndUpdate(int index)
{
	m_d->connectAndUpdate(index);
}
