/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "iservermanager.h"

/**
 * \class DataPack::IServerManager
 * Public interface of the server manager.
 * Usage:
 \code
   // Define the configuration of the manager
   manager->setGlobalConfiguration(xml);
   // manager->setLocalOutputPath();

   // If needed
   manager->addServer("http://my.url.com/");

   // Start the update screening process
   manager->checkServerUpdates();

   // [...]

   // Before deleting the manager
   save(manager->xmlConfiguration());  // This is important

 \endcode
 * \sa DataPack::DataPackCore, DataPack::DataPackCore::serverManager()
 */


/**
 * \fn virtual bool DataPack::IServerManager::setGlobalConfiguration(const QString &xmlContent, QString *errorMsg = 0)
 * Initialize all params of the DataPack::ServerManager using a XML'd content.
 * \return true is all goes fine
 * \param xmlContent the xml content of the configuration
 * \param errorMsg error message
 */


/**
 * \fn virtual void DataPack::IServerManager::connectServer(const Server &server, const ServerIdentification &ident = ServerIdentification())
 * Return the actual configuration XML'd.
 */


/**
 * \fn virtual bool DataPack::IServerManager::addServer(const QString &url)
 * Add a new server created with the default values and the specified \e url.
 * \sa DataPack::Server
 * \param url the URL of the target server
 * \return false if a server with the same URL already exists
 */

/**
 * \fn virtual bool DataPack::IServerManager::addServer(const DataPack::Server &server)
 * Add a fully configurated \e server.
 * \param url the URL of the target server
 * \return false if a server with the same URL already exists
 */

/**
 * \fn virtual int DataPack::IServerManager::serverCount() const
 * Return the number of managed servers.
 */


/**
 * \fn virtual Server DataPack::IServerManager::getServerAt(int index) const
 * Get the server at a specific index
 * \param index the server index
 * \return the server
 */


/**
 * \fn virtual int DataPack::IServerManager::getServerIndex(const QString &url) const
 * Get the index of the server for a specific URL
 * \param url the url from which we want the server index
 * \return the server index matching the url parameter or -1 if not found
 */


/**
 * \fn virtual void DataPack::IServerManager::removeServerAt(int index)
 * Remove a server
 * \param id the ID of the server to remove
 */


/**
 * \fn virtual void DataPack::IServerManager::connectAndUpdate(int index)
 * Connect and update a server infos. Asynchronous.
 * When the server is updated, the signal "serverInfosUpdated" is emitted.
 * \param index the index of the server to update infos of
 */


/**
 * \fn virtual void DataPack::IServerManager::checkServerUpdates()
 * Connect all servers, download to tmp dir descriptions (if needed -> check file date before ?)
 * and check if there is an update available. Get server to update with serverToUpdate().
 * \sa serverToUpdate()
 */

/**
 * \fn virtual QList<Pack> DataPack::IServerManager::getPackForServer(const Server &server);
 * Assuming that server is connected and all description files were downloaded, return the pack list related to the \e server.
 */

/**
 * \fn bool DataPack::IServerManager::isDataPackInstalled(const Pack &pack)
 * Return true if the \e pack server is actually installed (the manager will check the version of the pack too).
 */

/**
 * \fn bool DataPack::IServerManager::isDataPackInstalled(const QString &packUid, const QString &packVersion)
 * Return true if the pack identified by the uuid \e packUid and the version \e packVersion is actually installed.
 * If \e packVersion is empty, the manager will not check the pack version.
 */


/**
 * \fn bool DataPack::IServerManager::installDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar = 0);
 * Install the \e pack located on the \e server, alerting the user with \e progressBar.
 */

/**
 * \fn bool DataPack::IServerManager::removeDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar = 0);
 * Remove the \e pack located on the \e server, alerting the user with \e progressBar.
 */
/**
 * \fn bool DataPack::IServerManager::updateDataPack(const Server &server, const Pack &pack, QProgressBar *progressBar = 0);
 * Update the \e pack located on the \e server, alerting the user with \e progressBar.
 */

/**
 * \fn QList<Pack> DataPack::IServerManager::packDependencies(const Pack &pack, const PackDependencyData::TypeOfDependence &dependence);
 * \todo documentation
 */

/**
 * \fn virtual void DataPack::IServerManager::setInstallPath(const QString &absPath)
 * Define the path where the datapacks are installed.
*/


/**
 * \fn virtual QString DataPack::IServerManager::installPath() const
 * Return the path where the datapacks are installed.
*/


/**
 * \fn void DataPack::IServerManager::packInstalled(const Server &server, const Pack &pack);
 * Signal emitted when the \e pack is successfully installed from the \e server.
 * \sa installDataPack()
 */

/**
 * \fn void DataPack::IServerManager::serverUpdateChecked()
 * Signal emitted when all server configuration have been downloaded and version checked.
 * \sa checkServerUpdates()
 */

