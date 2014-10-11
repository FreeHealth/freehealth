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
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef GUARD_GUARDBASE_H
#define GUARD_GUARDBASE_H

#include <guardplugin/guard_exporter.h>
#include <utils/database.h>
//#include <account2plugin/database/accountbasequery.h>
//#include <account2plugin/database/accountbaseresult.h>

/**
 * \file guardbase.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 10 Oct 2014
*/

namespace Guard {
class GuardCore;

namespace Internal {
class GuardBasePrivate;

class GuardBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Guard::GuardCore;

protected:
    GuardBase(QObject *parent = 0);

public:
    ~GuardBase();

    // Initializer / Checkers
    bool initialize();
    bool isInitialized() const ;
    void logChronos(bool state);
    bool checkDatabaseVersion();

//    AccountBaseResult query(const AccountBaseQuery &query);

    bool createVirtuals(int nb);

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
    Internal::GuardBasePrivate *d;
};

} // namespace Internal
} // namespace Guard

#endif // GUARD_GUARDBASE_H
