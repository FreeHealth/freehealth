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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "server.h"
#include "datapackcore.h"

#include <utils/log.h>
#include <utils/versionnumber.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QDir>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {
const char *const TAG_ROOT = "DataPackServer";
const char *const TAG_SERVERDESCRIPTION = "ServerDescription";
const char *const TAG_SERVERCONTENT = "ServerContents";

const char * const SERVER_CONF_XML = "server.conf.xml";
const char * const SERVER_CONF_ZIP = "serverconf.zip";
}

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }

/** Create a server pointing to the URL \e url. The URL must be unique in the server's pool. */
Server::Server(const QString &url) :
    m_Connected(false),
    m_IsLocal(false),
    m_UrlStyle(NoStyle),
    m_UpFreq(-1)
{
    setUrl(url);
}

/** Return true if the server is null or invalid. */
bool Server::isNull() const
{
    return m_Url.isEmpty() && uuid().isEmpty();
}

/** Return the recorded UUID extracted for the server description. \sa Server::fromXml(), ServerDescription */
QString Server::uuid() const
{
    const QString &uuid = m_Desc.data(ServerDescription::Uuid).toString();
    if (uuid.isEmpty()) {
        if (!m_Url.isEmpty())
            return m_Url.toAscii().toBase64();
    }
    return uuid;
}

/** Return the version of the server */
QString Server::version() const
{
    return m_Desc.data(ServerDescription::Version).toString();
}

/** Return the label/name of the server */
QString Server::name() const
{
    return m_Desc.data(ServerDescription::Label).toString();
}

/** Define the URL of the server. All URL must be unique (url is used as uuid). */
void Server::setUrl(const QString &url)
{
    m_IsLocal = false;
    m_Url.clear();
    if (url.startsWith("file://", Qt::CaseInsensitive)) {
        QString t = url;
        QFileInfo file(t.replace("file:/", ""));
        if (file.exists() && file.isDir()) {
            m_Url = url;
            m_IsLocal = true;
            m_Connected = true;
            LOG_FOR("DataPackServer", "Local server added. Path: " + t.replace("file:/", ""));
        } else {
            LOG_ERROR_FOR("DataPackServer", tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(url));
            m_Connected = false;
            return;
        }
    }
    m_Url = url;
}

/**
 * Returns the server Url according to the \e UrlStyle and the \e native \e url params for the requested \e file.
 */
QString Server::url(const FileRequested &file, const QString &fileName) const
{   // TODO: test and improve code
    switch (file) {
    case NoFile : return m_Url;
    case ServerConfigurationFile:
    {
        switch (m_UrlStyle) {
        case NoStyle:
        {
            QString t = m_Url;
            return QDir::cleanPath(t.replace("file:/", "")) + "/" + ::SERVER_CONF_XML;
        }
        case HttpPseudoSecuredAndZipped: return m_Url + "/get-" + ::SERVER_CONF_ZIP;
        case HttpPseudoSecuredNotZipped: return m_Url + "/" + ::SERVER_CONF_XML;
        case Http: return m_Url + "/" + ::SERVER_CONF_XML;
        case FtpZipped: return m_Url + "/" + ::SERVER_CONF_ZIP;
        case Ftp: return m_Url + "/" + ::SERVER_CONF_XML;
        }
        break;
    }
    case PackDescriptionFile:
    {
        Q_ASSERT(!fileName.isEmpty());
        if (fileName.isEmpty())
            break;

        switch (m_UrlStyle) {
        case NoStyle:
        {
            QFileInfo info(fileName);
            if (info.isRelative()) {
                QString t = m_Url;
                info.setFile(t.replace("file:/", "") + "/" + fileName);
            }
            return info.absoluteFilePath();
        }
        case HttpPseudoSecuredAndZipped: return m_Url + "/get-" + fileName;
        case HttpPseudoSecuredNotZipped: return m_Url + "/" + fileName;
        case Http: return m_Url + "/" + fileName;
        case FtpZipped: return m_Url + "/" + fileName;
        case Ftp: return m_Url + "/" + fileName;
        }
        break;
    }
    case PackFile:
    {
        switch (m_UrlStyle) {
        case NoStyle:
        {
            QString t = m_Url;
            return QDir::cleanPath(t.replace("file:/", "")) + "/" + fileName;
        }
        case HttpPseudoSecuredAndZipped: return m_Url + "/get-" + fileName;
        case HttpPseudoSecuredNotZipped: return m_Url + "/" + fileName;
        case Http: return m_Url + "/" + fileName;
        case FtpZipped: return m_Url + "/" + fileName;
        case Ftp: return m_Url + "/" + fileName;
        }
        break;
    }
    }
    return nativeUrl();
}

QString Server::urlStyleName(int index)
{
    switch (index) {
    case Server::NoStyle: return QApplication::translate("Server", "Local file");
    case Server::HttpPseudoSecuredAndZipped: return QApplication::translate("Server", "Protected HTTP with zipped content");
    case Server::HttpPseudoSecuredNotZipped: return QApplication::translate("Server", "Protected HTTP non-zipped");
    case Server::Http: return QApplication::translate("Server", "HTTP (standard mode)");
    case Server::FtpZipped: return QApplication::translate("Server", "FTP with zipped content");
    case Server::Ftp: return QApplication::translate("Server", "FTP (standard mode)");
    }
    return QString();
}

QString Server::urlStyleName() const
{
    return urlStyleName(m_UrlStyle);
}

/** Return true if the server needs a user authentification to proceed pack installation. */
bool Server::requiereAuthentification() const
{
    return m_Desc.data(ServerDescription::RequiereAuthentification).toBool();
}

/**
 * Reads the XML configuration content of the server and
 * create the DataPack::ServerDescription and the DataPack::ServerContent
 * related to this server.
*/
void Server::fromXml(const QString &fullServerConfigXml)
{
    QDomDocument doc;
    if (!doc.setContent(fullServerConfigXml)) {
        LOG_ERROR_FOR("DataPack::Server", "Wrong XML");
        return;
    }
    QDomElement root = doc.firstChildElement(::TAG_ROOT);
    QDomElement descr = root.firstChildElement(::TAG_SERVERDESCRIPTION);
    QDomElement content = root.firstChildElement(::TAG_SERVERCONTENT);
    m_Desc.fromDomElement(descr);
    m_Content.fromDomElement(content);
    m_UpFreq = m_Desc.data(ServerDescription::RecommendedUpdateFrequency).toInt();
}

/**
 * Return the DataPack::Server::UpdateState of the server according to
 * its description and the serverManager's data.
 */
Server::UpdateState Server::updateState() const
{
    if (m_LocalVersion.isEmpty())
        return Server::UpdateInfoNotAvailable;
    const QString &v = m_Desc.data(ServerDescription::Version).toString();
    if (v.isEmpty())
        return Server::UpdateInfoNotAvailable;
    Utils::VersionNumber local(m_LocalVersion);
    Utils::VersionNumber remote(v);
    if (local < remote)
        return Server::UpdateAvailable;
    return Server::UpToDate;
}

/** Return the recommended update frequency of the server. The index refers to Trans::Constants::CheckUpdateRepresentation */
int Server::recommendedUpdateFrequency() const
{
    return m_Desc.data(ServerDescription::RecommendedUpdateFrequency).toInt();
}

/** Test DataPack::Server equality. */
bool Server::operator==(const Server &s)
{
    return ((m_Url == s.url()) && (m_UrlStyle==s.urlStyle()) && (version()==s.version()));
}

/** Return the file name used for the server configuration file */
QString Server::serverConfigurationFileName()
{
    return ::SERVER_CONF_XML;
}

QString Server::serialize() const
{
    return m_Url + "|||" + QString::number(m_UrlStyle);
}

void Server::fromSerializedString(const QString &string)
{
    if (string.contains("|||")) {
        QStringList v = string.split("|||");
        if (v.count() == 2) {
            setUrl(v.at(0));
            setUrlStyle(m_UrlStyle=UrlStyle(v.at(1).toInt()));
        }
    }
}
