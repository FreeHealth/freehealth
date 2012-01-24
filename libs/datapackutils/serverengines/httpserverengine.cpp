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
#include "httpserverengine.h"
#include <datapackutils/core.h>
#include <datapackutils/servermanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>
#include <quazip/global.h>

#include <QDir>
#include <QFile>

#include <QDebug>

using namespace DataPack;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DataPack::Core &core() { return DataPack::Core::instance(); }

ReplyData::ReplyData(QNetworkReply *reply, Server *server, Server::FileRequested fileType) {
    this->reply = reply;
    this->server = server;
    this->fileType = fileType;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  IServerEngine code  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
HttpServerEngine::HttpServerEngine(IServerManager *parent)  :
    IServerEngine(parent),
    m_DownloadCount_Server(0),
    m_DownloadCount_PackDescription(0)
{
    setObjectName("HttpServerEngine");
    m_NetworkAccessManager = new QNetworkAccessManager(this);
}

HttpServerEngine::~HttpServerEngine()
{}

ServerManager *HttpServerEngine::serverManager()
{
    return qobject_cast<ServerManager*>(parent());
}

bool HttpServerEngine::managesServer(const Server &server)
{
    return server.nativeUrl().startsWith("http");
}

void HttpServerEngine::addToDownloadQueue(const ServerEngineQuery &query)
{
    m_queue.append(query);
}

int HttpServerEngine::downloadQueueCount() const
{
    return m_queue.count();
}

bool HttpServerEngine::startDownloadQueue()
{
    for(int i=0; i < m_queue.count(); ++i) {
        Server *s = m_queue.at(i).server;
        if (!managesServer(*s))
            continue;
        qWarning() << "HTTP:startDownloadQueue; server #" << i << s->nativeUrl();

        // Download server.conf.xml
        QNetworkRequest request = createRequest(s->url(Server::ServerConfigurationFile));
        QNetworkReply *reply = m_NetworkAccessManager->get(request);
        m_replyToData.insert(reply, ReplyData(reply, s, Server::ServerConfigurationFile));
        ++m_DownloadCount_Server;
        connect(reply, SIGNAL(readyRead()), this, SLOT(serverReadyRead()));
        connect(reply, SIGNAL(finished()), this, SLOT(serverFinished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(serverError(QNetworkReply::NetworkError)));
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  Specific Http code  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/** Server configuration file read enable. */
void HttpServerEngine::serverReadyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    ReplyData &data = m_replyToData[reply];
    data.response.append(reply->readAll());
}

/** An error occured during the network access. */
void HttpServerEngine::serverError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    /** \todo code here */
    --m_DownloadCount_Server;
}

/** Server or Pack description fully read. */
void HttpServerEngine::serverFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
        return;

    qWarning() << "HTTP : serverFinished" << reply->request().url();

    ReplyData &data = m_replyToData[reply];
    data.server->setConnected(true);
    reply->deleteLater(); // we don't need reply anymore

    switch (data.fileType) {
    case Server::ServerConfigurationFile:
    {
        afterServerConfigurationDownload(data);
        --m_DownloadCount_Server;
        break;
    }
    case Server::PackDescriptionFile:
    {
        afterPackDescriptionFileDownload(data);
        --m_DownloadCount_PackDescription;
        break;
    }
    case Server::PackFile:
    {
        afterPackFileDownload(data);
        break;
    }
    default:;
    }

    // we can remove the associated data
    m_replyToData.remove(reply);

    // if queue is empty -> emit signal
    if (m_DownloadCount_PackDescription==0 && m_DownloadCount_Server==0) {
        LOG("Queue downloaded");
        m_queue.clear();
        Q_EMIT queueDowloaded();
    }
}

/** Reads Server description XML file and start the dowloading of pack description if needed. */
void HttpServerEngine::afterServerConfigurationDownload(const ReplyData &data)
{
    bool downloadPackDescriptionNeeded = false;
    Server *server = data.server;

    switch (server->urlStyle()) {
    case Server::Http:
    case Server::HttpPseudoSecuredNotZipped:
    {
        // Save to persistentCache path
        Utils::saveStringToFile(data.response, core().persistentCachePath() + QDir::separator() + server->uuid() + QDir::separator() + "server.conf.xml");
        // Read the XML from the buffer
        server->fromXml(data.response);
        downloadPackDescriptionNeeded = true;
        break;
    }
    case Server::HttpPseudoSecuredAndZipped:
    {
        // save buffer to tmp zip file
        QString zipName = core().persistentCachePath() + QDir::separator() + "datapacktmp" + QDir::separator() + server->uuid() + QDir::separator() + "serverconf.zip";
        QDir().mkpath(QFileInfo(zipName).absolutePath());
        QFile zip(zipName);
        if (!zip.open(QFile::WriteOnly | QFile::Text)) {
            LOG_ERROR(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(zip.fileName()));
            return;
        }
        zip.write(data.response);
        zip.close();

        // unzip file
        if (!QuaZipTools::unzipFile(zipName)) {
            LOG_ERROR("Unable to unzip file: " + zipName);
            return;
        }

        // read server configuration file
        QString serverConfFile = QFileInfo(zipName).absolutePath() + QDir::separator() + Server::serverConfigurationFileName();
        server->fromXml(Utils::readTextFile(serverConfFile, Utils::DontWarnUser));

        // test downloaded zip files for all pack description
        foreach(const QString &file, server->content().packDescriptionFileNames()) {
            QFileInfo info(file);
            if (info.isRelative()) { // This must be always the case...
                info.setFile(QFileInfo(zipName).absolutePath() + QDir::separator() + file);
            }
            if (!info.exists()) {
                downloadPackDescriptionNeeded = true;
                continue;
            }
            // create the pack and record it to server manager
            createPackAndRegisterToServerManager(*server, info.absoluteFilePath());
        }
        break;
    }
    default: LOG_ERROR("Url type not managed.");
    }

    // Download all linked packagedescription -> see ServerContent --> server.content().packDescriptionFileNames()
    if (downloadPackDescriptionNeeded) {
        foreach(const QString &file, server->content().packDescriptionFileNames()) {
            QNetworkRequest request = createRequest(server->url(Server::PackDescriptionFile, file));
            QNetworkReply *reply = m_NetworkAccessManager->get(request);
            m_replyToData.insert(reply, ReplyData(reply, server, Server::PackDescriptionFile));
            ++m_DownloadCount_PackDescription;
            connect(reply, SIGNAL(readyRead()), this, SLOT(serverReadyRead()));
            connect(reply, SIGNAL(finished()), this, SLOT(serverFinished()));
            // TODO manage errors
        }
    }
}
void HttpServerEngine::afterPackDescriptionFileDownload(const ReplyData &data)
{
    PackDescription desc;
    desc.fromXmlContent(data.response);
    /** \todo add description to server manager */
//    m_PackDescriptions.insert(data.reply->request().url().toString(), desc);
}

void HttpServerEngine::afterPackFileDownload(const ReplyData &data)
{
    // TODO
    Q_UNUSED(data);
}

void HttpServerEngine::createPackAndRegisterToServerManager(const Server &server, const QString &pathToPackDescription)
{
    Pack p;
    p.fromXmlFile(pathToPackDescription);
    serverManager()->registerPack(server, p);
}

QNetworkRequest HttpServerEngine::createRequest(const QString &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", QString("FreeMedForms:%1;%2")
                         .arg(qApp->applicationName())
                         .arg(qApp->applicationVersion()).toAscii());
    return request;
}
