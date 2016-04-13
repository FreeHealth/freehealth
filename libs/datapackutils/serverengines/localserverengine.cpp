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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "localserverengine.h"
#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProgressBar>
#include <QTimer>

#include <QDebug>

using namespace DataPack;
using namespace Internal;

static inline DataPack::DataPackCore &core() {return DataPack::DataPackCore::instance();}
static inline DataPack::Internal::ServerManager *serverManager() {return qobject_cast<ServerManager*>(core().serverManager());}

static QString statusKey(const Pack &pack) {
    return pack.uuid()+pack.version();
}

static QString statusKey(const Server &server) {
    return server.uuid()+server.version();
}

LocalServerEngine::LocalServerEngine(QObject *parent) :
    IServerEngine(parent)
{
    setObjectName("LocalServerEngine");
}

bool LocalServerEngine::managesServer(const Server &server)
{
    return server.nativeUrl().startsWith("file://");
}

void LocalServerEngine::addToDownloadQueue(const ServerEngineQuery &query)
{
    m_queue.append(query);
}

int LocalServerEngine::downloadQueueCount() const
{
    return m_queue.count();
}

bool LocalServerEngine::startDownloadQueue()
{
    qWarning() << "LocalServerEngine::startDownloadQueue()";
    for(int i = 0; i < m_queue.count(); ++i) {
        const ServerEngineQuery &query = m_queue.at(i);
        Server *server = query.server;
        if (query.downloadDescriptionFiles) {
            // Read the local server config
            QString fileName = server->url(Server::ServerConfigurationFile);
            if (!QFileInfo(fileName).exists()) {
                // Create the status of the server
                ServerEngineStatus status;
                status.downloadCorrectlyFinished = false;
                status.engineMessages << tr("Server configuration can be downloaded. Path: %1").arg(fileName);
                status.hasError = true;
                status.isSuccessful = false;
                status.proxyIdentificationError = false;
                status.serverIdentificationError = false;
                m_ServerStatus.insert(statusKey(*server), status);
                continue;
            }
            server->fromXml(Utils::readTextFile(fileName, Utils::DontWarnUser));
            // Read the local pack config
            for(int j = 0; j < server->content().packDescriptionFileNames().count(); ++j) {
                const QString &fileName = server->url(Server::PackDescriptionFile, server->content().packDescriptionFileNames().at(j));
                QFileInfo packXml(fileName);
                if (!packXml.exists()) {
                    LOG_ERROR(QString("Missing pack description file: %1 in local server.").arg(fileName));
                    continue;
                }
                Pack p;
                p.fromXmlFile(fileName);
                serverManager()->registerPack(*server, p);
            }

            // Create the status of the server
            ServerEngineStatus status;
            status.downloadCorrectlyFinished = true;
            status.engineMessages << tr("Server configuration successfully downloaded.");
            status.hasError = false;
            status.isSuccessful = true;
            status.proxyIdentificationError = false;
            status.serverIdentificationError = false;
            m_ServerStatus.insert(statusKey(*server), status);
        }
        if (query.downloadPackFile) {
            const Pack *pack = query.pack;
            QString url = server->url(Server::PackFile, pack->serverFileName());
            QFileInfo local(url);
            if (local.exists()) {
                // copy pack to datapack core persistentCachePath
                QString newPath = QFileInfo(pack->persistentlyCachedZipFileName()).absolutePath();
                QString error;
                Utils::removeDirRecursively(newPath, &error);
                QDir().mkpath(newPath);

                // copy pack File and XML config
                QFile::copy(local.absoluteFilePath(), pack->persistentlyCachedZipFileName());
                QFile::copy(pack->originalXmlConfigFileName(), pack->persistentlyCachedXmlConfigFileName());

                // Create the status of the server
                ServerEngineStatus status;
                status.downloadCorrectlyFinished = true;
                status.engineMessages << tr("Pack successfully downloaded.");
                status.hasError = false;
                status.isSuccessful = true;
                status.proxyIdentificationError = false;
                status.serverIdentificationError = false;
                m_PackStatus.insert(statusKey(*pack), status);
                Q_EMIT packDownloaded(*pack, status);
            } else {
                LOG_ERROR("Pack file does not exist.");
                // Create the status of the server
                ServerEngineStatus status;
                status.downloadCorrectlyFinished = false;
                status.engineMessages << tr("Pack file does not exist.");
                status.hasError = true;
                status.isSuccessful = false;
                status.proxyIdentificationError = false;
                status.serverIdentificationError = false;
                m_PackStatus.insert(statusKey(*pack), status);
                Q_EMIT packDownloaded(*pack, status);
            }
        }

        if (query.progressBar) {
            query.progressBar->setRange(0,1);
            query.progressBar->setValue(1);
        }
    }
    m_queue.clear();
    QTimer::singleShot(100, this, SLOT(emitQueueDowloaded()));
    return true;
}

int LocalServerEngine::runningJobs() const
{
    // Not asynchrone
    return 0;
}

bool LocalServerEngine::stopJobsAndClearQueue()
{
    return true;
}

const ServerEngineStatus &LocalServerEngine::lastStatus(const Pack &pack)
{
    const QString &key = statusKey(pack);
    return m_PackStatus[key];
}

const ServerEngineStatus &LocalServerEngine::lastStatus(const Server &server)
{
    const QString &key = statusKey(server);
    return m_ServerStatus[key];
}

void LocalServerEngine::emitQueueDowloaded()
{
    Q_EMIT queueDowloaded();
}
