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
#ifndef DATAPACK_INTERNAL_HTTPSERVERENGINE_H
#define DATAPACK_INTERNAL_HTTPSERVERENGINE_H

#include <datapackutils/iserverengine.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace DataPack {
namespace Internal {
class ServerManager;

// this private struct contains all data associated with a QNetworkReply. It will be set as a user attribute in the QNetworkReply at its creation (see QNetworkReply::setAttribute()).
struct ReplyData {
    ReplyData(){}
    ReplyData(QNetworkReply *reply, Server *server, Server::FileRequested fileType);
    QNetworkReply *reply;
    Server *server;
    QByteArray response;
    Server::FileRequested fileType; // a configuration file? a pack file? etc
};

class HttpServerEngine : public IServerEngine
{
    Q_OBJECT
public:
    explicit HttpServerEngine(IServerManager *parent);
    ~HttpServerEngine();

    // IServerEngine interface
    bool managesServer(const Server &server);
    void addToDownloadQueue(const ServerEngineQuery &query);
    int downloadQueueCount() const;
    bool startDownloadQueue();

private Q_SLOTS:
    void serverReadyRead();
    void serverError(QNetworkReply::NetworkError error);
    void serverFinished();

private:
    ServerManager *serverManager();

    void afterServerConfigurationDownload(const ReplyData &data); // called after a server configuration file download
    void afterPackDescriptionFileDownload(const ReplyData &data); // called after a pack description file download
    void afterPackFileDownload(const ReplyData &data); // called after a pack file download

    void createPackAndRegisterToServerManager(const Server &server, const QString &pathToPackDescription);

    QNetworkRequest createRequest(const QString &url); // create a request with good headers


private:
    QList<ServerEngineQuery> m_queue;
    QNetworkAccessManager *m_NetworkAccessManager;
    QHash<QNetworkReply*,ReplyData> m_replyToData;
    int m_DownloadCount_Server;
    int m_DownloadCount_PackDescription;
};

} // namespace Internal
} // namespace DataPack

#endif // DATAPACK_INTERNAL_HTTPSERVERENGINE_H
