/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNTBASE_H
#define ACCOUNTBASE_H

#include <accountbaseplugin/accountbase_exporter.h>

#include <utils/database.h>

/**
 * \file accountbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date 29 Jun 2012
*/

namespace AccountDB {
class AccountData;

namespace Internal {
class AccountBasePlugin;
class AccountBasePrivate;
} // End namespace Internal

// TODO: AccountBase should be in Internal and not exported
class ACCOUNTBASE_EXPORT AccountBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class AccountDB::Internal::AccountBasePlugin;

protected:
    AccountBase(QObject *parent = 0);

public:
    static AccountBase *instance();
    ~AccountBase();

    // Initializer / Checkers
    bool initialize();
    bool isInitialized() const ;
    void logChronos(bool state);

private:
    bool createDatabase(const QString &connectionName , const QString &dbName,
                        const QString &pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString & /*login*/, const QString & /*pass*/,
                        const int port,
                        CreationOption /*createOption*/
                       );
    AccountData *getAccountByUid(const QString &uid);

    bool checkIfIsFirstVersion();
    bool versionHasChanged();
    QString checkAndReplaceVersionNumber();
    bool checkIfVersionBeforeThirdVersion();
    bool alterFieldPatientNameIntToVarchar();

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

private:
    // intialization state
    static AccountBase *m_Instance;
    Internal::AccountBasePrivate *d;
};


}


#endif // ACCOUNTBASE_H
