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
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT2_ACCOUNTBASE_H
#define ACCOUNT2_ACCOUNTBASE_H

#include <account2plugin/account2_exporter.h>
#include <utils/database.h>
#include <account2plugin/database/accountbasequery.h>
#include <account2plugin/database/accountbaseresult.h>

/**
 * \file accountbase.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 13 Mar 2013
*/

namespace Account2 {
class AccountCore;

namespace Internal {
class Account2Plugin;
class AccountBasePrivate;

class AccountBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Account2::AccountCore;

protected:
    AccountBase(QObject *parent = 0);

public:
    ~AccountBase();

    // Initializer / Checkers
    bool initialize();
    bool isInitialized() const ;
    void logChronos(bool state);

    AccountBaseResult query(const AccountBaseQuery &query);

    bool createVirtuals(int nb);

    bool save(Fee &fee);
    bool save(Payment &payment);
    bool save(Banking &banking);
    bool save(Quotation &quotation);
    bool save(QList<Fee> &fees);
    bool save(QList<Payment> &payments);
    bool save(QList<Banking> &bankings);
    bool save(QList<Quotation> &quotations);
    bool save(QList<MedicalProcedure> &medicalProcedures);

private:
    bool createDatabase(const QString &connectionName , const QString &dbName,
                        const QString &pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString & /*login*/, const QString & /*pass*/,
                        const int port,
                        CreationOption /*createOption*/
                       );

//    bool checkIfIsFirstVersion();
//    bool versionHasChanged();
//    QString checkAndReplaceVersionNumber();
//    bool checkIfVersionBeforeThirdVersion();
//    bool alterFieldPatientNameIntToVarchar();

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

private:
    Internal::AccountBasePrivate *d;
};

} // namespace Internal
} // namespace Account2

#endif // ACCOUNT2_ACCOUNTBASE_H
