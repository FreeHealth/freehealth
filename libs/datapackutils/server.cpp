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

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

Server::Server(const QString &url, QObject *parent) :
    m_Connected(false), m_IsLocal(false)
{
    setUrl(url);
}

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

void Server::setXmlDescription(const QString &xml)
{
    if (!m_Desc.fromXmlContent(xml)) {
        LOG_ERROR_FOR("DataPackServer", "Wrong XML description");
        m_Desc.clear();
    }
}

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
