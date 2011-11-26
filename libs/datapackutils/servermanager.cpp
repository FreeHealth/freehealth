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

#include <QNetworkConfigurationManager>

#include <QDebug>
#include <QNetworkRequest>

using namespace DataPack;

ServerManager::ServerManager(QObject *parent) :
    QObject(parent)
{
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

int ServerManager::addServer(const QUrl &url)
{
    // TODO check if a server already exists with the same URL
    int id = getFreeServerId();
    m_servers.insert(id, Server(url));
    return id;
}

int ServerManager::getFreeServerId() const
{
    bool free;
    int id = 0;
    do {
        free = true;
        foreach (int idLoop, m_servers.keys()){
            if (idLoop == id) {
                free = false;
                break;
            }
        }
    } while (!free);
    return id;
}

int ServerManager::connectAndUpdate(int id)
{
    if (m_servers.find(id) == m_servers.end()) // id unknown?
        return -1;

    const Server &server = m_servers[id];

    QNetworkRequest request(server.url()); // TODO ADD THE FILES TO THE URL
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
}
