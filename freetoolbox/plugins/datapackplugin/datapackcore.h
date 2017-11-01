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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACKPLUGIN_DATAPACKCORE_H
#define DATAPACKPLUGIN_DATAPACKCORE_H

#include <datapackplugin/datapack_exporter.h>
#include <datapackplugin/datapackquery.h>
#include <datapackplugin/datapackresult.h>
#include <datapackplugin/datapackserverquery.h>

#include <QObject>

/**
 * \file datapackcore.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 20 Nov 2012
*/

namespace DataPack {
class ServerDescription;
}

namespace DataPackPlugin {
namespace Internal {
class DataPackCorePrivate;
class DataPackIPlugin;
}  // namespace Internal

class DATAPACK_PLUGIN_EXPORT DataPackCore : public QObject
{
    Q_OBJECT
    friend class DataPackPlugin::Internal::DataPackIPlugin;

protected:
    explicit DataPackCore(QObject *parent = 0);
    bool initialize();

public:
    static DataPackCore *instance() {return _instance;}
    ~DataPackCore();

public Q_SLOTS:
    bool refreshServerDataPacks(const QString &serverUid);
    bool registerDataPack(const DataPackQuery &query, const QString &serverUid);

public:
    QList<DataPackQuery> serverRegisteredDatapacks(const QString &serverUid) const;
    QList<DataPackServerQuery> servers() const;
    const DataPackServerQuery &server(const QString &serverUid) const;
    const DataPack::ServerDescription &serverDescription(const QString &serverUid);

    //bool updateServer(const QString &serverUid);
    bool createServer(const QString &serverUid);

private:
    Internal::DataPackCorePrivate *d;
    static DataPackCore *_instance;
};

} // namespace DataPackPlugin

#endif  // DATAPACKPLUGIN_DATAPACKCORE_H

