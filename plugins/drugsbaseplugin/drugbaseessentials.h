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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_INTERNAL_DRUGBASEESSENTIALS_H
#define DRUGSDB_INTERNAL_DRUGBASEESSENTIALS_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <utils/database.h>

/**
 * \file drugsbaseessentials.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.10.0
 * \date 12 Jan 2014
*/
namespace DrugsDB {
namespace Internal {

class DRUGSBASE_EXPORT DrugBaseEssentials : public Utils::Database
{
public:
    DrugBaseEssentials();
    virtual ~DrugBaseEssentials() {}
    void forceFullDatabaseRefreshing();
    bool initialize(const QString &pathToDb, bool createIfNotExists = false);

    bool checkDatabaseVersion() const;

    int getSourceId(const QString &drugsDbUid);
    bool isAtcAvailable() const;

private:
    bool createDatabase(const QString &connectionName , const QString &prefixedDbName,
                        const Utils::DatabaseConnector &connector,
                        CreationOption createOption
                       );

protected:
    bool m_dbcore_initialized;
    bool m_isDefaultDb;
};

}  // End namespace Internal
}  // End namespace DrugsDB

#endif // DRUGSDB_INTERNAL_DRUGBASEESSENTIALS_H
