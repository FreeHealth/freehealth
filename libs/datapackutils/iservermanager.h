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
#ifndef DATAPACK_ISERVERMANAGER_H
#define DATAPACK_ISERVERMANAGER_H

#include <datapackutils/datapack_exporter.h>

// IMPROVE THIS
#include <datapackutils/serverenginestatus.h>
#include <datapackutils/server.h>
#include <datapackutils/pack.h>
#include <datapackutils/serveridentification.h>
#include <datapackutils/serverdescription.h>
#include <datapackutils/packdescription.h>

#include <QObject>
class QProgressBar;

/**
 * \file iservermanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 30 Nov 2011
 * \warning Needs Qt >= 4.7
*/

namespace DataPack {
class DataPackCore;

class DATAPACK_EXPORT IServerManager : public QObject
{
    Q_OBJECT
    friend class DataPack::DataPackCore;

protected:
    explicit IServerManager(QObject *parent = 0) : QObject(parent) {}

public:
    virtual ~IServerManager() {}

    // In use
    virtual bool setGlobalConfiguration(const QString &xmlContent, QString *errorMsg = 0) = 0;
    virtual QString xmlConfiguration() const = 0;

    // Server list
    virtual bool addServer(const QString &url) = 0;
    virtual bool addServer(const Server &server) = 0;
    virtual int serverCount() const = 0;
    virtual Server getServerAt(int index) const = 0;
    virtual int getServerIndex(const QString &url) const = 0;
    virtual void removeServerAt(int index) = 0;

    // Downloads
    virtual void getServerDescription(const int index) = 0;
    virtual void getAllDescriptionFile(QProgressBar *bar = 0) = 0;
    virtual void checkServerUpdates() = 0;

    virtual QList<Pack> getPackForServer(const Server &server) = 0;
    virtual Server &getServerForPack(const Pack &pack) = 0;
//    virtual bool isPackInPersistentCache(const Pack &pack) = 0;

//    virtual QList<Pack> installedPack(bool forceRefresh = false) = 0;
//    virtual bool isDataPackInstalled(const Pack &pack) = 0;
//    virtual bool isDataPackInstalled(const QString &packUid, const QString &packVersion = QString::null) = 0;

    // Not actually in use
    virtual void connectServer(const Server &server, const ServerIdentification &ident = ServerIdentification()) = 0;
    virtual void connectAndUpdate(int index) = 0;

Q_SIGNALS:
    void allServerDescriptionAvailable();

    void serverAdded(const int index);
    void serverAboutToBeRemoved(const Server &server);
    void serverAboutToBeRemoved(const int index);
    void serverRemoved(const Server &server);
    void serverRemoved(const int index);

    void serverConnected(const Server &server, const ServerIdentification &ident);
    void serverInfosUpdated(int serverId); // emitted when a server infos have been updated

    // TODO: add signal including the ServerEngineStatus

    // In use
    void serverUpdateChecked();
};

}  // End namespace DataPack


#endif // DATAPACK_ISERVERMANAGER_H
