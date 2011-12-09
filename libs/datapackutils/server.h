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
    enum UrlStyle {
        NoStyle = 0,                     // used for local file server (default)
        HttpPseudoSecuredAndZipped,      // eg: http://url/get-[serverconf]
        HttpPseudoSecuredNotZipped,      // eg: http://url/get-[server.conf.xml]
        Http,                            // eg: http://url/[server.conf.xml]
        FtpZipped,                       // eg: ftp://user@pass:url/[serverconf.zip]
        Ftp                              // eg: ftp://user@pass:url/[server.conf.xml]
    };
    enum FileRequested {
        NoFile = 0,                      // Computed URL of the server without any request
        ServerConfigurationFile,         // File URL on the server
        PackDescriptionFile
    };

    Server(const QString &url = QString::null);
    virtual ~Server() {}

    void setUrlStyle(const UrlStyle &style) {m_UrlStyle=style;}
    UrlStyle urlStyle() const {return m_UrlStyle;}

    void setUrl(const QString &url);
    const QString &nativeUrl() const {return m_Url;}
    QString url(const FileRequested &file = NoFile, const QString &fileName = QString::null) const;

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

    bool operator==(const Server &s);

    static QString serverConfigurationFileName();

    QString serialize() const;
    void fromSerializedString(const QString &string);

private:
    QString m_Url, m_LocalVersion;
    QDateTime m_LastCheck;
    bool m_Connected, m_IsLocal;
    ServerDescription m_Desc;
    ServerContent m_Content;
    QStringList m_Errors;
    UrlStyle m_UrlStyle;
};

}  // End namespace DataPack


#endif // SERVER_H
