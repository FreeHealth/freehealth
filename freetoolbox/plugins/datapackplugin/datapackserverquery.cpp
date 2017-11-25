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
#include "datapackserverquery.h"

#include <utils/global.h>

using namespace DataPackPlugin;

/** Create an empty object */
DataPackServerQuery::DataPackServerQuery() :
    _autoVersion(true)
{
}

/** Test validity of the object (return false if object is not valid) */
bool DataPackServerQuery::isValid() const
{
    if (_internalUid.isEmpty())
        return false;

    if (_absPathServer.isEmpty())
        return false;
//    foreach(const DataPackQuery &query, _dataPackQueries) {
//        if (!query.isValid())
//            return false;
//    }
    return true;
}

//void DataPackServerQuery::setDataPackQueries(const QList<DataPackQuery> &queries)
//{
//    _dataPackQueries = queries;
//}

//void DataPackServerQuery::addDataPackQueries(const QList<DataPackQuery> &queries)
//{
//    _dataPackQueries << queries;
//}

//void DataPackServerQuery::addDataPackQuery(const DataPackQuery &query)
//{
//    _dataPackQueries << query;
//}

/** Create needed dirs */
bool DataPackServerQuery::createDirs() const
{
    if (!Utils::checkDir(_absPathServer, true, "DataPackServerQuery::ServerRootPath"))
            return false;
    // Create each datapack path
//    foreach(const DataPackQuery &query, _dataPackQueries) {
//        if (!Utils::checkDir(query. + , true, "DataPackServerQuery::DataPack"))
//            return false;
//    }
    return true;
}

///** Create a datapack structure using the DataPackPlugin::DataPackQuery */
//DataPackResult DataPackServerQuery::createDataPack(const DataPackQuery &query)
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
//    pack.setData(DataPack::PackDescription::Md5, Utils::fileMd5(query.originalContentFileAbsolutePath()));
//    pack.setData(DataPack::PackDescription::Sha1, Utils::fileSha1(query.originalContentFileAbsolutePath()));
//    pack.setData(DataPack::PackDescription::LastModificationDate, QDate::currentDate().toString(Qt::ISODate));
//    result.setPackDescriptionXmlContent(pack.toXml());
//    return result;
//}
