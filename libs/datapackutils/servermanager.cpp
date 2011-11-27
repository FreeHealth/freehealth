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
#include "servermanager.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QNetworkConfigurationManager>
#include <QNetworkRequest>
#include <QDir>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

ServerManager::ServerManager(const QString &backUpPath, QObject *parent) :
    QObject(parent)
{
    setObjectName("ServerManager");
    if (!QDir(backUpPath).exists()) {
        LOG_ERROR(tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(backUpPath));
    }
    m_BackUpPath = QDir::cleanPath(backUpPath);
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
    // check if a server already exists with the same URL
    foreach (const Server &server, m_servers)
        if (server.url() == url)
            return false;

    m_servers << Server(url);
    return true;
}

const Server &ServerManager::getServerAt(int index) const
{
    return m_servers[index];
}

int ServerManager::getServerIndex(const QUrl &url) const
{
    for (int i = 0; i < m_servers.count(); i++)
    {
        const Server &server = m_servers[i];
        if (server.url() == url)
            return i;
    }
    return -1;
}

void ServerManager::removeServerAt(int index)
{
    // TODO stop all jobs linked to the server if there are running ones
    m_servers.removeAt(index);
}

bool ServerManager::connectAndUpdate(int index)
{
    if (index < 0 || index >= m_servers.count()) // index out-of-bounds
        return false;

    const Server &server = m_servers[index];

    // Forge the server config filename
    QUrl requestUrl = server.url();
    QDir dir(server.url().path());
    requestUrl.setPath(dir.filePath("server.conf.xml"));

    QNetworkRequest request(requestUrl); // TODO ADD THE FILES TO THE URL
    request.setRawHeader("User-Agent", "FreeMedForms"); // TODO specify the version?
    QNetworkReply *reply = m_networkAccessManager.get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(requestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(requestError(QNetworkReply::NetworkError)));
}

void ServerManager::requestReadyRead()
{
    // TODO
}

void ServerManager::requestError(QNetworkReply::NetworkError error)
{
    // TODO
    // Save the config file to the m_BackUpPath
}
