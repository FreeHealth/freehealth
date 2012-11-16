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
 */

#include "datapackcore.h"

#include <utils/global.h>
#include <translationutils/constants.h>
#include <datapackutils/packdescription.h>
#include <datapackutils/serverdescription.h>

#include <QFile>

#include <QDebug>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

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
    }
    
public:
    QMultiHash<QString, DataPackQuery> _serverDatapacks;


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
    const QList<DataPackQuery> &queries = d->_serverDatapacks.values(serverUid);
    foreach(const DataPackQuery &serverQuery, queries) {
        if (serverQuery==query)
            return true;
    }

    // add the query to the server
    d->_serverDatapacks.insertMulti(serverUid, query);
    return true;
}


///** Create a datapack structure using the DataPackPlugin::DataPackQuery */
//DataPackResult DataPackCore::createDataPack(const DataPackQuery &query)
//{
//    DataPackResult result;
//    if (!query.isValid())
//        return result;
//    QFile contentFile(query.originalContentFileAbsolutePath());

//    // get and update the packdescription
//    DataPack::PackDescription pack;
//    pack.setSourceFileName(query.descriptionFileAbsolutePath());
//    pack.setData(DataPack::PackDescription::AbsFileName, query.originalContentFileAbsolutePath());
//    pack.setData(DataPack::PackDescription::Size, contentFile.size());
//    pack.setData(DataPack::PackDescription::Md5, Utils::md5(query.originalContentFileAbsolutePath()));
//    pack.setData(DataPack::PackDescription::Sha1, Utils::sha1(query.originalContentFileAbsolutePath()));
//    pack.setData(DataPack::PackDescription::LastModificationDate, QDate::currentDate().toString(Qt::ISODate));
//    result.setPackDescriptionXmlContent(pack.toXml());
//    return result;
//}

//bool DataPackCore::updateServerConfiguration(const QString &absPath, const QString &serverDescriptionAbsPath)
//{
//}
