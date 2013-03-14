/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *                                                                         *
 *  (C) 2013-now by Christian A Reiter and Eric Maeker                     *
 *  (C) 2010-2012 by Pierre-Marie Desombre and Eric Maeker                 *
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
 *  Main Developers :  Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT2_ACCOUNTBASEQUERY_H
#define ACCOUNT2_ACCOUNTBASEQUERY_H

#include <QDateTime>
#include <QStringList>

/**
 * \file accountbasequery.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 14 Mar 2013
*/

namespace Account2 {
namespace Internal {

class AccountBaseQuery
{
public:
    enum DataObject {
        Fee,
        Payment,
        Banking,
        Quotation,
        None
    };
    Q_DECLARE_FLAGS(DataObjects, DataObject)

    AccountBaseQuery(): _objects(None), _object(None), _invalid(false) {}
    ~AccountBaseQuery() {}

    // Get only one item
    void setRetrieveObject(DataObject object, const QString &uuid) {_object=object; _objectUuid=uuid;}
    bool retrieveObject() const {return _object;}
    QString retrieveObjectUuid() const {return _objectUuid;}

    // Get a list of items
    void setRetrieveObjects(DataObjects objects) {_objects=objects;}
    DataObjects retrieveObjects() const {return _objects;}

    void setIncludeInvalidObjects(bool invalid) {_invalid=invalid;}
    bool includeInvalidObjects() const {return _invalid;}

    void setStartDate(const QDateTime &start) {_start=start;}
    void setEndDate(const QDateTime &end) {_end=end;}
    void setDateRange(const QDateTime &start, const QDateTime &end) {_start=start;_end=end;}

    QDateTime startDate() const {return _start;}
    QDateTime endDate() const {return _end;}

    void addUser(const QString &uuid) {_userUids << uuid;}
    void addPatient(const QString &uuid) {_patientUids << uuid;}
    QStringList userUids() const {return _userUids;}
    QStringList patientUids() const {return _patientUids;}

private:
    DataObjects _objects;
    DataObject _object;
    QString _objectUuid;
    QDateTime _start, _end;
    bool _invalid;
    QStringList _userUids, _patientUids;
};

} // namespace Internal
} // namespace Account2

Q_DECLARE_OPERATORS_FOR_FLAGS(Account2::Internal::AccountBaseQuery::DataObjects)

#endif // ACCOUNT2_ACCOUNTBASEQUERY_H
