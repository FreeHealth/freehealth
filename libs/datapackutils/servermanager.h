/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef DATAPACK_SERVERMANAGER_H
#define DATAPACK_SERVERMANAGER_H

#include <datapackutils/iservermanager.h>
#include <datapackutils/server.h>
QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QSignalMapper;
QT_END_NAMESPACE

#include <QNetworkReply>
#include <QVector>
#include <QMultiHash>
#include <QString>
#include <QPointer>

/**
 * \file servermanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 02 Dec 2011
 * \warning Needs Qt >= 4.7
*/

namespace DataPack {
class IServerEngine;

namespace Internal {
class ServerManagerPrivate;
class LocalServerEngine;
class HttpServerEngine;

class ServerManager : public IServerManager
{
    Q_OBJECT
public:
    explicit ServerManager(QObject *parent = 0);
    ~ServerManager();
    void init(const QVector<DataPack::IServerEngine*> &engines);

    // IServerManager interface
    // Manage path
    bool setGlobalConfiguration(const QString &xmlContent, QString *errorMsg = 0);
    QString xmlConfiguration() const;

    // Server list
    bool addServer(const QString &url);
    bool addServer(const Server &server);
    int serverCount() const;
    Server getServerAt(int index) const;
    int getServerIndex(const QString &url) const;
    void removeServerAt(int index);

    // Downloads
    void getServerDescription(const int index);
    void getAllDescriptionFile(QProgressBar *bar = 0);
    void checkServerUpdates();

private Q_SLOTS:
    void engineDescriptionDownloadDone();


    ////////////////////////////////////////////////
public:
    // In use
    void registerPack(const Server &server, const Pack &pack);

    Server &getServerForPack(const Pack &pack);
    // End

    ////////////////////////////////////////////////
    void connectServer(const Server &server, const ServerIdentification &ident = ServerIdentification()); // will be deprecated regarding the connectAndUpdate function
    void connectAndUpdate(int index);

    QList<Pack> getPackForServer(const Server &server);

//    void checkInstalledPacks();
//    QList<Pack> installedPack(bool forceRefresh = false);
//    bool isDataPackInstalled(const Pack &pack);
//    bool isDataPackInstalled(const QString &packUid, const QString &packVersion = QString::null);

protected:
    QString cachePath() const;

public:
    // Hidden Private part
    const QVector<Server> &servers() const {return m_Servers;}
    void createServerPackList(const Server &server);
    const QVector<DataPack::IServerEngine*> &serverEngines() const {return m_WorkingEngines;}

private Q_SLOTS:
//    bool downloadDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar);
//    void checkAndInstallPack(const Server &server, const Pack &pack, QProgressBar *progBar);


private:
    // void checkServerUpdatesAfterDownload();

private:
    QVector<Server> m_Servers;
    QMultiHash<QString, Pack> m_Packs;
    QList<Pack> m_PacksToInstall;
    QList<Pack> m_InstalledPacks;
    Server m_NullServer;

    QVector<DataPack::IServerEngine *> m_WorkingEngines;
//    FtpServerEngine *m_FtpEngine;

    QPointer<QProgressBar> m_ProgressBar;
};

}  // End namespace Internal
}  // End namespace DataPack

#endif // DATAPACK_SERVERMANAGER_H
