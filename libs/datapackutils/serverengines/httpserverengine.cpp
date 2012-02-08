/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/basiclogindialog.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>
#include <quazip/global.h>

#include <QDir>
#include <QFile>
#include <QProgressBar>
#include <QNetworkProxyQuery>
#include <QAuthenticator>

#include <QDebug>

// Zip Command Line: zip serverconf.zip server.conf.xml icd10/icd10.xml drugs/drugs.xml

using namespace DataPack;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline DataPack::Internal::ServerManager *serverManager() {return qobject_cast<ServerManager*>(core().serverManager());}

namespace {
    const int MAX_AUTHENTIFICATION_TRIES = 3;
    const char * const  ICONEYES = "eyes.png";

    static QString statusKey(const Pack &pack) {
        return pack.uuid()+pack.version();
    }

    static QString statusKey(const Server &server) {
        return server.uuid()+server.version();
    }

}

ReplyData::ReplyData(QNetworkReply *reply, Server *server, Server::FileRequested fileType, const Pack &pack, QProgressBar *progBar) {
    this->reply = reply;
    this->server = server;
    this->fileType = fileType;
    this->pack = pack;
    this->bar = progBar;
}

ReplyData::ReplyData(QNetworkReply *reply, Server *server, Server::FileRequested fileType, QProgressBar *progBar)
{
    this->reply = reply;
    this->server = server;
    this->fileType = fileType;
    this->bar = progBar;
}


/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  IServerEngine code  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
HttpServerEngine::HttpServerEngine(QObject *parent)  :
    IServerEngine(parent),
    m_DownloadCount_Server(0),
    m_DownloadCount_PackDescription(0)
{
    setObjectName("HttpServerEngine");
    m_NetworkAccessManager = new QNetworkAccessManager(this);
    const QString &id = Utils::testInternetConnexion();
    if (!id.isEmpty()) {
        LOG("Internet connection is enabled.");
        bool proxy = false;
        if (!core().networkProxy().hostName().isEmpty()) {
            LOG("Using proxy " + core().networkProxy().hostName());
            m_NetworkAccessManager->setProxy(core().networkProxy());
            proxy = true;
        } else {
            // Auto-check for system proxys
            QNetworkProxyQuery npq(QUrl("http://www.google.com"));
            QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
            foreach(const QNetworkProxy &p, listOfProxies) {
                if (p.type()==QNetworkProxy::HttpProxy && !p.hostName().isEmpty()) {
                    LOG("Using proxy " + p.hostName());
                    m_NetworkAccessManager->setProxy(p);
                    proxy = true;
                    break;
                }
            }
        }
        if (!proxy)
            LOG("No proxy in use.");
    } else {
        LOG_ERROR("No internet connection available.");
    }
}

HttpServerEngine::~HttpServerEngine()
{}

bool HttpServerEngine::managesServer(const Server &server)
{
    if (core().isInternetConnexionAvailable())
        return server.nativeUrl().startsWith("http");
    return false;
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
    // Internet connection available ?
    if (!core().isInternetConnexionAvailable()) {
        LOG_ERROR("No internet connexion available.");
        return false;
    }
    // Use a proxy ?
    if (!core().networkProxy().hostName().isEmpty()) {
        m_NetworkAccessManager->setProxy(core().networkProxy());
        LOG("Using proxy: " + m_NetworkAccessManager->proxy().hostName());
    }
    for(int i = 0; i < m_queue.count(); ++i) {
        const ServerEngineQuery &query = m_queue.at(i);
        Server *s = query.server;
        if (!managesServer(*s))
            continue;
        qWarning() << "HTTP:startDownloadQueue; server #" << i << s->nativeUrl();

        QNetworkReply *reply = 0;

        if (query.downloadDescriptionFiles) {
            // Create a network request for the server config
            QNetworkRequest request = createRequest(s->url(Server::ServerConfigurationFile));
            reply = m_NetworkAccessManager->get(request);
            m_replyToData.insert(reply, ReplyData(reply, s, Server::ServerConfigurationFile, query.progressBar));
            ++m_DownloadCount_Server;

            // Create a status for the server
            ServerEngineStatus status;
            m_ServerStatus.insert(statusKey(*s), status);
        } else if (query.downloadPackFile) {
            // Create a network request for the pack file
            QNetworkRequest request = createRequest(s->url(Server::PackFile, query.pack->serverFileName()));
            reply = m_NetworkAccessManager->get(request);
            m_replyToData.insert(reply, ReplyData(reply, s, Server::PackFile, *query.pack, query.progressBar));

            // Create a status for the pack
            ServerEngineStatus status;
            m_PackStatus.insert(statusKey(*s), status);
        }
        connect(reply, SIGNAL(readyRead()), this, SLOT(serverReadyRead()));
        connect(reply, SIGNAL(finished()), this, SLOT(serverFinished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(serverError(QNetworkReply::NetworkError)));
        connect(m_NetworkAccessManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        connect(m_NetworkAccessManager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

        QProgressBar *bar = query.progressBar;
        if (bar) {
            bar->setRange(0, 100);
            bar->setValue(0);
            connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  Specific Http code  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void HttpServerEngine::downloadProgress(qint64 bytesRead, qint64 totalBytes)
{
    // Retreive progressBar
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QProgressBar *bar = m_replyToData[reply].bar;
    if (!bar) {
        disconnect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
        return;
    }
    if (totalBytes>0) {
        int v = bytesRead*100/totalBytes;
        bar->setValue(v);
    } else
        bar->setValue(0);
}

void HttpServerEngine::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    LOG("Server authentification requiered: " +  reply->url().toString());
    Utils::BasicLoginDialog dlg;
    dlg.setModal(true);
    dlg.setTitle(tr("Server authentification requiered"));
    dlg.setToggleViewIcon(core().icon(ICONEYES));
    if (dlg.exec()==QDialog::Accepted) {
        authenticator->setUser(dlg.login());
        authenticator->setPassword(dlg.password());
    }
    /** \todo manage ServerEngineStatus here */
}

void HttpServerEngine::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    LOG("Proxy authentification requiered: " +  proxy.hostName());
    if (!proxy.user().isEmpty() && !proxy.password().isEmpty()) {
        authenticator->setUser(proxy.user());
        authenticator->setPassword(proxy.password());
    } else {
        // Ask user for identification
        Utils::BasicLoginDialog dlg;
        dlg.setModal(true);
        dlg.setTitle(tr("Proxy authentification requiered"));
        dlg.setToggleViewIcon(core().icon(ICONEYES));
        if (dlg.exec()==QDialog::Accepted) {
            authenticator->setUser(dlg.login());
            authenticator->setPassword(dlg.password());
        }
    }
    /** \todo manage ServerEngineStatus here */
}

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
    WARN_FUNC << error;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    ReplyData &data = m_replyToData[reply];
    reply->deleteLater(); // we don't need reply anymore
    ServerEngineStatus *status = getStatus(data);
    Q_ASSERT(status);
    status->hasError = true;
    status->isSuccessful = false;
    status->errorMessages << tr("Server error: %1").arg(reply->errorString());
    LOG_ERROR(tr("Server error: %1").arg(reply->errorString()));
    Q_EMIT packDownloaded(data.pack, *status);
    --m_DownloadCount_Server;
}

/** Server or Pack description fully read. */
void HttpServerEngine::serverFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    qWarning() << "HTTP : serverFinished" << reply->request().url() << reply->error();

    if (reply->error() != QNetworkReply::NoError)
        return;


    ReplyData &data = m_replyToData[reply];
    data.server->setConnected(true);
    reply->deleteLater(); // we don't need reply anymore
    ServerEngineStatus *status = getStatus(data);
    Q_ASSERT(status);
    status->downloadCorrectlyFinished = true;
    status->serverIdentificationError = false;
    status->proxyIdentificationError = false;

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

ServerEngineStatus *HttpServerEngine::getStatus(const ReplyData &data)
{
    if (data.server) {
        return &m_ServerStatus[statusKey(*data.server)];
    }
    return &m_PackStatus[statusKey(data.pack)];
}

/** Reads Server description XML file and start the dowloading of pack description if needed. */
void HttpServerEngine::afterServerConfigurationDownload(const ReplyData &data)
{
    bool downloadPackDescriptionNeeded = false;
    Server *server = data.server;
    ServerEngineStatus *status = getStatus(data);
    Q_ASSERT(status);

    switch (server->urlStyle()) {
    case Server::Http:
    case Server::HttpPseudoSecuredNotZipped:
    {
        // Save to persistentCache path
        Utils::saveStringToFile(data.response, core().persistentCachePath() + QDir::separator() + server->uuid() + QDir::separator() + "server.conf.xml");
        // Read the XML from the buffer
        server->fromXml(data.response);
        downloadPackDescriptionNeeded = true;
        status->engineMessages << tr("Server description file correctly downloaded.");
        break;
    }
    case Server::HttpPseudoSecuredAndZipped:
    {
        // save buffer to tmp zip file
        QString zipName = core().persistentCachePath() + QDir::separator() + server->uuid() + QDir::separator() + "serverconf.zip";
        QDir().mkpath(QFileInfo(zipName).absolutePath());
        QFile zip(zipName);
        if (!zip.open(QFile::WriteOnly | QFile::Text)) {
            LOG_ERROR(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(zip.fileName()));
            status->errorMessages << tr("Server description file is not readable.");
            status->hasError = true;
            status->isSuccessful = false;
            return;
        }
        zip.write(data.response);
        zip.close();

        // unzip file
        if (!QuaZipTools::unzipFile(zipName)) {
            LOG_ERROR("Unable to unzip file: " + zipName);
            status->errorMessages << tr("Server description file can not be unzipped.");
            status->hasError = true;
            status->isSuccessful = false;
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
        status->engineMessages << tr("Adding pack description file to the download queue.");
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
    ServerEngineStatus *status = getStatus(data);
    Q_ASSERT(status);
    status->engineMessages.append(tr("Pack description correctly downloaded."));
}

void HttpServerEngine::afterPackFileDownload(const ReplyData &data)
{
    ServerEngineStatus *status = getStatus(data);
    Q_ASSERT(status);
    status->engineMessages.append(tr("Pack correctly downloaded."));

    // Save downladed content to persistently pack cache
    const Pack &pack = data.pack;
    QFileInfo toPersistentCache(pack.persistentlyCachedZipFileName());
    if (toPersistentCache.exists()) {
        // remove everything
        QFile::remove(pack.persistentlyCachedZipFileName());
        QFile::remove(pack.persistentlyCachedXmlConfigFileName());
    }

    // Copy pack to datapack core persistentCachePath
    QString newPath = toPersistentCache.absolutePath();
    QDir newDir(newPath);
    if (!newDir.exists()) {
        QDir().mkpath(newPath);
    }

    QFile out(toPersistentCache.absoluteFilePath());
    if (!out.open(QFile::WriteOnly)) {
        LOG_ERROR(tkTr(Trans::Constants::FILE_1_CAN_NOT_BE_CREATED).arg(toPersistentCache.absoluteFilePath()));
        status->engineMessages.append(tr("Pack file can not be created in the persistent cache."));
        status->hasError = true;
        status->isSuccessful = false;
        /** \todo a better management of error is requiered */
        return;
    }
    LOG("Writing pack content to " + toPersistentCache.absoluteFilePath());
    out.write(data.response);
    out.close();

    // copy pack XML config
    QFile::copy(pack.originalXmlConfigFileName(), pack.persistentlyCachedXmlConfigFileName());

    Q_EMIT packDownloaded(pack, *status);
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

const ServerEngineStatus &HttpServerEngine::lastStatus(const Pack &pack)
{
    const QString &key = statusKey(pack);
    return m_PackStatus[key];
}

const ServerEngineStatus &HttpServerEngine::lastStatus(const Server &server)
{
    const QString &key = statusKey(server);
    return m_ServerStatus[key];
}
