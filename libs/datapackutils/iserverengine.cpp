/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class DataPack::IServerEngine
  Default virtual interface of server engine. The server engine assumes
  all data access. You can add them at in the DataPack::DataPackCore
  constructor.
*/

/** \fn virtual bool DataPack::IServerEngine::managesServer(const Server &server) = 0;
  Return true if the engine manages this \e server.
*/

/** \fn virtual void DataPack::IServerEngine::addToDownloadQueue(const ServerEngineQuery &query) = 0;
  Add a DataPack::ServerEngineQuery to the download queue. The download will not start
  untill you call the DataPack::IServerEngine::downloadQueue()
*/

/** \fn virtual int DataPack::IServerEngine::downloadQueueCount() const = 0;
  Return the number of DataPack::ServerEngineQuery in the engine's queue.
*/

/** \fn virtual bool DataPack::IServerEngine::startDownloadQueue() = 0;
  Starts all the download from the registered queue and return true if all
  goes fine. The signal DataPack::IServerEngine::queueDowloaded() is emitted
  when all jobs are completed. If a pack is downloaded the signal
  DataPack::IServerEngine::packDownloaded(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status)
  is emitted.
  \sa void DataPack::IServerEngine::addToDownloadQueue()
*/

/** \fn virtual bool DataPack::IServerEngine::stopJobsAndClearQueue() = 0;
  Stops any started jobs and clear the download queue.
*/

/** \fn virtual const ServerEngineStatus &DataPack::IServerEngine::lastStatus(const Pack &pack) = 0;
  Return the lastest associated status for the \e pack processing (download, install, removal).
*/

/** \fn virtual const ServerEngineStatus &DataPack::IServerEngine::lastStatus(const Server &server) = 0;
  Return the lastest associated status for the \e server processing.
*/

/** \fn void DataPack::IServerEngine::queueDowloaded();
  Signal emitted by the engine when all download jobs are completed.
  \sa void DataPack::IServerEngine::addToDownloadQueue()
  \sa bool DataPack::IServerEngine::startDownloadQueue()
*/

/** \fn void DataPack::IServerEngine::packDownloaded(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status);
  Signal emitted by the engine when a pack is fully downloaded.
  \sa void DataPack::IServerEngine::addToDownloadQueue()
  \sa bool DataPack::IServerEngine::startDownloadQueue()
*/


#include "iserverengine.h"
#include "iservermanager.h"

#include <QDebug>

using namespace DataPack;

IServerEngine::IServerEngine(QObject *parent) :
    QObject(parent)
{
}

QDebug operator<<(QDebug dbg, const DataPack::ServerEngineStatus &status)
{
    dbg.nospace() << "DataPack::ServerEngineStatus("
                  << "hasError:" << status.hasError
                  << "; dldCorrect:" << status.downloadCorrectlyFinished
                  << "; proxyErr:" << status.proxyIdentificationError
                  << "; identErr:" << status.serverIdentificationError
                  << "; errors:" << status.errorMessages.join("/")
                  << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const DataPack::ServerEngineStatus *status)
{
    if (!status) {
        dbg.nospace() << "DataPack::ServerEngineStatus(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *status);
}

