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
//#include "servermanager_p.h"
#include "servermanager.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <quazip/global.h>

#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkRequest>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QSignalMapper>
#include <QUuid>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {

const char * const TAG_ROOT                 = "ServerManagerConfig";
const char * const TAG_SERVER               = "Server";
const char * const TAG_PACK                 = "Pack";

// Server attribs
const char * const ATTRIB_URL                 = "url";
const char * const ATTRIB_LASTCHECK           = "lastChk";
const char * const ATTRIB_RECORDEDVERSION     = "recVer";
const char * const ATTRIB_USERUPDATEFREQUENCY = "uUpFq";

// Pack specific attribs
const char * const ATTRIB_INSTALLED         = "inst";
const char * const ATTRIB_INSTALLPATH       = "instPath";

const char * const SERVER_CONFIG_FILENAME   = "server.conf.xml";

}  // End namespace Anonymous


ServerManager::ServerManager(QObject *parent) :
    IServerManager(parent),
    m_NetworkAccessManager(new QNetworkAccessManager(this))
{
    setObjectName("ServerManager");
//    if (!QDir(filesCachePath).exists()) {
//        LOG_ERROR(tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(filesCachePath));
//    }
//    m_d->filesCachePath = QDir::cleanPath(filesCachePath);
}

ServerManager::~ServerManager()
{
    delete m_NetworkAccessManager;
    // Servers are delete by the QObject inheritance
}

bool ServerManager::setGlobalConfiguration(const QString &xmlContent, QString *errorMsg)
{
    QDomDocument doc;
    QString msg;
    int col, line;
    if (!doc.setContent(xmlContent, &msg, &line, &col)) {
        if (errorMsg) {
            errorMsg->append(QString("XML Error (l:%1;c:%2): %3.").arg(line).arg(col).arg(msg));
        }
        return false;
    }

    QDomElement root = doc.firstChildElement(::TAG_ROOT);
    // Read servers
    QDomElement server = root.firstChildElement(::TAG_SERVER);
    while (!server.isNull()) {
        Server s;
        s.fromSerializedString(server.attribute(::ATTRIB_URL));
        s.setLastChecked(QDateTime::fromString(server.attribute(::ATTRIB_LASTCHECK), Qt::ISODate));
        s.setLocalVersion(server.attribute(::ATTRIB_RECORDEDVERSION));
        s.setUserUpdateFrequency(server.attribute(::ATTRIB_USERUPDATEFREQUENCY).toInt());
        m_Servers.append(s);
        server = server.nextSiblingElement(::TAG_SERVER);
    }
    // Read packs
//    QDomElement pack = root.firstChildElement(::TAG_PACK);
//    while (!server.isNull()) {
//        Pack p;
//        /** \todo here */
//        pack = pack.nextSiblingElement(::TAG_PACK);
//    }
    return true;
}

QString ServerManager::xmlConfiguration() const
{
    QDomDocument doc;
    QDomElement root = doc.createElement(::TAG_ROOT);
    doc.appendChild(root);
    for(int i = 0; i < m_Servers.count(); ++i) {
        const Server &s = m_Servers.at(i);
        QDomElement e = doc.createElement(::TAG_SERVER);
        root.appendChild(e);
        e.setAttribute(::ATTRIB_URL, s.serialize());
        e.setAttribute(::ATTRIB_RECORDEDVERSION, s.localVersion());
        e.setAttribute(::ATTRIB_LASTCHECK, s.lastChecked().toString(Qt::ISODate));
        e.setAttribute(::ATTRIB_USERUPDATEFREQUENCY, s.userUpdateFrequency());
    }
    return doc.toString(2);
}

void ServerManager::connectServer(const Server &server, const ServerIdentification &ident)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(ident);
}

ServerDescription ServerManager::downloadServerDescription(const Server &server)
{
    // TODO
    Q_UNUSED(server);
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

bool ServerManager::installDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    Q_UNUSED(progressBar);
    // Algo
    // 1. check if pack is already installed
    // 1.1 ? yes -> call upgradePackage and return
    // Connect progressBar
    // Download the package file
    // Check MD5 of the file
    // Unzip file
    // Copy files to the install path
    // Emit pack installed
    return false;
}

bool ServerManager::removeDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    Q_UNUSED(progressBar);
    return false;
}

bool ServerManager::updateDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    Q_UNUSED(progressBar);
    return false;
}

void ServerManager::setCachePath(const QString &absPath)
{
    filesCachePath = absPath;
}

QString ServerManager::cachePath() const
{
    return filesCachePath;
}

void ServerManager::setInstallPath(const QString &absPath)
{
    m_installPath=QDir::cleanPath(absPath);
}

QString ServerManager::installPath() const
{
    return m_installPath;
}

bool ServerManager::addServer(const QString &url)
{
    Server server(url);
    return addServer(server);
}

bool ServerManager::addServer(const Server &server)
{
    // check if a server already exists with the same URL
    foreach (Server child, m_Servers) {
        if (child == server)
            return false;
    }
    m_Servers.append(server);
    Q_EMIT serverAdded(m_Servers.count() - 1);
    return true;
}

int ServerManager::serverCount() const
{
    return m_Servers.count();
}

Server ServerManager::getServerAt(int index) const
{
    if (index < m_Servers.count() && index >= 0)
        return m_Servers.at(index);
    return Server();
}

int ServerManager::getServerIndex(const QString &url) const
{
    for (int i = 0; i < m_Servers.count(); i++)
        if (m_Servers.at(i).url() == url)
            return i;
    return -1;
}

void ServerManager::removeServerAt(int index)
{
    if (index >= 0 && index < m_Servers.count()) {
        Server removed = m_Servers.at(index);
        Q_EMIT serverAboutToBeRemoved(removed);
        Q_EMIT serverAboutToBeRemoved(index);
        m_Servers.remove(index);
        Q_EMIT serverRemoved(removed);
        Q_EMIT serverRemoved(index);
    }
}

void ServerManager::connectAndUpdate(int index)
{
    Q_UNUSED(index);
//    if (index < m_Servers.count() && index >= 0)
//        m_Servers.at(index).connectAndUpdate();
}

void ServerManager::checkServerUpdates()
{
    WARN_FUNC << m_Servers.count();
    for(int i=0; i < m_Servers.count(); ++i) {
        Server &s = m_Servers[i];
        qDebug("%d: %s", i, qPrintable(s.url()));
        if (s.isLocalServer()) {
            // check directly
            s.fromXml(Utils::readTextFile(s.url(Server::ServerConfigurationFile), Utils::DontWarnUser));
            // move a copy of the description in the working path of server manager
        } else {
            // FTP | HTTP
            // Download server.conf.xml
            QNetworkRequest request;
            request.setUrl(s.url(Server::ServerConfigurationFile));
            request.setRawHeader("User-Agent", QString("FreeMedForms:%1;%2")
                                 .arg(qApp->applicationName())
                                 .arg(qApp->applicationVersion()).toAscii());

            QNetworkReply *reply = m_NetworkAccessManager->get(request);
            m_replyToServer.insert(reply, &s);
            m_replyToBuffer.insert(reply, QByteArray());
            connect(reply, SIGNAL(readyRead()), this, SLOT(serverReadyRead()));
            connect(reply, SIGNAL(finished()), this, SLOT(serverFinished()));
        }
    }
    // TODO THIS LINE IS ONLY FOR TESTING PURPOSE
    checkServerUpdatesAfterDownload();
}

QList<PackDescription> ServerManager::getPackDescription(const Server &server)
{
    WARN_FUNC;
    // If Pack list already known return it
    QList<PackDescription> toReturn;
    const QStringList keys = m_Packs.uniqueKeys();
    if (keys.contains(server.url(), Qt::CaseInsensitive)) {
        QList<Pack> packs = m_Packs.values(server.url());
        for(int i = 0; i < packs.count(); ++i) {
            toReturn << packs.at(i).description();
        }
        return toReturn;
    }

    createServerPackList(server);

    QList<Pack> packs = m_Packs.values(server.url());
    for(int i = 0; i < packs.count(); ++i) {
        toReturn << packs.at(i).description();
    }
    return toReturn;
}

QList<Pack> ServerManager::getPackForServer(const Server &server)
{
    createServerPackList(server);
    return m_Packs.values(server.url());
}

void ServerManager::createServerPackList(const Server &server)
{
    if (!m_Packs.values(server.url()).isEmpty()) {
        return;
    }
    // Get the server config
    foreach(const QString &file, server.content().packDescriptionFileNames()) {
        QString path = server.url();
        path = path.replace("file:/", "") + QDir::separator() + file;
        QFileInfo f(path); // relative path
        Pack pack;
        // Read the packDescription
        pack.fromXmlFile(f.absoluteFilePath());
        // Store in the cache
        m_Packs.insertMulti(server.url(), pack);
    }
}

void ServerManager::checkServerUpdatesAfterDownload()
{
    for(int i=0; i < m_Servers.count(); ++i) {
        Server &s = m_Servers[i];
        if (s.updateState() == Server::UpdateAvailable) {
            qWarning() << "UPDATE" << s.url() << s.localVersion() << s.description().data(ServerDescription::Version).toString();
        }
        s.setLastChecked(QDateTime::currentDateTime());
//        s.setLocalVersion();
    }
    Q_EMIT serverUpdateChecked();
}

void ServerManager::serverReadyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QByteArray &ba = m_replyToBuffer[reply];
    ba.append(reply->readAll());
}

void ServerManager::serverError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);

    // TODO manage errors
}

void ServerManager::serverFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
        return;
    Server *server = m_replyToServer[reply];

    Q_ASSERT_X(server, "ServerManager::serverFinished()", "there is not Server associated with the QNetworkReply pointer!");

    bool downloadPackDescriptionNeeded = false;
    switch (server->urlStyle()) {
    case Server::NoStyle:
    {
        // Read the XML from the buffer
        server->fromXml(m_replyToBuffer[reply]);
        break;
    }
    case Server::Ftp:
    case Server::Http:
    case Server::HttpPseudoSecuredNotZipped:
    {
        // Read the XML from the buffer
        server->fromXml(m_replyToBuffer[reply]);
        downloadPackDescriptionNeeded = true;
        break;
    }
    case Server::FtpZipped:
    case Server::HttpPseudoSecuredAndZipped:
    {
        // save buffer to tmp zip file
        QString zipName = m_installPath + QDir::separator() + "datapacktmp" + QDir::separator() + QUuid::createUuid().toString().remove("-").remove("{").remove("}") + QDir::separator() + "serverconf.zip";
        QDir().mkpath(QFileInfo(zipName).absolutePath());
        QFile zip(zipName);
        if (!zip.open(QFile::WriteOnly | QFile::Text)) {
            LOG_ERROR(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(zip.fileName()));
            reply->deleteLater();
            return;
        }
        zip.write(m_replyToBuffer[reply]);
        zip.close();

        // unzip file
        if (!QuaZipTools::unzipFile(zipName)) {
            LOG_ERROR("Unable to unzip file: " + zipName);
            reply->deleteLater();
            return;
        }

        // read server configuration file
        QString serverConfFile = QFileInfo(zipName).absolutePath() + QDir::separator() + Server::serverConfigurationFileName();
        server->fromXml(Utils::readTextFile(serverConfFile, Utils::DontWarnUser));

        // test downloaded zip files for all pack description
        foreach(const QString &file, server->content().packDescriptionFileNames()) {
            QFileInfo info(file);
            if (info.isRelative()) { // This must be always the case...
                info.setFile(QFileInfo(zipName).absolutePath() + QDir::separator() + info.fileName());
            }
            if (!info.exists()) {
                downloadPackDescriptionNeeded = true;
            }
        }
        break;
    }
    }

    // Download all linked packagedescription -> see ServerContent --> server.content().packDescriptionFileNames()
    if (downloadPackDescriptionNeeded) {
        foreach(const QString &file, server->content().packDescriptionFileNames()) {
            // TODO DOWNLOAD ME: file
            // We should move the request contruction in the Server class to be sure to always have the same headers !!
            // somthing like
            // server->constructRequest(...);
//            QNetworkRequest request;
//            request.setUrl(s.url(Server::PackDescriptionFile, file));
//            request.setRawHeader("User-Agent", QString("FreeMedForms:%1;%2")
//                                 .arg(qApp->applicationName())
//                                 .arg(qApp->applicationVersion()).toAscii());

//            QNetworkReply *reply = m_NetworkAccessManager->get(request);
//            m_replyToServer.insert(reply, &s);
//            m_replyToBuffer.insert(reply, QByteArray());
//            connect(reply, SIGNAL(readyRead()), this, SLOT(serverReadyRead()));
//            connect(reply, SIGNAL(finished()), this, SLOT(serverFinished()));
        }
    }
    reply->deleteLater();

    // When all descriptions are downloaded call -> checkServerUpdatesAfterDownload()
    checkServerUpdatesAfterDownload();
}
