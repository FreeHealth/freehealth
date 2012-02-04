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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGBASECORE_H
#define DRUGBASECORE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <utils/database.h>

/**
 * \file drugsbasecore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 13 Dec 2011
*/
namespace DrugsDB {
namespace Internal {

class DRUGSBASE_EXPORT DrugBaseCore : public Utils::Database
{
public:
    DrugBaseCore();
    virtual ~DrugBaseCore() {}
    bool initialize(const QString &pathToDb, bool createIfNotExists = false);

    void setVersion(const QString &version);
    QString version() const;
    bool checkDatabaseVersion() const;

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

#endif // DRUGBASECORE_H
