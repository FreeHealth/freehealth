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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_DATABASECONNECTOR_H
#define UTILS_DATABASECONNECTOR_H

#include <utils/global_exporter.h>
#include <utils/database.h>

/**
 * \file databaseconnector.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 29 May 2011
*/

namespace Utils {
namespace Internal {
class DatabaseConnectorPrivate;
}

class UTILS_EXPORT DatabaseConnector
{
public:
    enum AccessMode {
        ReadOnly = 0,
        ReadWrite
    };

    DatabaseConnector();
    DatabaseConnector(const QString &clearLog, const QString &clearPass, const QString &hostName, const int port);
    DatabaseConnector(const QString &clearLog, const QString &clearPass);
    ~DatabaseConnector();

    void clear();
    bool isValid() const;

    void setDriver(const ::Utils::Database::AvailableDrivers driver);
    void setClearLog(const QString &log);
    void setClearPass(const QString &pass);
    void setHost(const QString &hostName);
    void setPort(const int port);
    void setAbsPathToReadOnlySqliteDatabase(const QString &absPath);
    void setAbsPathToReadWriteSqliteDatabase(const QString &absPath);
    void setAccessMode(const AccessMode mode);
    void setSqliteUsesExactFile(bool exactFile);
    void setGlobalDatabasePrefix(const QString &prefix);

    Database::AvailableDrivers driver() const;
    QString clearLog() const;
    QString clearPass() const;
    QString cryptedLog() const;
    QString cryptedPass() const;
    QString host() const;
    int port() const;
    QString absPathToSqliteReadOnlyDatabase() const;
    QString absPathToSqliteReadWriteDatabase() const;
    AccessMode accessMode() const;
    bool isDriverValid() const;
    bool useExactFile() const;
    QString globalDatabasePrefix() const;

    void fromSettings(const QString &value);

    DatabaseConnector &operator=(const DatabaseConnector &in);
    bool operator==(const DatabaseConnector &other) const;

    QString toString() const;

private:
    Internal::DatabaseConnectorPrivate *d;
};

}  // End namespace Utils

UTILS_EXPORT QDebug operator<<(QDebug dbg, const Utils::DatabaseConnector &c);

#endif // UTILS_DATABASECONNECTOR_H
