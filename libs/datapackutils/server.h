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
#ifndef DATAPACK_SERVER_H
#define DATAPACK_SERVER_H

#include <datapackutils/datapack_exporter.h>
#include <datapackutils/serverdescription.h>
#include <datapackutils/servercontent.h>

#include <QString>
#include <QDateTime>
#include <QStringList>

/**
 * \file server.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 02 Dec 2011
*/

namespace DataPack {

class DATAPACK_EXPORT Server
{
public:
    enum UpdateState {
        UpdateAvailable = 0,
        UpToDate,
        UpdateInfoNotAvailable
    };

    Server(const QString &url = QString::null);
    virtual ~Server() {}

    void setUrl(const QString &url);
    const QString &url() const {return m_Url;}

    void setLocalVersion(const QString &version) {m_LocalVersion=version;}
    const QString &localVersion() const {return m_LocalVersion;}

    void setLastChecked(const QDateTime &dt) {m_LastCheck=dt;}
    const QDateTime &lastChecked() const {return m_LastCheck;}

    void setConnected(bool connected) {m_Connected=connected;}
    bool isConnected() const {return m_Connected;}

    void setIsLocalServer(bool isLocal) {m_IsLocal=isLocal;}
    bool isLocalServer() const {return m_IsLocal;}

    void fromXml(const QString &fullServerConfigXml);

    ServerDescription description() const {return m_Desc;}

    ServerContent content() const {return m_Content;}

    UpdateState updateState() const;

    void addErrorMessage(const QString &error) {m_Errors.append(error);}
    QStringList errors() const {return m_Errors;}


    // OBSOLETE
    void connectAndUpdate() {}

//private:
//    void setXmlDescription(const QString &xml);
//    void setXmlContent(const QString &xml);

private:
    QString m_Url, m_LocalVersion;
    QDateTime m_LastCheck;
    bool m_Connected, m_IsLocal;
    ServerDescription m_Desc;
    ServerContent m_Content;
    QStringList m_Errors;
};

}  // End namespace DataPack


#endif // SERVER_H
