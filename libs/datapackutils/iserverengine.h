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
#ifndef DATAPACK_ISERVERENGINE_H
#define DATAPACK_ISERVERENGINE_H

#include <datapackutils/datapack_exporter.h>
#include <datapackutils/iservermanager.h>
#include <datapackutils/serverenginestatus.h>
#include <datapackutils/server.h>
#include <datapackutils/pack.h>
#include <datapackutils/serveridentification.h>
#include <QObject>

/**
 * \file iserverengine.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 30 Jan 2012
 * \warning Needs Qt >= 4.7
*/

namespace DataPack {

struct DATAPACK_EXPORT ServerEngineQuery {
    ServerEngineQuery() :
        server(0), ident(0), pack(0), progressBar(0),
        forceDescriptionFromLocalCache(false),
        downloadDescriptionFiles(false),
        downloadPackFile(false)
    {}

    Server *server;
    ServerIdentification *ident;
    const Pack *pack;
    QProgressBar *progressBar;
    bool forceDescriptionFromLocalCache;
    bool downloadDescriptionFiles;
    bool downloadPackFile;
};

class DATAPACK_EXPORT IServerEngine : public QObject
{
    Q_OBJECT
public:
    explicit IServerEngine(QObject *parent = 0);

    virtual bool managesServer(const Server &server) = 0;
    virtual void addToDownloadQueue(const ServerEngineQuery &query) = 0;
    virtual int downloadQueueCount() const = 0;
    virtual bool startDownloadQueue() = 0;

    virtual int runningJobs() const = 0;
    virtual bool stopJobsAndClearQueue() = 0;

    virtual const ServerEngineStatus &lastStatus(const Pack &pack) = 0;
    virtual const ServerEngineStatus &lastStatus(const Server &server) = 0;

Q_SIGNALS:
    void queueDowloaded();
    void packDownloaded(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status);
};

} // namespace DataPack

DATAPACK_EXPORT QDebug operator<<(QDebug dbg, const DataPack::ServerEngineStatus &status);
DATAPACK_EXPORT QDebug operator<<(QDebug dbg, const DataPack::ServerEngineStatus *status);

#endif // DATAPACK_ISERVERENGINE_H
