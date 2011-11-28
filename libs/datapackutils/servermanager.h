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
#ifndef DATAPACK_SERVERMANAGER_H
#define DATAPACK_SERVERMANAGER_H

#include <datapackutils/datapack_exporter.h>
#include <datapackutils/server.h>
#include <datapackutils/pack.h>
#include <datapackutils/serveridentification.h>
#include <datapackutils/serverdescription.h>
#include <datapackutils/packdescription.h>

/**
 * \file servermanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 25 Nov 2011
 * Needs Qt >= 4.7
*/

namespace DataPack {

class ServerManagerPrivate;

class DATAPACK_EXPORT ServerManager : public QObject
{
    Q_OBJECT
public:
    /**
     * Construct an empty valid object with the back up path set to \e filesCachePath.
	 * \param filesCachePath All downloaded files will be stored in this place.
    */
    explicit ServerManager(const QString &filesCachePath, QObject *parent = 0);

    bool isInternetConnexionAvailable();

    void connectServer(const Server &server, const ServerIdentification &ident = ServerIdentification()); // will be deprecated regarding the connectAndUpdate function

    /**
     * Add a new server
     * \param url the URL of the target server
     * \return false if a server with the same URL already exists
     */
    bool addServer(const QUrl &url);

    /**
     * Get the server at a specific index
     * \param index the server index
     * \return the server
     */
    Server *getServerAt(int index) const;

    /**
     * Get the index of the server for a specific URL
     * \param url the url from which we want the server index
     * \return the server index matching the url parameter or -1 if not found
     */
    int getServerIndex(const QUrl &url) const;

    /**
     * Remove a server
     * \param id the ID of the server to remove
     */
    void removeServerAt(int index);

    /**
     * Connect and update a server infos. Asynchronous.
     * When the server is updated, the signal "serverInfosUpdated" is emitted.
     * \param index the index of the server to update infos of
     */
    void connectAndUpdate(int index);

    ServerDescription downloadServerDescription(const Server &server);
    QList<PackDescription> downloadPackDescription(const Server &server, const Pack &pack);
    Pack downloadAndUnzipPack(const Server &server, const Pack &pack);

    bool installDataPack(const Server &server, const Pack &pack);

Q_SIGNALS:
    void serverConnected(const Server &server, const ServerIdentification &ident);
    void serverInfosUpdated(int serverId); // emitted when a server infos have been updated

private:
	ServerManagerPrivate *m_d;
};

}  // End namespace DataPack

#endif // DATAPACK_SERVERMANAGER_H
