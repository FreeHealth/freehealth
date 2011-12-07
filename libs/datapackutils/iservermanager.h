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
#ifndef DATAPACK_ISERVERMANAGER_H
#define DATAPACK_ISERVERMANAGER_H

#include <datapackutils/datapack_exporter.h>

// IMPROVE THIS
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
class Core;

class DATAPACK_EXPORT IServerManager : public QObject
{
    Q_OBJECT
    friend class DataPack::Core;

protected:
    explicit IServerManager(QObject *parent = 0) : QObject(parent) {}

public:
    virtual ~IServerManager() {}

    // In use
    virtual bool setGlobalConfiguration(const QString &xmlContent, QString *errorMsg = 0) = 0;
    virtual QString xmlConfiguration() const = 0;

    virtual void connectServer(const Server &server, const ServerIdentification &ident = ServerIdentification()) = 0;

    virtual bool addServer(const QString &url) = 0;
    virtual int serverCount() const = 0;
    virtual Server getServerAt(int index) const = 0;
    virtual int getServerIndex(const QString &url) const = 0;
    virtual void removeServerAt(int index) = 0;

    virtual void connectAndUpdate(int index) = 0;
    virtual void checkServerUpdates() = 0;

    virtual QList<PackDescription> getPackDescription(const Server &server) = 0;
    virtual QList<Pack> getPackForServer(const Server &server) = 0;

    // Not actually in use
    virtual ServerDescription downloadServerDescription(const Server &server) = 0;
    virtual QList<PackDescription> downloadPackDescription(const Server &server, const Pack &pack) = 0;
    virtual Pack downloadAndUnzipPack(const Server &server, const Pack &pack) = 0;

    virtual bool installDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar = 0) = 0;
    virtual bool removeDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar = 0) = 0;
    virtual bool updateDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar = 0) = 0;

    virtual void setInstallPath(const QString &absPath) = 0;
    virtual QString installPath() const = 0;
protected:
    virtual QString cachePath() const = 0;

Q_SIGNALS:
    void serverConnected(const Server &server, const ServerIdentification &ident);
    void serverInfosUpdated(int serverId); // emitted when a server infos have been updated

    void packInstalled(const Server &server, const Pack &pack);
    void packRemoved(const Server &server, const Pack &pack);
    void packUpdated(const Server &server, const Pack &pack);

    // In use
    void serverUpdateChecked();
};

}  // End namespace DataPack


#endif // DATAPACK_ISERVERMANAGER_H
