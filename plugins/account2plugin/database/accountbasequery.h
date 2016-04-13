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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT2_ACCOUNTBASEQUERY_H
#define ACCOUNT2_ACCOUNTBASEQUERY_H

#include <account2plugin/basicfilter.h>

#include <QDateTime>
#include <QStringList>

/**
 * \file accountbasequery.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 17 Mar 2013
*/

namespace Account2 {
namespace Internal {

class AccountBaseQuery : public BasicFilter
{
public:
    enum DataObject {
        None                = 0,
        Fee                 = 0x0001,
        Payment             = 0x0002,
        Banking             = 0x0004,
        Quotation           = 0x0008,
        MedicalProcedure    = 0x0010
    };
    Q_DECLARE_FLAGS(DataObjects, DataObject)

    AccountBaseQuery(): BasicFilter(), _objects(None), _object(None) {}
    AccountBaseQuery(const BasicFilter &filter): BasicFilter(filter), _objects(None), _object(None) {}
    ~AccountBaseQuery() {}

    // Get only one item
    void setRetrieveObject(DataObject object, const QString &uuid) {_object=object; _objectUuid=uuid;}
    DataObject retrieveObject() const {return _object;}
    QString retrieveObjectUuid() const {return _objectUuid;}

    // Get a list of items
    void setRetrieveObjects(DataObjects objects) {_objects=objects;}
    DataObjects retrieveObjects() const {return _objects;}

private:
    DataObjects _objects;
    DataObject _object;
    QString _objectUuid;
};

} // namespace Internal
} // namespace Account2

Q_DECLARE_OPERATORS_FOR_FLAGS(Account2::Internal::AccountBaseQuery::DataObjects)

#endif // ACCOUNT2_ACCOUNTBASEQUERY_H
