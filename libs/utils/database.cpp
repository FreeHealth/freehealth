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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Utils::Database
  this class is a base class for databases. It manages scheme and creates SQL queries.
    The idea is to create a database scheme dynamically. Your superbase must hold the enums
    corresponding to the tables and the fields of each tables.\n
    Then Database is populated with real names of tables and fields.\n
    Then you can get what ever you want without any knowledge of the database tables, fields names and driver.\n

    It manages connections and propose a virtual member for the database creation if requiered.\n
    You can easily manage automatic scheme database creation with createTables().\n

    Usage :
    \code
    // Your superclass inherits of Database
    class MyDatabase : public Database
    // in the header public part put your enums (don't forget the '=0' next the first enumerator)
    enum Tables { Table_TABLE1=0, Table_TABLE2, Table_End };
    enum TABLE1FIELDS { TAB1_FIELD1=0, TAB1_FIELD2... };
    enum TABLE2FIELDS { TAB2_FIELD1=0, TAB2_FIELD2... };

    // In constructor populate tables' names and fields' names in the same order they appear in database/table.
    addTable( Table_TABLE1, "FIRST_TABLE");
    addField( Table_TABLE1, TAB1_FIELD1, "FIRST_FIELD_OF_TABLE1", FieldIsInteger, "NULL");
    addField( Table_TABLE1, TAB1_FIELD2, "SECOND_FIELD_OF_TABLE1", FieldIsInteger, "NULL");
    ...
    addPrimaryKey(Table_TABLE1, TAB1_FIELD1);

    addTable( Table_TABLE2, "SECOND_TABLE");
    addField( Table_TABLE2, TAB2_FIELD1, "FIRST_FIELD_OF_TABLE2", FieldIsInteger, "NULL");
    ...
    // In constructor call createConnection()
    createConnection( "users", "users.db", databasePath(), ReadWrite, SQLite);
    // That's done, now you can work with enums instead of magic strings...
    // If you want to create database if it doesn't exist define the member createDatabase() in your superclass.
    \endcode
*/

#include "database.h"
#include "log.h"
#include "global.h"
#include "databaseconnector.h"

#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_filepathxml.h>

#include <QDir>
#include <QSqlRecord>
#include <QSqlField>
#include <QCoreApplication>
#include <QHash>
#include <QMultiHash>
#include <QMap>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QProgressDialog>
#include <QUuid>

#include <QDebug>

enum { WarnSqlCommands = false, WarnCreateTableSqlCommand = false, WarnLogMessages = false };

using namespace Utils;
using namespace Utils::Internal;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {

struct DbIndex {
    Utils::Field field;
    QString name;
};

class DatabasePrivate
{
public:
    DatabasePrivate();
    ~DatabasePrivate() {}
    QStringList getSQLCreateTable(const int & tableref);
    QString getTypeOfField(const int & fieldref) const;

    static Database::Grants getGrants(const QString &connection, const QStringList &grants)
    {
        Q_UNUSED(connection);
        QHash<QString, int> ref;
        ref.insert("ALL PRIVILEGES", Database::Grant_All);
        ref.insert("ALTER", Database::Grant_Alter);
        ref.insert("ALTER ROUTINE", Database::Grant_AlterRoutine);
        ref.insert("CREATE", Database::Grant_Create);
        ref.insert("CREATE ROUTINE", Database::Grant_CreateRoutine);
        ref.insert("CREATE TEMPORARY TABLES", Database::Grant_CreateTmpTables);
        ref.insert("CREATE USER", Database::Grant_CreateUser);
        ref.insert("CREATE VIEW", Database::Grant_CreateView);
        ref.insert("DELETE", Database::Grant_Delete);
        ref.insert("DROP", Database::Grant_Drop);
        ref.insert("EXECUTE", Database::Grant_Execute);
        ref.insert("GRANT OPTION", Database::Grant_Options);
        ref.insert("INDEX", Database::Grant_Index);
        ref.insert("INSERT", Database::Grant_Insert);
        ref.insert("LOCK TABLES", Database::Grant_LockTables);
        ref.insert("PROCESS", Database::Grant_Process);
        ref.insert("SELECT", Database::Grant_Select);
        ref.insert("SHOW DATABASES", Database::Grant_ShowDatabases);
        ref.insert("SHOW VIEW", Database::Grant_ShowView);
        ref.insert("TRIGGER", Database::Grant_Trigger);
        ref.insert("UPDATE", Database::Grant_Update);
//        ref.insert("EVENT", Database::Grant_Event);
//        ref.insert("FILE", Database::Grant_File);
//        ref.insert("REFERENCES", Database::Grant_References);
//        ref.insert("RELOAD", Database::Grant_Reload);
//        ref.insert("USAGE", Database::Grant_Usage);

        // check grants from stringlist
        Database::Grants g = 0;
        foreach(const QString &s, grants) {
            foreach(const QString &k, ref.keys()) {
                if (s.contains(k + ",") || s.contains(k+" ON")) {
                    g |= Database::Grants(ref.value(k));
                    break;
                }
            }
        }
        return g;
    }

public:
    QHash<int, QString>        m_Tables;         // tables are not sorted
    QMultiHash<int, int>       m_Tables_Fields;  // links are not sorted
    QMap<int, QString>         m_Fields;         // fields should be sorted from first to last one using ref
    QHash<int, int>            m_TypeOfField;
    QHash<int, QString>        m_DefaultFieldValue;
    bool                       m_initialized;
    int                        m_LastCorrectLogin;
    QString                    m_ConnectionName;
    QHash<QString, Database::Grants> m_Grants;
    Database::AvailableDrivers m_Driver;
    QMultiHash<int,int> m_PrimKeys;
    QVector<DbIndex> m_DbIndexes;
};
}
}

void Database::logAvailableDrivers()
{
    QString tmp;
    foreach(const QString &drv, QSqlDatabase::drivers()) {
        if (QSqlDatabase::isDriverAvailable(drv)) {
            tmp += drv + " ; ";
        }
    }
    tmp.chop(3);
    LOG_FOR("Database", QString("Available drivers: %1").arg(QSqlDatabase::drivers().join(" ; ")));
}


Database::Database()
        : d(0)
{
    d = new DatabasePrivate();
}

Database::~Database()
{
    if (d) delete d; d=0;
}

DatabasePrivate::DatabasePrivate() :
        m_initialized(false),
        m_LastCorrectLogin(-1),
        m_Driver(Database::SQLite)
{
    m_ConnectionName = "";
}

//--------------------------------------------------------------------------------------------------------
//------------------------------- Managing Databases files and connections -------------------------------
//--------------------------------------------------------------------------------------------------------
QString Database::prefixedDatabaseName(AvailableDrivers driver, const QString &dbName) const
{
    if (driver==SQLite) {
        return dbName;
    }
    if (driver==MySQL || driver==PostSQL) {
        if (dbName.startsWith("fmf_"))
            return dbName;
        return "fmf_" + dbName;
    }
    return dbName;
}

/** Creates a MySQL database on an opened database. The current connection must be opened.*/
bool Database::createMySQLDatabase(const QString &dbName)
{
    if (!database().isOpen()) {
        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(database().connectionName()).arg(database().lastError().text()));
        return false;
    }
    // Testing current connected user grants
    Grants userGrants = d->m_Grants.value(d->m_ConnectionName, Grant_NoGrant);
    if (userGrants & Grant_Create) {
        LOG_ERROR_FOR("Database", "Trying to create database, no suffisant rights.");
        return false;
    }
    LOG_FOR("Database", QString("Trying to create database: %1\n"
                                "       on host: %2(%3)\n"
                                "       with user: %4")
            .arg(dbName).arg(database().hostName()).arg(database().port()).arg(database().userName()));

    QSqlQuery query(database());
    if (!query.exec(QString("CREATE DATABASE `%1`;").arg(dbName))) {
        LOG_QUERY_ERROR_FOR("Database", query);
        return false;
    }
    LOG_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    query.finish();
    return true;
}

/** Create a MySQL server user using the \e log and \e pass, with the specified \e grants on the \e userHost and the \e userDatabases. */
bool Database::createMySQLUser(const QString &log, const QString &password,
                               const Grants grants,
                               const QString &userHost, const QString &userDatabases)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(database().connectionName()).arg(database().lastError().text()));
            return false;
        }
    }

    // Testing current connected user grants
    Grants userGrants = d->m_Grants.value(d->m_ConnectionName, Grant_NoGrant);

    /** \todo bug with grant privileges of created user */
//    qWarning() << "xxxxxxxxxxxxxxxxxxxxxxx check";
//    qWarning() << grants << (grants & Grant_All);

    if (!(userGrants & Grant_CreateUser)) {
        LOG_ERROR_FOR("Database", "Trying to create user, no suffisant rights.");
        LOG_DATABASE_FOR("Database", database());
        return false;
    }
    // Creating grants string
    QString g;
//    if (grants & Grant_All) {
//        g = "ALL PRIVILEGES";
//    } else {
    // Database privileges
        if (grants & Grant_Select) {
            g += "SELECT, ";
        }
        if (grants & Grant_Update) {
            g += "UPDATE, ";
        }
        if (grants & Grant_Insert) {
            g += "INSERT, ";
        }
        if (grants & Grant_Delete) {
            g += "DELETE, ";
        }
        if (grants & Grant_Create) {
            g += "CREATE, ";
        }
        if (grants & Grant_Drop) {
            g += "DROP, ";
        }
        if (grants & Grant_Alter) {
            g += "ALTER, ";
        }
        g.chop(2);
//    }
    if (g.isEmpty()) {
        LOG_ERROR_FOR("Database","No grants when creating user");
        return false;
    }
    // Managing defaults
    QString uh = userHost;
    if (uh.isEmpty()) {
        uh = "%";
    }
    QString udb = userDatabases;
    if (udb.isEmpty()) {
        udb = "fmf\\_%";
    }
    LOG_FOR("Database", QString("Trying to create MySQL user: %1\n"
                                "       on host: %2(%3)\n"
                                "       with user: %4")
            .arg(log).arg(database().hostName()).arg(database().port()).arg(database().userName()));

    QSqlQuery query(database());
    QString req;
    req = QString("CREATE USER '%1'@'%2' IDENTIFIED BY '%3';").arg(log).arg(uh).arg(password);
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        return false;
    }
    query.finish();
    // If grants fail -> remove user and return false
//    req = QString("GRANT %1 ON `%2`.* TO '%3'@'%' IDENTIFIED BY '%4';").arg(g).arg(udb).arg(log).arg(password);
    req = QString("GRANT %1, GRANT OPTION ON `%2`.* TO '%3'@'%';").arg(g).arg(udb).arg(log);
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        query.finish();
        req = QString("DROP USER '%1'@'%2'").arg(log).arg(uh);
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR("Database", query);
            LOG_DATABASE_FOR("Database", database());
        } else {
            LOG_ERROR_FOR("Database", QString("User %1 removed").arg(log));
        }
        return false;
    }
    query.finish();

    if (grants & Grant_CreateUser) {
        req = QString("GRANT CREATE USER, GRANT OPTION ON *.* TO '%1'@'%';").arg(log);
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR("Database", query);
            LOG_DATABASE_FOR("Database", database());
            query.finish();
            req = QString("DROP USER '%1'@'%2'").arg(log).arg(uh);
            if (!query.exec(req)) {
                LOG_QUERY_ERROR_FOR("Database", query);
                LOG_DATABASE_FOR("Database", database());
            } else {
                LOG_ERROR_FOR("Database", QString("User %1 removed").arg(log));
            }
            return false;
        }
        query.finish();
    }

    LOG_FOR("Database", tkTr(Trans::Constants::DATABASE_USER_1_CORRECTLY_CREATED).arg(log));
    return true;
}

/** Drop a MySQL user identified by his \e log and the \e userHostName. */
bool Database::dropMySQLUser(const QString &log, const QString &userHost)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(database().connectionName()).arg(database().lastError().text()));
            return false;
        }
    }

    // Testing current connected user grants
    Grants userGrants = d->m_Grants.value(d->m_ConnectionName, Grant_NoGrant);

//    qWarning() << "xxxxxxxxxxxxxxxxxxxxxxx check";
//    qWarning() << grants << (grants & Grant_All);

    if (!(userGrants & Grant_CreateUser)) {
        LOG_ERROR_FOR("Database", "Trying to create user, no suffisant rights.");
        return false;
    }
    LOG_FOR("Database", QString("Trying to drop MySQL user: %1\n"
                                "       on host: %2(%3)\n"
                                "       with user: %4")
            .arg(log).arg(database().hostName()).arg(database().port()).arg(database().userName()));

    QString req;
    if (userHost.isEmpty()) {
        req = QString("DROP USER '%1';").arg(log);
    } else {
        req = QString("DROP USER '%1'@'%2';").arg(log).arg(userHost);
    }
    QSqlQuery query(database());
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        return false;
    } else {
        LOG_FOR("Database", QString("User %1 removed").arg(log));
    }
    return true;
}

/** Change the password to \e newPassword for a MySQL user identified by his \e login. */
bool Database::changeMySQLUserPassword(const QString &login, const QString &newPassword)
{
    if (login.isEmpty())
        return false;

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(database().connectionName()).arg(database().lastError().text()));
            return false;
        }
    }

    /** \todo manage user grants */
//    // Testing current connected user grants
//    Grants userGrants = d->m_Grants.value(d->m_ConnectionName, Grant_NoGrant);

//    if (!(userGrants & Grant_CreateUser)) {
//        LOG_ERROR_FOR("Database", "Trying to create user, no suffisant rights.");
//        return false;
//    }
    LOG_FOR("Database", QString("Trying to change MySQL user password:\n"
                                "       user: %1\n"
                                "       host: %2(%3)\n"
                                "       new password: %4")
            .arg(login).arg(database().hostName()).arg(database().port()).arg(newPassword));

    QString req;
    req = QString("UPDATE `mysql`.`user` SET `Password` = PASSWORD('%1') WHERE `User` = '%2';")
            .arg(newPassword).arg(login);
    QSqlQuery query(database());
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        return false;
    }
    query.finish();
    if (!query.exec("FLUSH PRIVILEGES;")) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        return false;
    }
    LOG_FOR("Database", QString("User %1 password modified").arg(login));
    return true;
}

/** Return the pointer to the QSqlDatabase in use. */
QSqlDatabase Database::database() const
{ return QSqlDatabase::database(d->m_ConnectionName); }


/**
   Create the connection to the database.
  If database does not exist, according to the \e createOption, createDatabase() is called.
  An error is returned if :
  - Driver is not available
  - Can not connect to server (wrong host/log/pass)
  - Can not create database is it doesn't exists and user asked to create it
  - Can not read database if asked to be readable
  - Can not write in database if asked to be writable
  \param connectionName = name of the connect
  \param connector = Utils::DatabaseConnector = connection params
  \param createOption = what to do if the database does not exist.
*/
bool Database::createConnection(const QString &connectionName, const QString &nonPrefixedDbName,
                                const Utils::DatabaseConnector &connector,
                                CreationOption createOption
                                )
{
    bool toReturn = true;
    d->m_ConnectionName.clear();
    d->m_Driver = connector.driver();
    QString dbName = prefixedDatabaseName(d->m_Driver, nonPrefixedDbName);

    if (WarnLogMessages) {
        LOG_FOR("Database", connectionName + "  //  " + dbName);
        qWarning() << connector;
    }

    // does driver is available
    if (!connector.isDriverValid()) {
        LOG_ERROR_FOR("Database", "Driver is not valid");
        return false;
    }

    // does connection already exists ?
    if (QSqlDatabase::contains(connectionName)) {
        if (WarnLogMessages)
            LOG_FOR("Database", QCoreApplication::translate("Database",
                                                            "WARNING: %1 database already in use")
                    .arg(connectionName));
        d->m_ConnectionName = connectionName;
        return true;
    }

    QSqlDatabase DB;

    // Construct SQLite database fileName
    QString fileName;
    if (connector.accessMode()==DatabaseConnector::ReadOnly) {
        fileName = QDir::cleanPath(connector.absPathToSqliteReadOnlyDatabase() + QDir::separator() + connectionName + QDir::separator() + dbName);
    } else if (connector.accessMode()==DatabaseConnector::ReadWrite) {
        fileName = QDir::cleanPath(connector.absPathToSqliteReadWriteDatabase() + QDir::separator() + connectionName + QDir::separator() + dbName);
    }
    if (!fileName.endsWith(".db")) {
        fileName += ".db";
    }
    QFileInfo sqliteFileInfo(fileName);

    // if DeleteAndRecreateDatabase only for read/write databases
    if (createOption == DeleteAndRecreateDatabase && connector.accessMode()==DatabaseConnector::ReadWrite) {
        LOG_FOR("Database", "Delete database before re-creating it. Connection: " + connectionName + "; PathOrHost: " + dbName);
        switch (connector.driver()) {
        case SQLite:
        {
            if (sqliteFileInfo.exists()) {
                QFile f(sqliteFileInfo.absoluteFilePath());
                if (!f.rename(sqliteFileInfo.absolutePath() + QDir::separator() + sqliteFileInfo.baseName() + "-bkup" + QDateTime::currentDateTime().toString("yyyyMMddhhMMss") + "." + sqliteFileInfo.completeSuffix()))
                    LOG_ERROR_FOR("Database", "Unable to rename file.");
            }
            break;
        }
        case MySQL:
        {
            DB = QSqlDatabase::addDatabase("QMYSQL" , "__DB_DELETOR" + connectionName);
            DB.setHostName(connector.host());
            DB.setUserName(connector.clearLog());
            DB.setPassword(connector.clearPass());
            DB.setPort(connector.port());
//            DB.setDatabaseName("mysql");
            if (!DB.open()) {
                LOG_ERROR_FOR("Database", QString("Unable to connect to the server %1 - %2")
                                     .arg(connector.host()).arg(DB.lastError().text()));
                return false;
            }
            QSqlQuery query(DB);
            if (!query.exec("DROP DATABASE " + dbName)) {
                LOG_QUERY_ERROR_FOR("Database", query);
                LOG_ERROR_FOR("Database", "Unable to drop database");
            }
            if (WarnLogMessages)
                LOG_FOR("Database", QString("Connected to host %1").arg(connector.host()));
            break;
        }
        case PostSQL: break;
        }
        createOption = CreateDatabase;
    }
    if (QSqlDatabase::contains("__DB_DELETOR" + connectionName)) {
        QSqlDatabase::removeDatabase("__DB_DELETOR" + connectionName);
    }

    // check server connection
    switch (connector.driver()) {
    case SQLite: break;
    case MySQL:
        {
            DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
            DB.setHostName(connector.host());
            DB.setUserName(connector.clearLog());
            DB.setPassword(connector.clearPass());
            DB.setPort(connector.port());
            if (!DB.open()) {
                LOG_ERROR_FOR("Database", QString("Unable to connect to the server %1 - %2")
                                     .arg(connector.host()).arg(DB.lastError().text()));
                return false;
            }
            if (WarnLogMessages)
                LOG_FOR("Database", QString("Connected to host %1").arg(connector.host()));
            break;
        }
    case PostSQL:
        {
            /** \todo Check PostSQL connection */
            break;
        }
    }

    // create database is not exists and user ask for database creation
    switch (connector.driver()) {
    case SQLite:
        {
            if ((!sqliteFileInfo.exists()) || (sqliteFileInfo.size() == 0)) {
                if (createOption == CreateDatabase) {
                    if (!createDatabase(connectionName, sqliteFileInfo.fileName(), connector, createOption)) {
                        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName + "@" + fileName).arg(""));
                        return false;
                    }
                } else { // Warn Only
                    LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName + "@" + fileName).arg(""));
                    return false;
                }
            }
            // now that the file was created, reload the fileinfo
            sqliteFileInfo.setFile(sqliteFileInfo.absoluteFilePath());
            break;
        }
    case MySQL:
        {
            // can get connection to server ?
            DB.setDatabaseName(dbName);
            if (!DB.open()) {
                LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(dbName).arg(DB.lastError().text()));
                if (createOption == CreateDatabase) {
                    if (!createDatabase(connectionName, dbName, connector, createOption)) {
                        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName + "@" + connector.host()).arg(""));
                        return false;
                    }
                } else { // Warn Only
                    if (WarnLogMessages)
                        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName + "@" + connector.host()).arg(""));
                    return false;
                }
            }
            if (WarnLogMessages)
                LOG_FOR("Database", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(dbName).arg(DB.driverName()));
            break;
        }
    case PostSQL:
        {
            /** \todo Test database existence */
            break;
        }
    }

    // test read access to database
    switch (connector.driver()) {
    case SQLite:
        {
            if (!sqliteFileInfo.isReadable()) {
                LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "ERROR: Database `%1` is not readable. Path: %2")
                              .arg(dbName, sqliteFileInfo.absoluteFilePath()));
                toReturn = false;
            }
            break;
        }
    case MySQL:
        {
            if (!DB.isOpen()) {
                if (!DB.open()) {
                    LOG_ERROR_FOR("Database", QCoreApplication::translate("Database",
                                                                          "ERROR: Database %1 is not readable. Host: %2")
                                  .arg(dbName, connector.host()));
                    return false;
                }
            }
            QSqlQuery query("SHOW GRANTS FOR CURRENT_USER;", DB);
            if (!query.isActive()) {
                LOG_ERROR_FOR("Database", QCoreApplication::translate("Database",
                                                                      "ERROR: Database %1 is not readable. Path: %2")
                              .arg(dbName, connector.host()));
                LOG_QUERY_ERROR_FOR("Database", query);
                return false;
            } else {
                QStringList grants;
                while (query.next()) {
                    grants << query.value(0).toString();
                }
                d->m_Grants.insert(connectionName, d->getGrants(connectionName, grants));
            }
            break;
        }
    case PostSQL:
        {
            /** \todo Test database connection PostSQL */
            break;
        }
    }

    // test write access
    if (connector.accessMode() == DatabaseConnector::ReadWrite) {
        switch (connector.driver()) {
        case SQLite:
            {
                if (!sqliteFileInfo.isWritable()) {
                    LOG_ERROR_FOR("Database", QCoreApplication::translate("Database",
                                  "ERROR: Database %1 is not writable. Path: %2.")
                                  .arg(dbName, fileName));
                    toReturn = false;
                }
                break;
            }
        case MySQL:
            {
                /** \todo test write access to MySQL database */
                break;
            }
        case PostSQL:
            {
                /** \todo test write access to PostGreSQL database */
                break;
            }
        }
    }

    // create connection
    switch (connector.driver())
    {
        case SQLite :
        {
             DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
             DB.setDatabaseName(sqliteFileInfo.absoluteFilePath());
             break;
         }
        case MySQL :
        {
             break;
         }
        case PostSQL :
             /** \todo : manage PostGre SQL */
             return false;
             break;
    }

    DB.open();
    if (WarnLogMessages)
        LOG_FOR("Database", QCoreApplication::translate("Database",  "INFO: database %1 connection = %2")
                       .arg(dbName).arg(DB.isOpen()));

    // test connection
    if (!DB.isOpen()) {
        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(dbName, DB.lastError().text()));
        toReturn = false;
    } else {
        if (WarnLogMessages)
            LOG_FOR("Database", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(dbName).arg(DB.driverName()));
    }

    // return boolean
    if (toReturn)
        d->m_ConnectionName = connectionName;

    return toReturn;
}



bool Database::createDatabase(const QString &connectionName , const QString &prefixedDbName,
                            const Utils::DatabaseConnector &connector,
                            CreationOption createOption
                           )
{
    if (connector.driver()==SQLite) {
        return createDatabase(connectionName, prefixedDbName,
                              connector.absPathToSqliteReadWriteDatabase() + QDir::separator() + connectionName + QDir::separator(),
                              Database::TypeOfAccess(connector.accessMode()),
                              connector.driver(),
                              connector.clearLog(), connector.clearPass(),
                              connector.port(),
                              createOption);
    } else {
        return createDatabase(connectionName, prefixedDbName,
                              connector.host(),
                              Database::TypeOfAccess(connector.accessMode()),
                              connector.driver(),
                              connector.clearLog(), connector.clearPass(),
                              connector.port(),
                              createOption);
    }
}


/**  Returns the connectionName in use */
QString Database::connectionName() const
{
    return d->m_ConnectionName;
}

/** Create an UUID compatible with the Utils::Database manager. This code uses QUuid::createUuid().toString() with some string contraction. */
QString Database::createUid()
{
    return QUuid::createUuid().toString().remove("-").remove("{").remove("}");
}

/**  returns the grants according to the database \e connectionName. When using a SQLite driver Grants always == 0. */
Database::Grants Database::grants(const QString &connectionName) const
{
    return d->m_Grants.value(connectionName, 0);
}

/**
   Returns the grants according to the database \e connectionName.
   Database must be open and connected with a specific user.
*/
Database::Grants Database::getConnectionGrants(const QString &connectionName) // static
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(connectionName).arg(DB.lastError().text()));
            return Database::Grant_NoGrant;
        }
    }
    if (DB.driverName()=="QSQLITE") {
        return Grant_All;
    }
    if (DB.driverName()=="QMYSQL") {
        QStringList grants;
        QSqlQuery query("SHOW GRANTS FOR CURRENT_USER;", DB);
        if (!query.isActive()) {
            LOG_ERROR_FOR("Database", "No grants for user on database?");
            LOG_QUERY_ERROR_FOR("Database", query);
            return Grant_NoGrant;
        } else {
            while (query.next()) {
                grants << query.value(0).toString();
            }
        }
        query.finish();
        return DatabasePrivate::getGrants(connectionName, grants);
    }
    /** \todo code here : PostGreSQL and other drivers */
    return Grant_NoGrant;
}


/**  Set connectionName to \e c */
void Database::setConnectionName(const QString & c)
{ d->m_ConnectionName = c; }

/**  Define the driver to use */
void Database::setDriver(const Database::AvailableDrivers drv)
{ d->m_Driver = drv; }

Database::AvailableDrivers Database::driver() const
{ return d->m_Driver; }

/**  Add a table \e name to the database scheme with the index \e ref */
int Database::addTable(const int & ref, const QString & name)
{
    d->m_Tables.insert(ref, name);
    return d->m_Tables.key(name);
}

/**  Add a field \e name to the database scheme with the index \e fieldref into table indexed \e tableref.\n
    The field is a type \e type and get the default value \e defaultValue.\n
    Please take care that \e name can not exceed 50 chars.
    \sa createTables(), createTable()
*/
int Database::addField(const int & tableref, const int & fieldref, const QString & name, TypeOfField type, const QString & defaultValue)
{
    Q_ASSERT_X(name.length() < 50, "Database", "Name of field can not exceed 50 chars");
    int ref = fieldref + (tableref * 1000);
    d->m_Tables_Fields.insertMulti(tableref, ref);
    d->m_Fields.insert(ref , name);
    d->m_TypeOfField.insert(ref , type);
    d->m_DefaultFieldValue.insert(ref, defaultValue);
    return d->m_Fields.key(name) - (tableref * 1000);
}

/**  Add a primary key reference to \e tableref \e fieldref.
    \sa createTables(), createTable(), addField(), addTable()
*/
void Database::addPrimaryKey(const int &tableref, const int &fieldref)
{
    d->m_PrimKeys.insertMulti(tableref, fieldref);
}

/** Create an index on the specified \e tableRef, \e fieldRef, named \e name. If \e name is not specified a unique name is created. */
void Database::addIndex(const int &tableref, const int &fieldref, const QString &name)
{
    Utils::Field f = this->field(tableref, fieldref);
    addIndex(f, name);
}

/** Create an index on the specified \e field, named \e name. If \e name is not specified a unique name is created. */
void Database::addIndex(const Utils::Field &field, const QString &name)
{
    Internal::DbIndex index;
    // Get the correct field with field and table names
    index.field = this->field(field.table, field.field);
    // Recreate index' name
    if (name.isEmpty()) {
        index.name = index.field.tableName + "__" + index.field.fieldName;
    } else {
        index.name = name;
    }
    // Store index
    d->m_DbIndexes.append(index);
}

/**  Verify that the dynamically scheme passed is corresponding to the real database scheme. */
bool Database::checkDatabaseScheme()
{
    if (d->m_ConnectionName.isEmpty())
        return false;
    if (d->m_Tables.keys().count() == 0)
        return false;
    if (d->m_Tables_Fields.keys().count() == 0)
        return false;

    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(d->m_ConnectionName).arg(DB.lastError().text()));
            return false;
        }
    }

    QList<int> list = d->m_Tables.keys();
    qSort(list);
    foreach(int i, list) {
        QSqlRecord rec = DB.record(d->m_Tables.value(i));
        if (rec.count() != d->m_Tables_Fields.values(i).count()) {
            LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "Database Scheme Error: wrong number of fields for table %1")
                                   .arg(d->m_Tables.value(i)));
            return false;
        }
        QList<int> fields = d->m_Tables_Fields.values(i);
        qSort(fields);
        int id = 0;
        foreach(int f, fields) {
            if (d->m_Fields.value(f)!= rec.field(id).name()) {
                LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "Database Scheme Error: field number %1 differs: %2 instead of %3 in table %4")
                                   .arg(id).arg(d->m_Fields.value(f), rec.field(id).name(), d->m_Tables.value(i)));
                return false;
            }
            id++;
        }
    }
    return true;
}

/**
    Return the field name for the table \e tableref field \e fieldref
   \sa addField()
*/
QString Database::fieldName(const int &tableref, const int &fieldref) const
{
    if (!d->m_Tables.contains(tableref))
        return QString::null;
    if (!d->m_Tables_Fields.keys().contains(tableref))
        return QString::null;
    if (!d->m_Fields.keys().contains(fieldref + (tableref * 1000)))
        return QString::null;

    return d->m_Fields.value(fieldref + (tableref * 1000));
}

/**
    Return a complete reference to a field for the table \e tableref field \e fieldref
   \sa addField()
*/
Field Database::field(const int &tableref, const int &fieldref) const
{
    Field ret;
    ret.table = tableref;
    ret.field = fieldref;
    ret.tableName = table(tableref);
    ret.fieldName = fieldName(tableref, fieldref);
    return ret;
}

/**  Return all fields of a table as \e FieldList */
FieldList Database::fields(const int tableref) const
{
    FieldList fields;
    for(int i = 0; i < (d->m_Tables_Fields.values(tableref).count()); ++i) {
        fields << field(tableref, i);
    }
    return fields;
}

/**  Return all fields name of a table */
QStringList Database::fieldNames(const int &tableref) const
{
    if (!d->m_Tables.contains(tableref))
        return QStringList();
    if (!d->m_Tables_Fields.keys().contains(tableref))
        return QStringList();

    QList<int> list = d->m_Tables_Fields.values(tableref);
    qSort(list);
    QStringList toReturn;
    foreach(int i, list)
       toReturn << d->m_Fields.value(i);
    return toReturn;
}

QString Database::table(const int & tableref) const
{
    return d->m_Tables.value(tableref, QString());
}

QStringList Database::tables() const
{
    return d->m_Tables.values();
}

/**
   Create a where clause on the table \e tableref using conditions mapped into a hash.
  Conditions: key = fieldReference , value = whereClauseString.
  \code
    QHash<int,QString> where;
    where.insert(myFieldId, QString("='%1').arg(myStringMatch);
    QString clause = db.getWhereClause(myTableId, where);
    // Will return: "WHERE (TABLE1.FIELD_ID='StringToMatch')"
  \endcode
*/
QString Database::getWhereClause(const int &tableref, const QHash<int, QString> &conditions) const
{
    // here we create a where condition
    QString where = "";
    QHashIterator<int, QString> i(conditions);
    while (i.hasNext()) {
        i.next();
        if (!d->m_Fields.keys().contains(i.key() + (tableref * 1000)))
            continue;
        where.append(QString(" (`%1`.`%2` %3) AND ")
                      .arg(d->m_Tables[tableref])
                      .arg(d->m_Fields.value(i.key() + (tableref * 1000)), i.value()));
    }
    where.chop(5);
    if (conditions.count() > 1)
        where = QString("(%1)").arg(where);
    if (WarnSqlCommands)
        qWarning() << where;
    return where;
}

/**
   Create a where clause on the \e fields.
  \code
    FieldList fields;
    fields << field(1, 2, QString("='%1'").arg(myStringMatch));
    QString clause = db.getWhereClause(fields);
    // Will return: "WHERE (TABLE1.FIELD2='StringToMatch')"
  \endcode
*/
QString Database::getWhereClause(const FieldList &fields) const
{
    QString where = "";
    for(int i = 0; i < fields.count(); ++i) {
        QString tab, f;
        if (fields.at(i).tableName.isEmpty()) {
            tab = table(fields.at(i).table);
            f = fieldName(fields.at(i).table, fields.at(i).field);
        } else {
            tab = fields.at(i).tableName;
            f = fields.at(i).fieldName;
        }

        if (fields.at(i).orCondition) {
            where.chop(4);
            where += "OR ";
        }
        where += QString(" (`%1`.`%2` %3) AND ")
                 .arg(tab)
                 .arg(f)
                 .arg(fields.at(i).whereCondition);
    }
    where.chop(5);
    if (fields.count() > 1)
        where = QString("(%1)").arg(where);
    if (WarnSqlCommands)
        qWarning() << where;
    return where;
}

/**
   Create a join statement on \e join.field1.tableName using fields equality.
  \code
    Join join(t1, f1, t2, f2, joinType);
    QString sqlJoin = db.joinToSql(join);
    // will return: JOIN T1NAME ON T1NAME.F1NAME=T2NAME.F2NAME
  \endcode
*/
QString Database::joinToSql(const Join &join) const
{
    QString s;
    switch (join.type) {
    case SimpleJoin: s = "JOIN "; break;
    case OuterJoin: s = "OUTER JOIN "; break;
    case LeftJoin: s = "LEFT JOIN "; break;
    case InnerJoin: s = "INNER JOIN "; break;
    case NaturalJoin: s = "NATURAL JOIN "; break;
    case CrossJoin: s = "CROSS JOIN "; break;
    }
    if (s.isEmpty())
        return s;
    s += "`" + join.field1.tableName + "` ON " ;
    s += QString("`%1`.`%2`=`%3`.`%4` ")
         .arg(join.field1.tableName, join.field1.fieldName)
         .arg(join.field2.tableName, join.field2.fieldName);
    return s;
}

/**
   Return a SELECT SQL command with the table \e tableref, field \e fieldref and where conditions \e conditions.
  \code
  // output is like:
  SELECT FIELD1 FROM TABLE1 WHERE (...);
  // output is adapted to the driver
  \endcode
  \sa getWhereClause(), setDriver()
*/
QString Database::select(const int &tableref, const int &fieldref, const QHash<int, QString> &conditions) const
{
    QString toReturn;
    toReturn = QString("SELECT `%2`.`%1` FROM `%2` WHERE %3")
            .arg(fieldName(tableref, fieldref))
            .arg(table(tableref))
            .arg(getWhereClause(tableref, conditions));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**
   Return a SELECT SQL command with the table \e tableref, field \e fieldref.
  \code
  // output is like:
  SELECT FIELD1 FROM TABLE1;
  // output is adapted to the driver
  \endcode
  \sa setDriver()
*/
QString Database::select(const int &tableref, const int &fieldref) const
{
    QString toReturn;
    toReturn = QString("SELECT `%2`.`%1` FROM `%2`")
            .arg(fieldName(tableref, fieldref))
            .arg(table(tableref));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**
   Return a SELECT SQL command.
*/
QString Database::select(const int &tableref, const QList<int> &fieldsref, const QHash<int, QString> & conditions) const
{
    QString toReturn;
    QString tmp;
    foreach(const int & i, fieldsref)
        tmp += "`" + table(tableref) + "`.`" + fieldName(tableref, i)+ "`, ";
    if (tmp.isEmpty())
        return QString::null;
    tmp.chop(2);
    toReturn = QString("SELECT %1 FROM `%2` WHERE %3")
            .arg(tmp)
            .arg(table(tableref))
            .arg(getWhereClause(tableref, conditions));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**
   Return a SELECT SQL command.
*/
QString Database::select(const int & tableref,const  QList<int> &fieldsref) const
{
    QString toReturn;
    QString tmp;
    foreach(const int &i, fieldsref)
        tmp += "`" + table(tableref) + "`.`" + fieldName(tableref, i)+ "`, ";
    if (tmp.isEmpty())
        return QString::null;
    tmp.chop(2);
    toReturn = QString("SELECT %1 FROM `%2`")
            .arg(tmp)
            .arg(table(tableref));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**
   Return a SELECT SQL command for the table \e tableref with conditions \e conditions.
  The fields are ordered.
  \sa addField()
*/
QString Database::select(const int &tableref, const QHash<int, QString> &conditions) const
{
    QString toReturn;
    QString tmp;
    QList<int> list = d->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int &i, list)
        tmp += "`" + table(tableref) + "`.`" + d->m_Fields.value(i) + "`, ";
//        tmp += "`" + d->m_Fields.value(i)+ "`, ";
    if (tmp.isEmpty())
        return QString::null;
    tmp.chop(2);
    toReturn = QString("SELECT %1 FROM `%2` WHERE %3")
            .arg(tmp)
            .arg(table(tableref))
            .arg(getWhereClause(tableref, conditions));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**
   Return a SELECT SQL command for the table \e tableref.
  The fields are ordered.
  \sa addField()
*/
QString Database::select(const int &tableref) const
{
    QString toReturn;
    QString tmp;
    QList<int> list = d->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int & i, list)
        tmp += "`" + table(tableref) + "`.`" + d->m_Fields.value(i) + "`, ";
    if (tmp.isEmpty())
        return QString::null;
    tmp.chop(2);
    toReturn = QString("SELECT %1 FROM `%2`")
            .arg(tmp)
            .arg(table(tableref));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**
   Return a SELECT DISTINCT SQL command.
  \sa select()
*/
QString Database::selectDistinct(const int &tableref, const QList<int> &fields, const QHash<int, QString> &conditions) const
{
    return select(tableref, fields, conditions).replace("SELECT", "SELECT DISTINCT");
}

/**
   Return a SELECT DISTINCT SQL command.
  \sa select()
*/
QString Database::selectDistinct(const int &tableref, const int &fieldref, const QHash<int, QString> &conditions) const
{
    return select(tableref, fieldref, conditions).replace("SELECT", "SELECT DISTINCT");
}

/**
   Return a SELECT DISTINCT SQL command.
  \sa select()
*/
QString Database::selectDistinct(const int & tableref, const int & fieldref) const
{
    return select(tableref, fieldref).replace("SELECT", "SELECT DISTINCT");
}

QString Database::select(const FieldList &select, const JoinList &joins) const
{
    FieldList get;
    JoinList jns;
    QString fields, from;
    QStringList tables;
    // check fields and joins on fieldName and tableName
    for(int i=0; i < select.count(); ++i) {
        get << select.at(i);
        if (get.at(i).tableName.isEmpty() || get.at(i).fieldName.isEmpty()) {
            get[i].tableName = table(select.at(i).table);
            get[i].fieldName = fieldName(select.at(i).table, select.at(i).field);
        }
    }
    for(int i=0; i < joins.count(); ++i) {
        Field f1 = field(joins.at(i).field1.table, joins.at(i).field1.field);
        Field f2 = field(joins.at(i).field2.table, joins.at(i).field2.field);
        jns << Join(f1, f2);
    }

    // calculate fields
    for(int i=0; i < get.count(); ++i) {
        fields += QString("`%1`.`%2`, ").arg(get.at(i).tableName).arg(get.at(i).fieldName);
        tables << get.at(i).tableName;
    }
    tables.removeDuplicates();

    if (fields.isEmpty())
        return QString();
    fields.chop(2);

    // Calculate joins
    QString j;
    for(int i=0; i < jns.count(); ++i) {
        j += joinToSql(jns.at(i)) + "\n";
        tables.removeAll(jns.at(i).field1.tableName);
    }
    tables.removeDuplicates();

    // Add tables
    foreach(const QString &tab, tables) {
        from += QString("`%1`, ").arg(tab);
    }
    from.chop(2);

    return QString("SELECT %1 FROM %2 \n %3").arg(fields, from, j);
}

/**
   Create a complex SELECT command with jointures and conditions.
  Jointures must be ordered as needed in the SQL command.
*/
QString Database::select(const FieldList &select, const JoinList &joins, const FieldList &conditions) const
{
    FieldList get, cond;
    JoinList jns;
    QString fields, from;
    QStringList tables;
    // check fields and joins on fieldName and tableName
    for(int i=0; i < select.count(); ++i) {
        get << select.at(i);
        if (get.at(i).tableName.isEmpty() || get.at(i).fieldName.isEmpty()) {
            get[i].tableName = table(select.at(i).table);
            get[i].fieldName = fieldName(select.at(i).table, select.at(i).field);
        }
    }
    for(int i=0; i < conditions.count(); ++i) {
        cond << conditions.at(i);
        if (cond.at(i).tableName.isEmpty() || cond.at(i).fieldName.isEmpty()) {
            cond[i].tableName = table(cond.at(i).table);
            cond[i].fieldName = fieldName(cond.at(i).table, cond.at(i).field);
        }
    }
    for(int i=0; i < joins.count(); ++i) {
        Field f1 = field(joins.at(i).field1.table, joins.at(i).field1.field);
        Field f2 = field(joins.at(i).field2.table, joins.at(i).field2.field);
        jns << Join(f1, f2);
    }

    // calculate fields
    for(int i=0; i < get.count(); ++i) {
        fields += QString("`%1`.`%2`, ").arg(get.at(i).tableName).arg(get.at(i).fieldName);
        tables << get.at(i).tableName;
    }
    tables.removeDuplicates();

    if (fields.isEmpty())
        return QString();
    fields.chop(2);

    // Calculate conditions
    QString w = getWhereClause(cond);
    for(int i=0; i < cond.count(); ++i) {
        tables << cond.at(i).tableName;
    }
    tables.removeDuplicates();

    // Calculate joins
    QString j;
    for(int i=0; i < jns.count(); ++i) {
        j += joinToSql(jns.at(i)) + "\n";
        tables.removeAll(jns.at(i).field1.tableName);
    }
    tables.removeDuplicates();

    // Add tables
    foreach(const QString &tab, tables) {
        from += QString("`%1`, ").arg(tab);
    }
    from.chop(2);

    return QString("SELECT %1 FROM %2 \n %3 WHERE %4").arg(fields, from, j, w);
}

QString Database::select(const FieldList &select, const JoinList &joins, const Field &condition) const
{
    FieldList cond;
    cond << condition;
    return this->select(select, joins, cond);
}

/**
   Create a complex SELECT command with jointures and conditions.
  Jointures must be ordered as needed in the SQL command.
*/
QString Database::select(const int tableref, const JoinList &joins, const FieldList &conditions) const
{
    FieldList fields;
    for(int i = 0; i < (d->m_Tables_Fields.values(tableref).count()); ++i) {
        fields << Field(tableref, i);
    }
    return this->select(fields, joins, conditions);
}

QString Database::select(const int tableref, const Join &join, const Field &condition) const
{
    JoinList joins;
    joins << join;
    FieldList cond;
    cond << condition;
    return this->select(tableref, joins, cond);
}

QString Database::select(const int tableref, const Join &join, const FieldList &conditions) const
{
    JoinList joins;
    joins << join;
    return this->select(tableref, joins, conditions);
}

QString Database::select(const Field &select, const JoinList &joins, const Field &conditions) const
{
    FieldList get;
    get << select;
    FieldList cond;
    cond << conditions;
    return this->select(get, joins, cond);
}

QString Database::select(const FieldList &select, const Join &join, const Field &condition) const
{
    JoinList joins;
    joins << join;
    FieldList cond;
    cond << condition;
    return this->select(select, joins, cond);
}

QString Database::select(const Field &select, const Join &join, const FieldList &conditions) const
{
    JoinList joins;
    joins << join;
    FieldList get;
    get << select;
    return this->select(get, joins, conditions);
}

QString Database::select(const Field &select, const Join &join, const Field &condition) const
{
    JoinList joins;
    joins << join;
    FieldList get;
    get << select;
    FieldList conds;
    conds << condition;
    return this->select(get, joins, conds);
}

/**
   Return a SELECT SQL structured field equality.
  \code
  // output is like:
  TABLE1.FIELD1=TABLE2.FIELD2
  // output is adapted to the driver
  \endcode
*/
QString Database::fieldEquality(const int tableRef1, const int fieldRef1, const int tableRef2, const int fieldRef2) const
{
    return QString("`%1`.`%2`=`%3`.`%4`")
            .arg(table(tableRef1), fieldName(tableRef1, fieldRef1))
            .arg(table(tableRef2), fieldName(tableRef2, fieldRef2));
}

/**
   Return a a COUNT SQL command on the table \e tableref, field \e fieldref with the filter \e filter.
  Filter whould be not contains the "WHERE" word.
*/
int Database::count(const int & tableref, const int & fieldref, const QString &filter) const
{
    QString req = QString("SELECT count(`%1`) FROM `%2`").arg(d->m_Fields.value(1000 * tableref + fieldref)).arg(d->m_Tables[tableref]);
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery q(req, database());
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0).toInt();
        } else {
            LOG_QUERY_ERROR_FOR("Database", q);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", q);
    }
    return -1;
}

/**
   Return a MAX SQL command on the table \e tableref, field \e fieldref with the filter \e filter.
  Filter whould be not contains the "WHERE" word.
*/
QVariant Database::max(const int &tableref, const int &fieldref, const QString &filter) const
{
    QString req = QString("SELECT max(%1) FROM %2")
                  .arg(d->m_Fields.value(1000 * tableref + fieldref))
                  .arg(d->m_Tables[tableref]);
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery q(req, database());
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0);
        } else {
            LOG_QUERY_ERROR_FOR("Database", q);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", q);
    }
    return QVariant();
}

/**
   Return a MAX SQL command with grouping on the table \e tableref, field \e fieldref, grouped by field \e groupBy with the filter \e filter.
  Filter whould be not contains the "WHERE" word.
*/
QVariant Database::max(const int &tableref, const int &fieldref, const int &groupBy, const QString &filter) const
{
    QString req = QString("SELECT max(%1) FROM %2 GROUP BY %3")
                  .arg(d->m_Fields.value(1000 * tableref + fieldref))
                  .arg(d->m_Tables[tableref])
                  .arg(d->m_Fields.value(1000 * tableref + groupBy));
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery q(req, database());
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0);
        } else {
            LOG_QUERY_ERROR_FOR("Database", q);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", q);
    }
    return QVariant();
}

/**
   Return a MIN SQL command with grouping on the table \e tableref, field \e fieldref, grouped by field \e groupBy with the filter \e filter.
  Filter whould be not contains the "WHERE" word.
*/
QVariant Database::min(const int &tableref, const int &fieldref, const QString &filter) const
{
    QString req = QString("SELECT MIN(%1) FROM %2")
                  .arg(d->m_Fields.value(1000 * tableref + fieldref))
                  .arg(d->m_Tables[tableref]);
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery q(req, database());
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0);
        } else {
            LOG_QUERY_ERROR_FOR("Database", q);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", q);
    }
    return QVariant();
}

/**
   Return a TOTAL SQL command on the table \e tableref, field \e fieldref with the filter \e filter.
   Filter whould be not contains the "WHERE" word.
*/
QString Database::totalSqlCommand(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const
{
    QString toReturn;
    if (where.count()) {
        toReturn = QString("SELECT SUM(`%1`) FROM `%2` WHERE %3")
                   .arg(d->m_Fields.value(fieldRef + tableRef * 1000))
                   .arg(d->m_Tables.value(tableRef))
                   .arg(getWhereClause(tableRef, where));
    } else  {
        toReturn = QString("SELECT SUM(`%1`) FROM `%2`")
                   .arg(d->m_Fields.value(fieldRef + tableRef * 1000))
                   .arg(d->m_Tables.value(tableRef));
    }
    return toReturn;
}

/** Return a TOTAL SQL command on the table \e tableref, field \e fieldref. */
QString Database::totalSqlCommand(const int tableRef, const int fieldRef) const
{
    QString toReturn;
    toReturn = QString("SELECT SUM(`%1`) FROM `%2`")
               .arg(d->m_Fields.value(fieldRef + tableRef * 1000))
               .arg(d->m_Tables.value(tableRef));
    return toReturn;
}

double Database::sum(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const
{
    QSqlQuery q(totalSqlCommand(tableRef, fieldRef, where), database());
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0).toDouble();
        } else {
            LOG_QUERY_ERROR_FOR("Database", q);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", q);
    }
    return 0.0;
}

double Database::sum(const int tableRef, const int fieldRef) const
{
    QSqlQuery q(totalSqlCommand(tableRef, fieldRef), database());
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0).toDouble();
        } else {
            LOG_QUERY_ERROR_FOR("Database", q);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", q);
    }
    return 0.0;
}

/** Return a SQL command usable for QSqlQuery::prepareInsertQuery(). Fields are ordered. */
QString Database::prepareInsertQuery(const int tableref) const
{
    QString toReturn;
    QString fields;
    QString numbers;
    QList<int> list = d->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int & i, list)
    {
        fields.append("`"+ d->m_Fields.value(i) + "`, ");
        numbers.append("? , ");
    }
    fields.chop(2);
    numbers.chop(2);
    toReturn = QString("INSERT INTO `%1` \n(%2) \nVALUES (%3);")
            .arg(table(tableref))
            .arg(fields)
            .arg(numbers);
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/** Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
QString Database::prepareUpdateQuery(const int tableref, const int fieldref, const QHash<int, QString> &conditions)
{
    QString toReturn;
    toReturn = QString("UPDATE `%1` SET `%2` = ? WHERE %4")
               .arg(table(tableref))
               .arg(fieldName(tableref, fieldref))
               .arg(getWhereClause(tableref, conditions));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/** Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
QString Database::prepareUpdateQuery(const int tableref, const QList<int> &fieldref, const QHash<int, QString> &conditions)
{
    QString toReturn;
    QString tmp;
    foreach(const int &i, fieldref) {
        tmp += "`" + fieldName(tableref, i) + "`= ?, ";
    }
    tmp.chop(2);
    toReturn = QString("UPDATE `%1` SET %2 WHERE %4")
               .arg(table(tableref))
               .arg(tmp)
               .arg(getWhereClause(tableref, conditions));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/** Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
QString Database::prepareUpdateQuery(const int tableref, const int fieldref)
{
    QString toReturn;
    toReturn = QString("UPDATE `%1` SET `%2` =?")
               .arg(table(tableref))
               .arg(fieldName(tableref, fieldref));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**  Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
QString Database::prepareUpdateQuery(const int tableref, const QHash<int, QString> &conditions)
{
    QString toReturn;
    QString tmp;
    foreach(const QString &f, fieldNames(tableref))
        tmp += QString ("`%1`=? , ").arg(f);
    tmp.chop(2);
    toReturn = QString("UPDATE `%1` SET \n%2 \nWHERE %3")
               .arg(table(tableref))
               .arg(tmp)
               .arg(getWhereClause(tableref, conditions));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**  Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
QString Database::prepareUpdateQuery(const int tableref)
{
    QString toReturn;
    QString tmp;
    foreach(const QString &f, fieldNames(tableref))
        tmp += QString ("`%1`=? , ").arg(f);
    tmp.chop(2);
    toReturn = QString("UPDATE `%1` SET \n%2 ")
               .arg(table(tableref))
               .arg(tmp);
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/** Return a SQL command usable for DELETE command. Fields are ordered. */
QString Database::prepareDeleteQuery(const int tableref)
{
    return prepareDeleteQuery(tableref, QHash<int,QString>());
}

/** Return a SQL command usable for DELETE command. Fields are ordered. */
QString Database::prepareDeleteQuery(const int tableref, const QHash<int,QString> &conditions)
{
    QString toReturn;
    if (!conditions.isEmpty()) {
        toReturn = QString("DELETE FROM `%1` \n WHERE %2")
                .arg(table(tableref))
                .arg(getWhereClause(tableref, conditions));
    } else {
        toReturn = QString("DELETE FROM `%1`")
                .arg(table(tableref));
    }
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/** Create table \e tableref in the database. */
bool Database::createTable(const int &tableref) const
{
    if (!d->m_Tables.contains(tableref))
        return false;
    if (! d->m_Tables_Fields.keys().contains(tableref))
        return false;
    if (d->m_ConnectionName.isEmpty())
        return false;

    // get database and open
    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);
    if (!DB.isOpen())
        return false;

    // create query
    QStringList req;
    req = d->getSQLCreateTable(tableref);

    return executeSQL(req, DB);
}

/** Create all the tables in the database. */
bool Database::createTables() const
{
    bool toReturn = true;
    QList<int> list = d->m_Tables.keys();
    qSort(list);
    foreach(const int & i, list) {
        if(!createTable(i)) {
            toReturn = false;
            LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "Can not create table %1").arg(table(i)));
        }
    }
    return toReturn;
}

/** Execute simple SQL commands on the QSqlDatabase \e DB. */
bool Database::executeSQL(const QStringList &list, const QSqlDatabase &DB)
{
    if (!DB.isOpen())
        return false;

    foreach(const QString &r, list)
    {
        if (r.isEmpty())
            continue;

        QSqlQuery q(r, DB);
        if (!q.isActive()) {
            Log::addQueryError("Database", q);
            return false;
        }
        q.finish();
    }
    return true;
}

/** Execute simple SQL commands on the QSqlDatabase \e DB. */
bool Database::executeSQL(const QString &req, const QSqlDatabase & DB)
{
    if (req.isEmpty())
        return false;
    /** \todo manage ; inside "" or '' */
    QStringList list = req.split(";\n", QString::SkipEmptyParts);
    return executeSQL(list, DB);
}

/**
   Execute simple SQL commands stored in a file on the QSqlDatabase connected as \e connectionName.
  Line starting with '-- ' are ignored.\n
  All SQL commands must end with a ;
*/
bool Database::executeSqlFile(const QString &connectionName, const QString &fileName, QProgressDialog *dlg)
{
    if (!QFile::exists(fileName)) {
        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(fileName));
        return false;
    }

    // execute all sql queries
    QString req = QString::fromUtf8(file.readAll());
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");

    QStringList list = req.split("\n");
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            return false;
        }
    }

//    if (!DB.transaction()) {
//        LOG_ERROR_FOR("Tools", "Can not create transaction. Tools::executeSqlFile()", __FILE__, __LINE__);
//        return false;
//    }

    req.clear();
    QStringList queries;
    // Reconstruct req : removes comments
    foreach(const QString &s, list) {
        if (s.startsWith("--")) {
            continue;
        }
        req += s + "\n";
        if (s.endsWith(";")) {
            queries.append(req);
            req.clear();
        }
    }

    // Execute queries
    if (dlg)
        dlg->setRange(0, queries.count());

    foreach(const QString &sql, queries) {
        QString q = sql.simplified();
        // Do not processed empty strings
        if (q.isEmpty())
            continue;

        // No SQLite extra commands
        if (q.startsWith("."))
            continue;

        // No BEGIN, No COMMIT
        if (q.startsWith("BEGIN", Qt::CaseInsensitive) || q.startsWith("COMMIT", Qt::CaseInsensitive))
            continue;

        QSqlQuery query(sql, DB);
        if (!query.isActive()) {
            LOG_QUERY_ERROR_FOR("Database", query);
            qWarning() << DB.database() << DB.hostName() << DB.userName() << DB.isOpenError();
//            DB.rollback();
            return false;
        }

        if (dlg)
            dlg->setValue(dlg->value()+1);
    }
//    DB.commit();
    return true;
}

/** Import a CSV structured file \e fielName into a database \e connectionName, table \e table, using the speficied \e separator, and \e ingoreFirstLine or not.*/
bool Database::importCsvToDatabase(const QString &connectionName, const QString &fileName, const QString &table, const QString &separator, bool ignoreFirstLine)
{
    // get database
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(db.connectionName(), db.lastError().text()));
            return false;
        }
    }

    QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);
    if (content.isEmpty())
        return false;
    QStringList lines = content.split("\n", QString::SkipEmptyParts);
    content.clear();
    int start = 0;
    if (ignoreFirstLine)
        start = 1;

    // get table field's name
    if (!db.tables().contains(table)) {
        LOG_ERROR_FOR("Database", "No table found");
        return false;
    }
    // prepare the sql query
    QSqlRecord record = db.record(table);
    QString req = "INSERT INTO " + table + " (\n";
    for(int i = 0; i < record.count(); ++i) {
        req += "`" + record.fieldName(i) + "`, ";
    }
    req.chop(2);
    req += ")\n VALUES (";

    db.transaction();

    for(int i = start; i < lines.count(); ++i) {

        QStringList values = lines.at(i).split(separator, QString::KeepEmptyParts);
//        qWarning() << lines.at(i) << separator << values;
        QString reqValues;
        int counter = 0;
        foreach(const QString &val, values) {
            ++counter;
            if (val.isEmpty()) {
                reqValues += "NULL, ";
            } else {
                /** \todo this needs some improvements (string, numbers, " chars...) */
                if (val.startsWith("'") && val.endsWith("'")) {
                    reqValues += val + ", ";
                } else if (val.startsWith("\"") && val.endsWith("\"")) {
                    reqValues += val + ", ";
                } else if (val.contains(QRegExp("\\D", Qt::CaseInsensitive))) {
                    QString tmp = val;
                    reqValues += "\"" + tmp.replace("\"", "“") + "\", ";
                } else {
                    reqValues += val + ", ";
                }
            }
        }
        reqValues.chop(2);
        reqValues += ");\n";
        QSqlQuery query(req + reqValues, db);
        if (!query.isActive()) {
            LOG_QUERY_ERROR_FOR("Database", query);
        }
//        else{
//            if (counter < 5)
//            {
//            	  qDebug() << __FILE__ << QString::number(__LINE__) << " query  =" << query.lastQuery() ;
//                }
//        }
//        qWarning() << lines.at(i) << req + reqValues << values;
    }

    db.commit();

    return true;
}

/** Create the SQL command for the table creation \e tableref. \sa addTable(), addField() */
QStringList DatabasePrivate::getSQLCreateTable(const int &tableref)
{
    QString toReturn;
    toReturn = QString("CREATE TABLE IF NOT EXISTS `%1` (\n  ").arg(m_Tables.value(tableref));
    QList<int> list = m_Tables_Fields.values(tableref);
    qSort(list);
    QStringList fieldLine;

    // Find the max field name length
    int maxLength = 0;
    for(int i=0; i < list.count(); ++i) {
        const QString &f = m_Fields.value(list.at(i));
        maxLength = qMax(maxLength, f.size());
    }
    maxLength += 3;

    // Create SQL field lines
    foreach(int i, list) {
        QString fieldName = QString("`%1`").arg(m_Fields.value(i)).leftJustified(maxLength, ' ');
        // Manage NULL value
        if (m_DefaultFieldValue.value(i) == "NULL") {
            if (Database::TypeOfField(m_TypeOfField.value(i)) != Database::FieldIsUniquePrimaryKey) {
                fieldLine.append(QString("%1 %2 DEFAULT NULL")
                                .arg(fieldName)
                                .arg(getTypeOfField(i)));// .leftJustified(20, ' '))
            } else {
                fieldLine.append(QString("%1 %2")
                                .arg(fieldName)
                                .arg(getTypeOfField(i)));// .leftJustified(20, ' '))
            }
        } else {
            // Manage DEFAULT value by type of field
            switch (Database::TypeOfField(m_TypeOfField.value(i)))
            {
            case Database::FieldIsUUID :
            case Database::FieldIsLongText :
            case Database::FieldIsShortText :
            case Database::FieldIsLanguageText :
            case Database::FieldIsBlob :
                fieldLine.append(QString("%1 %2 DEFAULT '%3'")
                                .arg(fieldName)
                                .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                .arg(m_DefaultFieldValue.value(i)));
                break;
            case Database::FieldIsDate :
                {
                    QString defVal = m_DefaultFieldValue.value(i).simplified();
                    if (defVal.startsWith("CUR")) {
                        if (m_Driver==Database::MySQL) {
                            // CURRENT_DATE as default value is not supported by MySQL
                            defVal = "NULL";
                        } else if (defVal.endsWith("()")) {
                            defVal = defVal.remove("()");
                        }
                        fieldLine.append(QString("%1 %2 DEFAULT %3")
                                         .arg(fieldName)
                                         .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                         .arg(defVal));
                    }
                    else
                        fieldLine.append(QString("%1 %2 DEFAULT '%3'")
                                        .arg(fieldName)
                                        .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                        .arg(m_DefaultFieldValue.value(i)));
                    break;
                }
            case Database::FieldIsBoolean :
            case Database::FieldIsInteger :
            case Database::FieldIsLongInteger :
            case Database::FieldIsReal :
                fieldLine.append(QString("%1 %2 DEFAULT %3")
                                .arg(fieldName)
                                .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                .arg(m_DefaultFieldValue.value(i)));
                break;
            default :
                fieldLine.append(QString("%1 %2 DEFAULT '%3'")
                                .arg(fieldName)
                                .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                .arg(m_DefaultFieldValue.value(i)));
                break;

        }
        }
    }
    toReturn.append(fieldLine.join(",\n  "));

    foreach(int field, m_PrimKeys.values(tableref)) {
        int ref = field + (tableref * 1000);
        if (m_TypeOfField.value(ref) != Database::FieldIsUniquePrimaryKey) {
            toReturn.append(QString(",\nPRIMARY KEY(%1)").arg(m_Fields.value(ref)));
        }
    }
    toReturn.append("\n);\n");

    QStringList indexes;
    for(int i = 0; i < m_DbIndexes.count(); ++i) {
        const DbIndex &idx = m_DbIndexes.at(i);
        if (idx.field.table==tableref) {
            indexes << QString("CREATE INDEX %1 ON %2 (%3);\n")
                    .arg(idx.name)
                    .arg(idx.field.tableName)
                    .arg(idx.field.fieldName);
        }
    }

    if (WarnCreateTableSqlCommand)
        qWarning() << toReturn << "\nIndexes: \n" << indexes;

    return QStringList() << toReturn << indexes;
}

/** Return the SQL type of field for the field \e fieldref.*/
QString DatabasePrivate::getTypeOfField(const int &fieldref) const
{
    QString toReturn;
    switch (Database::TypeOfField(m_TypeOfField.value(fieldref)))
    {
        case Database::FieldIsUUID :
            toReturn = "varchar(32)";
            break;
        case Database::FieldIsBoolean :
            toReturn = "int(1)";
            break;
        case Database::FieldIsLongText :
            toReturn = "varchar(2000)";
            break;
        case Database::FieldIsShortText :
            toReturn = "varchar(200)";
            break;
        case Database::FieldIsLanguageText :
            toReturn = "varchar(2)";
            break;
        case Database::FieldIsBlob :
            toReturn = "blob";
            break;
        case Database::FieldIsDate :
            toReturn = "date";
            break;
    case Database::FieldIsTime:
        toReturn = "time";
        break;
    case Database::FieldIsDateTime:
        toReturn = "datetime";
        break;
    case Database::FieldIsOneChar :
            toReturn = "varchar(1)";
            break;
        case Database::FieldIsInteger :
            toReturn = "integer";
            break;
        case Database::FieldIsUniquePrimaryKey :
            if (m_Driver==Database::SQLite) {
                toReturn = "integer not null primary key";
            } else if (m_Driver==Database::MySQL) {
                toReturn = "integer unsigned not null primary key auto_increment";
            }
            break;
        case Database::FieldIsLongInteger :
            toReturn = "int(11)";
            break;
        case Database::FieldIsReal :
            toReturn = "double";
            break;
        default : toReturn = QString::null; break;
    }
    return toReturn;
}

/**  Used for the debugging. */
void Database::warn() const
{
    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);
    if (WarnLogMessages)
        Log::addMessage("Database", QString("Connection name: %1, Database Name: %2, Driver: %3, Opened: %4, Can open: %5 ")
                       .arg(d->m_ConnectionName, DB.databaseName(), DB.driverName())
                       .arg(DB.isOpen())
                       .arg(DB.open()));

    foreach(int i, d->m_Tables.keys())
    {
        if (WarnLogMessages)
            Log::addMessage("Database", QString("Tables = %1: %2").arg(i).arg(d->m_Tables[i]));
        QList<int> list = d->m_Tables_Fields.values(i);
        qSort(list);
        foreach(int f, list)
            if (WarnLogMessages)
                Log::addMessage("Database", QString("    Fields = %1: %2 %3 %4")
                               .arg(f)
                               .arg(d->m_Fields[f], d->getTypeOfField(f), d->m_DefaultFieldValue[i]));

    }
}

/**  Used for debugging and information purpose. */
void Database::toTreeWidget(QTreeWidget *tree)
{
    QFont bold;
    bold.setBold(true);
    tree->clear();

    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);

    // General information
    QTreeWidgetItem *db = new QTreeWidgetItem(tree, QStringList() << "General information");
    db->setFont(0, bold);
    new QTreeWidgetItem(db, QStringList() << "Connection Name" << d->m_ConnectionName);
    new QTreeWidgetItem(db, QStringList() << "Database Name" << DB.databaseName());
    if (DB.isOpenError()) {
        QTreeWidgetItem *e = new QTreeWidgetItem(db, QStringList() << "Error" << DB.lastError().text());
        e->setFont(0, bold);
    } else {
        new QTreeWidgetItem(db, QStringList() << "Connected" << "Without error");
    }

    // Driver / Connection // ConnectionError
    QTreeWidgetItem *drv = new QTreeWidgetItem(tree, QStringList() << "Driver information");
    drv->setFont(0, bold);
    new QTreeWidgetItem(drv, QStringList() << "Qt Driver" << DB.driverName());
    if (DB.driverName()=="QSQLITE") {
        new QTreeWidgetItem(drv, QStringList() << "Driver" << "SQLite");
        QString path = QFileInfo(DB.databaseName()).absolutePath();
        path = QDir(qApp->applicationDirPath()).relativeFilePath(path);
        new QTreeWidgetItem(drv, QStringList() << "Path" << path);
        new QTreeWidgetItem(drv, QStringList() << "FileName" << QFileInfo(DB.databaseName()).baseName());
    }
    else if (DB.driverName()=="QMYSQL") {
        new QTreeWidgetItem(drv, QStringList() << "Driver" << "MySQL");
        new QTreeWidgetItem(drv, QStringList() << "Host" << DB.hostName());
        new QTreeWidgetItem(drv, QStringList() << "Port" << QString::number(DB.port()));
        new QTreeWidgetItem(drv, QStringList() << "Login" << "****");
        new QTreeWidgetItem(drv, QStringList() << "Password" << "****");
    }
    else if (DB.driverName()=="QPSQL") {
        new QTreeWidgetItem(drv, QStringList() << "Driver" << "PostGreSQL");
        new QTreeWidgetItem(drv, QStringList() << "Host" << DB.hostName());
        new QTreeWidgetItem(drv, QStringList() << "Port" << QString::number(DB.port()));
        new QTreeWidgetItem(drv, QStringList() << "Login" << "****");
        new QTreeWidgetItem(drv, QStringList() << "Password" << "****");
    }

    // Grants
    QTreeWidgetItem *grants = new QTreeWidgetItem(tree, QStringList() << "Grants");
    grants->setFont(0, bold);
    Database::Grants g = d->m_Grants.value(d->m_ConnectionName);
    if (g & Database::Grant_All) {
        new QTreeWidgetItem(grants, QStringList() << "ALL PRIVILEGES");
    } else {
        QHash<QString, int> ref;
        ref.insert("ALL PRIVILEGES", Database::Grant_All);
        ref.insert("ALTER", Database::Grant_Alter);
        ref.insert("ALTER ROUTINE", Database::Grant_AlterRoutine);
        ref.insert("CREATE", Database::Grant_Create);
        ref.insert("CREATE ROUTINE", Database::Grant_CreateRoutine);
        ref.insert("CREATE TEMPORARY TABLES", Database::Grant_CreateTmpTables);
        ref.insert("CREATE USER", Database::Grant_CreateUser);
        ref.insert("CREATE VIEW", Database::Grant_CreateView);
        ref.insert("DELETE", Database::Grant_Delete);
        ref.insert("DROP", Database::Grant_Drop);
        ref.insert("EXECUTE", Database::Grant_Execute);
        ref.insert("GRANT OPTION", Database::Grant_Options);
        ref.insert("INDEX", Database::Grant_Index);
        ref.insert("INSERT", Database::Grant_Insert);
        ref.insert("LOCK TABLES", Database::Grant_LockTables);
        ref.insert("PROCESS", Database::Grant_Process);
        ref.insert("SELECT", Database::Grant_Select);
        ref.insert("SHOW DATABASES", Database::Grant_ShowDatabases);
        ref.insert("SHOW VIEW", Database::Grant_ShowView);
        ref.insert("TRIGGER", Database::Grant_Trigger);
        ref.insert("UPDATE", Database::Grant_Update);
        foreach(const int grant, ref.values()) {
            if (g & grant)
                new QTreeWidgetItem(grants, QStringList() << ref.key(grant));
        }
    }
    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}
