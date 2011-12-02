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
#include "server.h"

#include <utils/log.h>
#include <utils/versionnumber.h>
#include <translationutils/constanttranslations.h>

#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {
const char *const TAG_ROOT = "DataPackServer";
const char *const TAG_SERVERDESCRIPTION = "ServerDescription";
const char *const TAG_SERVERCONTENT = "ServerContents";
}

/** Create a server pointing to the URL \e url. The URL must be unique in the server's pool. */
Server::Server(const QString &url) :
    m_Connected(false),
    m_IsLocal(false)
{
    setUrl(url);
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
 * Reads the XML configuration content of the server and
 * create the DataPack::ServerDescription and the DataPack::ServerContent
 * related to this server.
*/
void Server::fromXml(const QString &fullServerConfigXml)
{
    QDomDocument doc;
    if (!doc.setContent(fullServerConfigXml)) {
        LOG_ERROR_FOR("Server", "Wrong XML");
        return;
    }
    QDomElement root = doc.firstChildElement(::TAG_ROOT);
    QDomElement descr = root.firstChildElement(::TAG_SERVERDESCRIPTION);
    QDomElement content = root.firstChildElement(::TAG_SERVERCONTENT);
    m_Desc.fromDomElement(descr);
    m_Content.fromDomElement(content);
}

//void Server::setXmlDescription(const QString &xml)
//{
//    if (!m_Desc.fromXmlContent(xml)) {
//        LOG_ERROR_FOR("DataPackServer", "Wrong XML description");
//        m_Desc.clear();
//    }
//}

//void Server::setXmlContent(const QString &xml)
//{
//    if (!m_Content.fromXml(xml)) {
//        LOG_ERROR_FOR("DataPackServer", "Wrong XML content");
//        m_Content.clear();
//    }
//}

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
