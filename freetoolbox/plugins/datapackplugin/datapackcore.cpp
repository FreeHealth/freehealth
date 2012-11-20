/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DataPackPlugin::DataPackCore
 * Central place where user can register datapack to servers, get server content and description,
 * recreate/update server. \n
 * Servers references (DataPackPlugin::DataPackServerQuery) are created internally.
 */

#include "datapackcore.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <datapackutils/packdescription.h>
#include <datapackutils/serverdescription.h>

#include <QFile>
#include <QDir>

#include <QDebug>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

DataPackCore *DataPackCore::_instance = 0;

namespace DataPackPlugin {
namespace Internal {
class DataPackCorePrivate
{
public:
    DataPackCorePrivate(DataPackCore *parent) :
        q(parent)
    {
    }
    
    ~DataPackCorePrivate()
    {
        qDeleteAll(_serverDatapacks);
        qDeleteAll(_servers);
    }

    // Return the server output path
    QString serverOutputAbsPath()
    {
        return QDir::cleanPath(settings()->value(Core::Constants::S_DATAPACK_SERVER_OUTPUT_PATH).toString());
    }

    // Return the path to the description files (inside global_resources)
    QString serverDescriptionFileAbsPath()
    {
        return QString("%1/%2/servers")
                .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES);
    }

    bool createServers()
    {
        if (_servers.count() > 0)
            return true;

        DataPackServerQuery *freeCommunityServer = new DataPackServerQuery;
        DataPackServerQuery *nonFreeCommunityServer = new DataPackServerQuery;
        DataPackServerQuery *freeFrenchAssoServer = new DataPackServerQuery;
        DataPackServerQuery *nonFreeFrenchAssoServer = new DataPackServerQuery;

        freeCommunityServer->setServerInternalUuid(Core::Constants::SERVER_COMMUNITY_FREE);
        freeCommunityServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/freecommunity/server.conf");
        freeCommunityServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/freecommunity");

        nonFreeCommunityServer->setServerInternalUuid(Core::Constants::SERVER_COMMUNITY_NONFREE);
        nonFreeCommunityServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/nonfreecommunity/server.conf");
        nonFreeCommunityServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/nonfreecommunity");

        freeFrenchAssoServer->setServerInternalUuid(Core::Constants::SERVER_ASSO_FREE);
        freeFrenchAssoServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/freeasso/server.conf");
        freeFrenchAssoServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/freeasso");

        nonFreeFrenchAssoServer->setServerInternalUuid(Core::Constants::SERVER_ASSO_NONFREE);
        nonFreeFrenchAssoServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/nonfreeasso/server.conf");
        nonFreeFrenchAssoServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/nonfreeasso");

        _servers.append(freeCommunityServer);
        _servers.append(nonFreeCommunityServer);
        _servers.append(freeFrenchAssoServer);
        _servers.append(nonFreeFrenchAssoServer);

        return true;
    }
    
public:
    QMultiHash<QString, *DataPackQuery> _serverDatapacks;
    QVector<DataPackServerQuery*> _servers;

private:
    DataPackCore *q;
};
}  // namespace Internal
} // end namespace DataPackPlugin

/*! Constructor of the DataPackPlugin::DataPackCore class */
DataPackCore::DataPackCore(QObject *parent) :
    QObject(parent),
    d(new DataPackCorePrivate(this))
{
    _instance = this;
}

/*! Destructor of the DataPackPlugin::DataPackCore class */
DataPackCore::~DataPackCore()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DataPackCore::initialize()
{
    return true;
}

/**
 * Register a DataPackQuery inside a server. You can register multiple DataPackQuery for one server.
 * All queries will be processed and added to the server.\n
 * For the server internal uuid look at Core::Constants all SERVER_* constants. \n
 */
bool DataPackCore::registerDataPack(const DataPackQuery &query, const QString &serverUid)
{
    // avoid duplicates
    const QList<DataPackQuery*> &queries = d->_serverDatapacks.values(serverUid);
    foreach(DataPackQuery *serverQuery, queries) {
        if (*serverQuery==query)
            return true;
    }

    // add the query to the server
    DataPackQuery *newquery = new DataPackQuery(query);
    d->_serverDatapacks.insertMulti(serverUid, newquery);
    return true;
}

/** Return all the registered datapacks for one server identified by its \e serverUid */
QList<DataPackQuery> DataPackCore::serverRegisteredDatapacks(const QString &serverUid) const
{
    QList<DataPackQuery> toReturn;
    const QList<DataPackQuery*> &queries = d->_serverDatapacks.values(serverUid);
    foreach(DataPackQuery *serverQuery, queries)
       toReturn << *serverQuery;
    return toReturn;
}

/** Create all datapack server and return the list of servers */
QList<DataPackServerQuery> DataPackCore::servers() const
{
    QList<DataPackServerQuery> toReturn;
    if (!d->createServers())
        return toReturn;
    for(int i=0; i < d->_servers.count(); ++i)
        toReturn << *d->_servers.at(i);
    return toReturn;
}


//bool DataPackCore::updateServerConfiguration(const QString &absPath, const QString &serverDescriptionAbsPath)
//{
//}
