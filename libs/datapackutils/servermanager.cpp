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

#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkRequest>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {

const char * const TAG_ROOT                 = "ServerManagerConfig";
const char * const TAG_SERVER               = "Server";
const char * const TAG_PACK                 = "Pack";

// Server attribs
const char * const ATTRIB_URL               = "url";
const char * const ATTRIB_LASTCHECK         = "lastChk";
const char * const ATTRIB_RECORDEDVERSION   = "recVer";

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
        s.setUrl(server.attribute(::ATTRIB_URL));
        s.setLastChecked(QDateTime::fromString(server.attribute(::ATTRIB_LASTCHECK), Qt::ISODate));
        s.setLocalVersion(server.attribute(::ATTRIB_RECORDEDVERSION));
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
        e.setAttribute(::ATTRIB_URL, s.url());
        e.setAttribute(::ATTRIB_RECORDEDVERSION, s.localVersion());
        e.setAttribute(::ATTRIB_LASTCHECK, s.lastChecked().toString(Qt::ISODate));
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

QString ServerManager::cachePath() const
{
    return filesCachePath;
}

bool ServerManager::addServer(const QString &url)
{
    // check if a server already exists with the same URL
    foreach (Server child, m_Servers)
        if (child.url() == url)
            return false;

    Server server(url);
    m_Servers.append(server);
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
    m_Servers.remove(index);
}

void ServerManager::connectAndUpdate(int index)
{
//    if (index < m_Servers.count() && index >= 0)
//        m_Servers.at(index).connectAndUpdate();
}

void ServerManager::checkServerUpdates()
{
    WARN_FUNC;
    for(int i=0; i < m_Servers.count(); ++i) {
        Server &s = m_Servers[i];
        if (s.isLocalServer()) {
            // check directly
            QString t = s.url();
            t = QDir::cleanPath(t.replace("file:/", "")) + "/";
            t += ::SERVER_CONFIG_FILENAME;
            s.fromXml(Utils::readTextFile(t, Utils::DontWarnUser));
            // move a copy of the description in the working path of server manager
        } else {
            // FTP | HTTP
            // Download server.conf.xml

            // When done
            // add server XML description to the server
            // s.fromXml(xml);
            // Download all linked packagedescription -> see ServerContent --> server.content().packDescriptionFileNames()
            // When all descriptions are downloaded call -> checkServerUpdatesAfterDownload()
        }
    }
    // TODO THIS LINE IS ONLY FOR TESTING PURPOSE
    checkServerUpdatesAfterDownload();
}

QList<PackDescription> ServerManager::getPackDescription(const Server &server)
{
    WARN_FUNC;
    // If Pack list already known return it
//    const QStringList keys = m_PackDescriptions.uniqueKeys();
//    if (keys.contains(server.url(), Qt::CaseInsensitive)) {
//        return m_PackDescriptions.values(server.url());
//    }

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
        qWarning() << "    * PackageDescription @" << file;
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
