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

/**
 * \file iservermanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 30 Nov 2011
 * Needs Qt >= 4.7
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

    /**
     * Initialize all params of the DataPack::ServerManager using a XML'd content.
     * \return true is all goes fine
     * \param xmlContent the xml content of the configuration
     * \param errorMsg error message
     */
    virtual bool setGlobalConfiguration(const QString &xmlContent, QString *errorMsg = 0) = 0;
    virtual QString xmlConfiguration() const = 0;

    virtual void connectServer(const Server &server, const ServerIdentification &ident = ServerIdentification()) = 0;

    /**
     * Add a new server
     * \param url the URL of the target server
     * \return false if a server with the same URL already exists
     */
    virtual bool addServer(const QString &url) = 0;

    /**
     * Get the server at a specific index
     * \param index the server index
     * \return the server
     */
    virtual Server getServerAt(int index) const = 0;

    /**
     * Get the index of the server for a specific URL
     * \param url the url from which we want the server index
     * \return the server index matching the url parameter or -1 if not found
     */
    virtual int getServerIndex(const QString &url) const = 0;

    /**
     * Remove a server
     * \param id the ID of the server to remove
     */
    virtual void removeServerAt(int index) = 0;

    /**
     * Connect and update a server infos. Asynchronous.
     * When the server is updated, the signal "serverInfosUpdated" is emitted.
     * \param index the index of the server to update infos of
     */
    virtual void connectAndUpdate(int index) = 0;

    virtual ServerDescription downloadServerDescription(const Server &server) = 0;
    virtual QList<PackDescription> downloadPackDescription(const Server &server, const Pack &pack) = 0;
    virtual Pack downloadAndUnzipPack(const Server &server, const Pack &pack) = 0;

    virtual bool installDataPack(const Server &server, const Pack &pack) = 0;

protected:
    virtual QString cachePath() const = 0;

Q_SIGNALS:
    void serverConnected(const Server &server, const ServerIdentification &ident);
    void serverInfosUpdated(int serverId); // emitted when a server infos have been updated

};

}  // End namespace DataPack


#endif // DATAPACK_ISERVERMANAGER_H
