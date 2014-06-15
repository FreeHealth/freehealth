/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
/**
 * \class DataPack::IPackManager
 * IPackManager is the public interface available from the registered
 * in the DataPack::DataPackCore.
 *
 * With this manager, you can download, install, check the existence
 * and remove any datapacks. This manager lacks a pack dependencies checker.\n
 * The manager is available in the core: DataPack::DataPackCore::packManager()
 * \sa DataPack::DataPackCore, DataPack::IServerManager
 */

/** \fn virtual QList<Pack> DataPack::IPackManager::installedPack(bool forceRefresh = false) = 0;
 * Return the list of locally installed and ready to use datapacks. Datapacks are stored locally.
 */

/** \fn virtual bool DataPack::IPackManager::isDataPackInstalled(const Pack &pack) = 0;
 * Return true is the datapack is installed in the local datapack repository.
 */

/** \fn virtual bool DataPack::IPackManager::isDataPackInstalled(const QString &packUid, const QString &packVersion = QString::null) = 0;
 * Check if a datapack is installed and ready to use.
 */

/** \fn virtual bool DataPack::IPackManager::isPackInPersistentCache(const Pack &pack) = 0;
 * Check if a datapack is alreday available into the persistent cache (is it already
 * downloaded?).
 */

/** \fn virtual bool DataPack::IPackManager::downloadPack(const Pack &pack, QProgressBar *bar) = 0;
 * Download the datapack into the local persitent cache.
 */

/** \fn virtual bool DataPack::IPackManager::checkCachedPackFileIntegrity(const Pack &pack) = 0;
 * Check the downloaded and cached datapack integrity. When you request a datapack download,
 * the datapack is kept into a persistent cache. This method checks the integrity of the
 * cached downloaded datapack.
 */

/** \fn virtual bool DataPack::IPackManager::installDownloadedPack(const Pack &pack, QProgressBar *bar = 0) = 0;
 * Install a downloaded datapack. You first need to request the datapack download.
 * Return the status of the process.
 */

/** \fn virtual bool DataPack::IPackManager::removePack(const Pack &pack) = 0;
 * Removes a datapack from the local datapack installation. Return the status of the process.
 */

// SIG
/** \fn void DataPack::IPackManager::packDownloaded(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status);
 * Signal emitted when a pack is downloaded with or without error. You can get the download
 * status with the DataPack::ServerEngineStatus object.
 */

/** \fn void DataPack::IPackManager::packInstalled(const DataPack::Pack &pack);
 * Signal emitted when a pack is fully installed and ready to use.
 */

/** \fn void DataPack::IPackManager::packRemoved(const DataPack::Pack &pack);
 * Signal emitted when a pack is removed.
 */


