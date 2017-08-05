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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Utils::DatabaseConnector
  This class owns all parameters needed to connect to the databases.\n
  The Core::ISettings is in charge of populating paths for the SQLite driver.\n
  Default params are:
  - log: empty
  - pass: empty
  - host: empty
  - port: -1 (undefined)
  - driver: SQLite
  - accessMode: ReadWrite
  \sa Utils::Database
  \note Unit-test available (see: tests/auto/auto.pro)
*/

#include "databaseconnector.h"
#include "global.h"
#include "log.h"
#include "passwordandlogin.h"

#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <QApplication>
#include <QFileInfo>

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
    const char *const SEPARATOR = "_@:";
}

namespace Utils {
namespace Internal {
class DatabaseConnectorPrivate
{
public:
    DatabaseConnectorPrivate() :
        m_Port(-1),
        m_DriverIsValid(false),
        m_UseExactFile(false),
        m_Driver(Database::SQLite),
        m_AccessMode(DatabaseConnector::ReadOnly)
    {}

    bool testDriver(Database::AvailableDrivers drv)
    {
        switch (drv) {
        case Database::MySQL:
        {
            if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
                LOG_ERROR_FOR("DatabaseConnector", tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("MySQL"));
                Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                         tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("MySQL"),
                                         "", qApp->applicationName());
                return false;
            }
            break;
        }
        case Database::SQLite:
        {
            if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
                LOG_ERROR_FOR("DatabaseConnector", tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("SQLite"));
                Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                         tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("SQLite"),
                                         "", qApp->applicationName());
                return false;
            }
            break;
        }
        default: return false;
        } // switch
        return true;
    }

    QString m_ClearLog, m_ClearPass, m_HostName;
    QString m_AbsPathToReadOnlySQLiteDb, m_AbsPathToReadWriteSQLiteDb;
    QString m_GlobalDatabasePrefix;
    int m_Port;
    bool m_DriverIsValid, m_UseExactFile;
    Database::AvailableDrivers m_Driver;
    DatabaseConnector::AccessMode m_AccessMode;
};
} // namespace Internal
} // namespace Utils

/** Construct an empty invalid connector. */
DatabaseConnector::DatabaseConnector() :
    d(new DatabaseConnectorPrivate)
{
    d->m_Port = -1;
    d->m_Driver = Database::SQLite;
    d->m_AccessMode = ReadWrite;
    d->m_DriverIsValid = d->testDriver(Database::SQLite);
}

DatabaseConnector::DatabaseConnector(const QString &clearLog, const QString &clearPass, const QString &hostName, const int port) :
    d(new DatabaseConnectorPrivate)
{
    d->m_ClearLog = clearLog;
    d->m_ClearPass = clearPass;
    d->m_HostName = hostName;
    d->m_Port = port;
    d->m_Driver = Database::SQLite;
    d->m_AccessMode = ReadWrite;
    d->m_DriverIsValid = d->testDriver(Database::SQLite);
}

DatabaseConnector::DatabaseConnector(const QString &clearLog, const QString &clearPass) :
    d(new DatabaseConnectorPrivate)
{
    d->m_ClearLog = clearLog;
    d->m_ClearPass = clearPass;
    d->m_Driver = Database::SQLite;
    d->m_AccessMode = ReadWrite;
    d->m_DriverIsValid = d->testDriver(Database::SQLite);
    d->m_Port = -1;
}

DatabaseConnector::~DatabaseConnector()
{
}

void DatabaseConnector::setDriver(Database::AvailableDrivers driver)
{
    d->m_Driver = driver;
    d->m_DriverIsValid = d->testDriver(driver);
}

void DatabaseConnector::setClearLog(const QString &log)
{
    d->m_ClearLog = log;
}

void DatabaseConnector::setClearPass(const QString &pass)
{
    d->m_ClearPass = pass;
}

void DatabaseConnector::setHost(const QString &host)
{
    d->m_HostName = host;
}

void DatabaseConnector::setPort(const int port)
{
    d->m_Port = port;
}

void DatabaseConnector::setAbsPathToReadOnlySqliteDatabase(const QString &absPath)
{
    d->m_AbsPathToReadOnlySQLiteDb = absPath;
}

void DatabaseConnector::setAbsPathToReadWriteSqliteDatabase(const QString &absPath)
{
    d->m_AbsPathToReadWriteSQLiteDb = absPath;
}

void DatabaseConnector::setAccessMode(const AccessMode mode)
{
    d->m_AccessMode = mode;
}

/**
 * When using DatabaseConnector with a SQLite db, you can force the usage
 * of the exact file defined by absPathToReadWriteSqliteDatabase() and
 * hostName (which is understood as fileName).
 */
void DatabaseConnector::setSqliteUsesExactFile(bool exactFile)
{
    d->m_UseExactFile = exactFile;
}

/**
 * You can set a global prefix to database name using \e prefix.
 * \sa Utils::Database::setDatabasePrefix()
 * \sa Utils::Database::prefixedDatabaseName()
 */
void DatabaseConnector::setGlobalDatabasePrefix(const QString &prefix)
{
    d->m_GlobalDatabasePrefix = prefix;
}

/** Clear the internal data. Resulting connector is invalid */
void DatabaseConnector::clear()
{
    d->m_ClearLog.clear();
    d->m_ClearPass.clear();
    d->m_Driver = Utils::Database::SQLite;
    d->m_DriverIsValid = d->testDriver(d->m_Driver);
    d->m_HostName.clear();
    d->m_Port = -1;
    d->m_AbsPathToReadOnlySQLiteDb.clear();
    d->m_AbsPathToReadWriteSQLiteDb.clear();
    d->m_AccessMode = ReadWrite;
    d->m_GlobalDatabasePrefix.clear();
}

/**
 * Check validity of the connector:
 * - a driver is correctly defined and available
 * - a login is defined
 * - for SQLite configuration: path to database is defined and exists
 * - for MySQL configuration: a host and a port are defined
 */
bool DatabaseConnector::isValid() const
{
    if (!d->m_DriverIsValid)
        return false;
    if (d->m_ClearLog.isEmpty())
        return false;

    if (d->m_Driver == Utils::Database::SQLite) {
        if (d->m_AbsPathToReadOnlySQLiteDb.isEmpty())
            return false;
        if (!QFileInfo(d->m_AbsPathToReadOnlySQLiteDb).exists())
            return false;
        if (!QFileInfo(d->m_AbsPathToReadWriteSQLiteDb).exists())
            return false;
        return true;
    }

    if (d->m_Driver == Utils::Database::MySQL) {
        if (d->m_HostName.isEmpty() || d->m_Port == -1) {
            return false;
        }
        return true;
    }

    return false;
}

Database::AvailableDrivers DatabaseConnector::driver() const
{
    return d->m_Driver;
}

QString DatabaseConnector::clearLog() const
{
    return d->m_ClearLog;
}

QString DatabaseConnector::clearPass() const
{
    return d->m_ClearPass;
}

QString DatabaseConnector::cryptedLog() const
{
    return loginForSQL(d->m_ClearLog);
}

/**
 * Return the crypted password (computed from the clear password)
 * \sa setClearPassword()
*/
QString DatabaseConnector::cryptedPass() const
{
    if (d->m_ClearPass.isEmpty())
        return QString::null;
    Utils::PasswordCrypter crypter;
    return crypter.cryptPassword(d->m_ClearPass);
}

QString DatabaseConnector::host() const
{
    return d->m_HostName;
}

int DatabaseConnector::port() const
{
    return d->m_Port;
}

QString DatabaseConnector::absPathToSqliteReadOnlyDatabase() const
{
    return d->m_AbsPathToReadOnlySQLiteDb;
}

QString DatabaseConnector::absPathToSqliteReadWriteDatabase() const
{
    return d->m_AbsPathToReadWriteSQLiteDb;
}

Utils::DatabaseConnector::AccessMode DatabaseConnector::accessMode() const
{
    return d->m_AccessMode;
}

bool DatabaseConnector::isDriverValid() const
{
    return d->m_DriverIsValid;
}

bool DatabaseConnector::useExactFile() const
{
    return d->m_UseExactFile;
}

/**
 * Returns the global database prefix
 * \sa setGlobalDatabasePrefix()
 */
QString DatabaseConnector::globalDatabasePrefix() const
{
    return d->m_GlobalDatabasePrefix;
}

/**
 * Deserialize a setting value.
 * Left to handle transition from old serialized to new database settings format
 */
void DatabaseConnector::fromSettings(const QString &value)
{
    clear();
    QString tmp;
    tmp = Utils::decrypt(value.toUtf8());
    QStringList vals = tmp.split(SEPARATOR);
    if (vals.count() < 5) {
        return;
    }
    d->m_ClearLog = vals[0];
    d->m_ClearPass = vals[1];
    d->m_HostName = vals[2];
    d->m_Port = vals[3].toInt();
    d->m_Driver = Utils::Database::AvailableDrivers(vals[4].toInt());
    d->m_DriverIsValid = d->testDriver(d->m_Driver);
    if (vals.count() > 5)
        d->m_GlobalDatabasePrefix = vals[5];
    if (vals.count() > 6)
        setAbsPathToReadWriteSqliteDatabase(vals[6]);
}

DatabaseConnector &DatabaseConnector::operator=(const DatabaseConnector &in)
{
    if (this == &in)
        return *this;
    d->m_ClearLog = in.d->m_ClearLog;
    d->m_ClearPass = in.d->m_ClearPass;
    d->m_Driver = in.d->m_Driver;
    d->m_DriverIsValid = in.d->m_DriverIsValid;
    d->m_UseExactFile = in.d->m_UseExactFile;
    d->m_HostName = in.d->m_HostName;
    d->m_Port = in.d->m_Port;
    d->m_AbsPathToReadOnlySQLiteDb = in.d->m_AbsPathToReadOnlySQLiteDb;
    d->m_AbsPathToReadWriteSQLiteDb = in.d->m_AbsPathToReadWriteSQLiteDb;
    d->m_AccessMode = in.d->m_AccessMode;
    d->m_GlobalDatabasePrefix = in.d->m_GlobalDatabasePrefix;
    return *this;
}

bool DatabaseConnector::operator==(const DatabaseConnector &other) const
{
    if (this==&other)
        return true;
    if (d->m_ClearLog==other.d->m_ClearLog &&
        d->m_ClearPass==other.d->m_ClearPass &&
        d->m_Driver==other.d->m_Driver &&
        d->m_DriverIsValid==other.d->m_DriverIsValid &&
        d->m_UseExactFile == other.d->m_UseExactFile &&
        d->m_HostName==other.d->m_HostName &&
        d->m_AbsPathToReadOnlySQLiteDb==other.d->m_AbsPathToReadOnlySQLiteDb &&
        d->m_AbsPathToReadWriteSQLiteDb==other.d->m_AbsPathToReadWriteSQLiteDb &&
        d->m_Port==other.d->m_Port &&
        d->m_GlobalDatabasePrefix==other.d->m_GlobalDatabasePrefix) {
        return true;
    }
    return false;
}

/** Debug purpose */
QString DatabaseConnector::toString() const
{
    QString dr;
    switch (driver()) {
    case Database::SQLite: dr="SQLite"; break;
    case Database::MySQL: dr="MySQL"; break;
    default: dr="NoDriver";
    }
    dr.append(QString("%1").arg(isDriverValid()?"(Ok)":"(**Invalid**)"));

    QString t = QString("DatabaseConnector(-%9-; "
                        "Log:%1; "
                        "Pass:%2; "
                        "%6:%3; "
                        "Port:%4; "
                        "Driver:%5; "
                        "Prefix: %7; "
                        "ExactFileName: %8")
            .arg(clearLog())
            .arg(clearPass().length())
            .arg(host())
            .arg(port())
            .arg(dr)
            .arg((driver()==Utils::Database::SQLite)?"FileName":"HostName")
            .arg(d->m_GlobalDatabasePrefix)
            .arg(d->m_UseExactFile?"true":"false")
            .arg((accessMode()==Utils::DatabaseConnector::ReadWrite)?"RW":"RO");

    if (driver()==Database::SQLite) {
        if (!absPathToSqliteReadOnlyDatabase().isEmpty())
            t += QString("\n                   RO:%1")
                    .arg(absPathToSqliteReadOnlyDatabase());
        if (!absPathToSqliteReadWriteDatabase().isEmpty())
        t += QString("\n                   RW:%1")
                .arg(absPathToSqliteReadWriteDatabase());
    }
    t += ")";
    return t;
}

QDebug operator<<(QDebug dbg, const Utils::DatabaseConnector &c)
{
    dbg.nospace() << c.toString();
    return dbg.space();
}
