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
 *       Jerome PINGUET <jerome@jerome.cc>                                 *
 ***************************************************************************/
/**
  \class Utils::Database
  This class is a base class for databases. It manages scheme and creates SQL queries.
    The idea is to create a database scheme dynamically. Your superbase must hold the enums
    corresponding to the tables and the fields of each tables.\n
    Then Database is populated with real names of tables and fields.\n
    Then you can get what ever you want without any knowledge of the database tables, fields names and driver.\n

    It manages connections and propose a virtual member for the database creation if required.\n
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
    // If you want to create database if it doesn't exist define the member () in your superclass.
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
#include <QSqlDriver>
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

// for internal use only
static inline bool connectedDatabase(QSqlDatabase &db, int line)
{
    if (!db.isOpen()) {
        if (!db.open()) {
            Utils::Log::addError("Utils::Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(db.connectionName()).arg(db.lastError().text()), __FILE__, line);
            return false;
        }
    }
    return true;
}

namespace Utils {
namespace Internal {

/** @struct DbIndex
 *  @brief The DbIndex struct is used to add an index to a SQL table
 *  @var DbIndex::prefix
 *  Member 'prefix' contains the index prefix length
 */
struct DbIndex {
    Utils::Field field;
    QString name;
    int prefix;
};

/*
    Managing defaults
    For each new user account, create 1 MySQL user that can connect from "%"
    (any host).
    This behaviour could be fine tuned in the future but access from any
    host should be default behaviour for now.
    Make sure your MySQL server is not accessible from the wide area network.
    For secure access through the wide area network we recommend SSH tunnels.
*/
static QString mySqlHost()
{
    return QString("%");
}

class DatabasePrivate
{
public:
    enum ExtraCommand {
        NoExtra = 0,
        Count,
        Distinct,
        CountDistinct,
        Max,
        Total,
        Min
    };

    DatabasePrivate(Database *parent) :
        m_initialized(false),
        _transaction(false),
        m_LastCorrectLogin(-1),
        m_Driver(Database::SQLite),
        q(parent)
    {
    }
    ~DatabasePrivate() {}

    QStringList getSQLCreateTable(const int & tableref);
    QString getTypeOfField(const int & fieldref) const;

    // Return the index for: m_Fields, m_TypeOfField, m_DefaultFieldValue
    int index(const int tableRef, const int fieldRef)
    {
        return fieldRef + (tableRef * 1000);
    }

    int fieldFromIndex(const int index)
    {
        return index % 1000;
    }

    int tableFromIndex(const int index)
    {
        return index / 1000;
    }

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
        ref.insert("RELOAD", Database::Grant_Reload);
        //        ref.insert("USAGE", Database::Grant_Usage);

        // check grants from stringlist
        Database::Grants g = 0;
        foreach(const QString &s, grants) {
            foreach(const QString &k, ref.keys()) {
                if (s.contains(k + ",") || s.contains(k + " ON")) {
                    g |= QFlags<Database::Grant>(ref.value(k));
                }
            }
        }
        return g;
    }

    QString getSelectCommand(const FieldList &select, const JoinList &joins, const FieldList &conditions, ExtraCommand command = NoExtra) const
    {
        FieldList get, cond;
        JoinList jns;
        QString fields, from;
        QStringList tables;
        // check fields and joins on fieldName and tableName
        for(int i=0; i < select.count(); ++i) {
            get << select.at(i);
            if (get.at(i).tableName.isEmpty() || get.at(i).fieldName.isEmpty()) {
                get[i].tableName = q->table(select.at(i).table);
                get[i].fieldName = q->fieldName(select.at(i).table, select.at(i).field);
            }
        }
        for(int i=0; i < conditions.count(); ++i) {
            cond << conditions.at(i);
            if (cond.at(i).tableName.isEmpty() || cond.at(i).fieldName.isEmpty()) {
                cond[i].tableName = q->table(cond.at(i).table);
                cond[i].fieldName = q->fieldName(cond.at(i).table, cond.at(i).field);
            }
        }
        for(int i=0; i < joins.count(); ++i) {
            Field f1 = q->field(joins.at(i).field1.table, joins.at(i).field1.field);
            Field f2 = q->field(joins.at(i).field2.table, joins.at(i).field2.field);
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
        QString w;
        if (cond.count() > 0) {
            w = "\nWHERE " + q->getWhereClause(cond);
            for(int i=0; i < cond.count(); ++i) {
                tables << cond.at(i).tableName;
            }
            tables.removeDuplicates();
        }

        // Calculate joins
        QString j;
        for(int i=0; i < jns.count(); ++i) {
            j += q->joinToSql(jns.at(i)) + "\n";
            tables.removeAll(jns.at(i).field1.tableName);
        }
        tables.removeDuplicates();

        // Add tables
        foreach(const QString &tab, tables) {
            from += QString("`%1`, ").arg(tab);
        }
        from.chop(2);

        // Manage extra command
        switch (command) {
        case Count: fields = QString("count(%1)").arg(fields); break;
        case CountDistinct: fields = QString("count(distinct %1)").arg(fields); break;
        case Distinct: fields = QString("distinct(%1)").arg(fields); break;
        case Max: fields = QString("max(%1)").arg(fields); break;
        case Min: fields = QString("min(%1)").arg(fields); break;
        case Total: fields = QString("total(%1)").arg(fields); break;
        default: break;
        }

        return QString("SELECT %1 FROM %2\n%3%4").arg(fields, from, j, w);
    }

public:
    QHash<int, QString>        m_Tables;         // tables are not sorted
    QMultiHash<int, int>       m_Tables_Fields;  // links are not sorted
    QMap<int, QString>         m_Fields;         // fields should be sorted from first to last one using ref
    QHash<int, int>            m_TypeOfField;
    QHash<int, QString>        m_DefaultFieldValue;
    bool                       m_initialized, _transaction;
    int                        m_LastCorrectLogin;
    QString                    m_ConnectionName;
    QHash<QString, Database::Grants> m_Grants;
    Database::AvailableDrivers m_Driver;
    QMultiHash<int,int> m_PrimKeys; // K=table, V=field
    QVector<DbIndex> m_DbIndexes;

private:
    Database *q;
};
} // namespace Internal
} // namespace Utils

QString Utils::Database::_prefix;

/** Log to console all available drivers for database management */
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

/** CTor (no default values) */
Database::Database() :
    d_database(new DatabasePrivate(this))
{
}

Database::~Database()
{
    if (d_database)
        delete d_database;
    d_database=0;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------- Managing Databases files and connections -------------------------------
//--------------------------------------------------------------------------------------------------------
/**
 * All MySQL database used with this code are prefixed with  \e fmf_.
 * This member check that the database name starts with \e fmf_ or adds it
 * and return the corrected database name. For SQLite, the database name
 * stays unchanged.\n
 * You can add a general prefix to all your database (whatever is the driver)
 * using setDatabasePrefix(). This prefix will be applied to any Utils::Database
 * object created.\n Output looks like: {generalprefix}{fmf_}{DbName}. \n
 * This only affects read-write database.
 */
QString Database::prefixedDatabaseName(AvailableDrivers driver, const QString &dbName)
{
    QString toReturn = dbName;
    // Append global prefix whatever is the driver
    if (!_prefix.isEmpty()) {
        if (!toReturn.startsWith(_prefix)) {
            // Prepend MySQL
            if ((driver==MySQL)
                    && !toReturn.startsWith("fmf_"))
                toReturn.prepend("fmf_");
            // Prepend global prefix
            toReturn.prepend(_prefix);
        }
    } else {
        // No global prefix
        // MySQL -> add "fmf_"
        if ((driver==MySQL)
                && !toReturn.startsWith("fmf_"))
            toReturn.prepend("fmf_");
    }
    return toReturn;
}

/**
 * Creates a MySQL database on an opened database. The current connection must be opened.
 * Creates its own transaction.
*/
bool Database::createMySQLDatabase(const QString &dbName)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

    LOG_FOR("Database", QString("Trying to create database: %1\n"
                                "       on host: %2(%3)\n"
                                "       with user: %4")
            .arg(dbName).arg(database().hostName()).arg(database().port()).arg(database().userName()));

    DB.transaction();
    QSqlQuery query(DB);
    if (!query.exec(QString("CREATE DATABASE `%1`;").arg(dbName))) {
        LOG_QUERY_ERROR_FOR("Database", query);
        DB.rollback();
        return false;
    }
    LOG_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    query.finish();
    DB.commit();
    return true;
}

/**
 * Create a MySQL server user using the \e log and \e pass,
 * with the specified \e grants
 * with rights to access prefixed database using \e databasePrefix
 * ('fmf_' will be automatically added to this prefix).
 * Leave \e host empty to use the default one ('fmf_').\n
 * Creates its own transaction.
*/
bool Database::createMySQLUser(const QString &log,
                               const QString &password,
                               const Grants grants,
                               const QString &databasePrefix)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

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
    
    if (g.isEmpty()) {
        LOG_ERROR_FOR("Database","No grants when creating user");
        return false;
    }

    QString prefix = databasePrefix + "fmf_%";
    prefix = prefix.replace("_", "\\_");
    LOG_FOR("Database", QString("Trying to create MySQL user: %1\n"
                                "       on host: %2(%3)\n"
                                "       with user: %4\n"
                                "       db prefix: %5")
            .arg(log)
            .arg(database().hostName())
            .arg(database().port())
            .arg(database().userName())
            .arg(prefix));

    DB.transaction();
    QSqlQuery query(DB);
    QString req = QString("CREATE USER '%1'@'%2' IDENTIFIED BY '%3';")
            .arg(log).arg(mySqlHost()).arg(password);
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        DB.rollback();
        return false;
    }
    query.finish();

    req = QString("GRANT %1 ON `%2`.* TO '%3'@'%4';")
            .arg(g).arg(prefix).arg(log).arg(mySqlHost());
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        query.finish();
        DB.rollback();
        req = QString("DROP USER '%1'@'%2'").arg(log).arg(mySqlHost());
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR("Database", query);
            LOG_DATABASE_FOR("Database", database());
        } else {
            LOG_ERROR_FOR("Database", QString("User %1 removed").arg(log));
        }
        return false;
    }
    query.finish();
    
    // Manage user creation grant
    if (grants & Grant_CreateUser) {
        // grant privileges CREATE USER, WITH GRANT OPTION on host
        req = QString("GRANT CREATE USER ON *.* TO '%1'@'%2' WITH GRANT OPTION;")
                .arg(log).arg(mySqlHost());
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR("Database", query);
            LOG_DATABASE_FOR("Database", database());
            query.finish();
            DB.rollback();
            req = QString("DROP USER '%1'@'%2'").arg(log).arg(mySqlHost());
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

    DB.commit();
    LOG_FOR("Database", tkTr(Trans::Constants::DATABASE_USER_1_CORRECTLY_CREATED).arg(log));
    return true;
}

/**
 * Drop a MySQL user identified by his \e log and the \e userHostName.
 * Creates its own transaction.
*/
bool Database::dropMySQLUser(const QString &log, const QString &userHost)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

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
    DB.transaction();
    QSqlQuery query(DB);
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        DB.rollback();
        return false;
    } else {
        LOG_FOR("Database", QString("User %1 removed").arg(log));
    }
    DB.commit();
    return true;
}

/**
 * Change the password to \e newPassword for a MySQL user identified by his \e login.
 * Creates its own transaction.\n
 * \warning Use at own risk as this function does not make checks and uses SET PASSWORD (for the current MySQL user)
*/
bool Database::changeMySQLUserOwnPassword(const QString &login, const QString &newPassword)
{
    if (login.isEmpty())
        return false;

    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

    LOG_FOR("Database", QString("Trying to change MySQL OWN user password:\n"
                                ".user: %1\n"
                                "@%2(%3)\n"
                                ".pass: %4")
            .arg(login)
            .arg(database().hostName())
            .arg(database().port())
            .arg(QString().fill('*', newPassword.size())));

    DB.transaction();
    QSqlQuery query(DB);
    QString req;
    req = QString("SET PASSWORD = PASSWORD('%1');")
            .arg(newPassword);
    
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        DB.rollback();
        return false;
    }
    
    query.finish();
    
    DB.commit();
    LOG_FOR("Database", QString("User %1 password modified").arg(login));
    return true;
}

/**
 * Change the password to \e newPassword for a MySQL user identified by his \e login.
 * Creates its own transaction.\n
 * \warning Use at own risk as this function does not make checks and uses UPDATE `mysql`.`user`...
 * \warning Before calling this method you must take care that the current logged user has administrative rights to update users data
*/
bool Database::changeMySQLOtherUserPassword(const QString &login, const QString &newPassword)
{
    if (login.isEmpty())
        return false;

    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

    LOG_FOR("Database", QString("Trying to change MySQL OTHER user password:\n"
                                "       user: %1\n"
                                "       host: %2(%3)\n"
                                "       new password: %4")
            .arg(login)
            .arg(database().hostName())
            .arg(database().port())
            .arg(newPassword));

    DB.transaction();
    QSqlQuery query(DB);
    QString req;
    QStringList hostNames = mySQLUserHostNames(login);

    foreach(const QString &hostName, hostNames ) {
        req = QString("SET PASSWORD FOR '%1'@'%2' = PASSWORD('%3');")
                .arg(login).arg(hostName).arg(newPassword);
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR("Database", query);
            LOG_DATABASE_FOR("Database", database());
            DB.rollback();
            return false;
        }
        query.finish();
    }
    if (!query.exec("FLUSH PRIVILEGES;")) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_DATABASE_FOR("Database", database());
        DB.rollback();
        return false;
    }
    DB.commit();
    LOG_FOR("Database", QString("User %1 password modified").arg(login));
    return true;
}

/** Return the pointer to the QSqlDatabase in use. */
QSqlDatabase Database::database() const
{
    return QSqlDatabase::database(d_database->m_ConnectionName);
}

/**
 * Returns the absolute filename of the QSLite database
 * according to its options & app options.
 * \note unit-tested
 */
QString Database::sqliteFileName(const QString &connectionName,
                                 const QString &nonPrefixedDbName,
                                 const Utils::DatabaseConnector &connector,
                                 bool addGlobalPrefix)
{
    QString fileName;
    if (connector.accessMode()==DatabaseConnector::ReadOnly) {
        if (connector.useExactFile())
            fileName = QString("%1/%2")
                    .arg(connector.absPathToSqliteReadOnlyDatabase())
                    .arg(nonPrefixedDbName);
        else
            fileName = QString("%1/%2/%3")
                    .arg(connector.absPathToSqliteReadOnlyDatabase())
                    .arg(connectionName)
                    .arg(nonPrefixedDbName);
    } else if (connector.accessMode()==DatabaseConnector::ReadWrite) {
        if (connector.useExactFile()) {
            fileName = QString("%1/%2")
                    .arg(connector.absPathToSqliteReadWriteDatabase())
                    .arg(nonPrefixedDbName);
        } else {
            if (addGlobalPrefix) {
                fileName = QString("%1/%2/%3")
                        .arg(connector.absPathToSqliteReadWriteDatabase())
                        .arg(connectionName)
                        .arg(prefixedDatabaseName(connector.driver(), nonPrefixedDbName));
            } else {
                fileName = QString("%1/%2/%3")
                        .arg(connector.absPathToSqliteReadWriteDatabase())
                        .arg(connectionName)
                        .arg(nonPrefixedDbName);
            }
        }
    }
    if (!fileName.endsWith(".db"))
        fileName += ".db";
    return QDir::cleanPath(fileName);
}

/**
 * Create the connection to the database.
 * If database does not exist, according to the \e createOption,
 * createDatabase() is called.
 *
 * An error is returned if :
 * - Driver is not available
 * - Can not connect to server (wrong host/log/pass)
 * - Can not create database if it doesn't exists and user asked to create it
 * - Can not read database if asked to be readable
 * - Can not write in database if asked to be writable
 *
 * \param connectionName = name of the connect
 * \param connector = Utils::DatabaseConnector = connection params
 * \param createOption = what to do if the database does not exist.
 */
bool Database::createConnection(const QString &connectionName,
                                const QString &nonPrefixedDbName,
                                const Utils::DatabaseConnector &connector,
                                CreationOption createOption
                                )
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    bool toReturn = true;
    d_database->m_Driver = connector.driver();
    QString prefixedDbName = prefixedDatabaseName(d_database->m_Driver, nonPrefixedDbName);

    if (WarnLogMessages) {
        LOG_FOR("Database", connectionName + "  //  " + prefixedDbName);
        qWarning() << connector;
    }

    // Is the driver available?
    if (!connector.isDriverValid()) {
        LOG_ERROR_FOR("Database", "Driver is not valid");
        return false;
    }

    // Does the connection already exists ?
    if (QSqlDatabase::contains(connectionName)) {
        if (WarnLogMessages)
            LOG_FOR("Database", QCoreApplication::translate("Database",
                                                            "WARNING: %1 database already in use")
                    .arg(connectionName));
        d_database->m_ConnectionName = connectionName;
        return true;
    }

    QSqlDatabase DB;

    // Construct SQLite database fileName
    QString fileName = sqliteFileName(connectionName, nonPrefixedDbName, connector);
    QFileInfo sqliteFileInfo(fileName);

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // if DeleteAndRecreateDatabase only for read/write databases
    if (createOption == DeleteAndRecreateDatabase
            && connector.accessMode() == DatabaseConnector::ReadWrite) {
        LOG_FOR("Database", "Delete database before re-creating it. Connection: " + connectionName + "; PathOrHost: " + prefixedDbName);
        switch (connector.driver()) {
        case SQLite:
        {
            if (sqliteFileInfo.exists()) {
                QFile f(sqliteFileInfo.absoluteFilePath());
                QString newName = QString("%1/%2-bkup-%3.%4")
                        .arg(sqliteFileInfo.absolutePath())
                        .arg(sqliteFileInfo.baseName())
                        .arg(QDateTime::currentDateTime().toString("yyyyMMddhhMMss"))
                        .arg(sqliteFileInfo.completeSuffix());
                if (!f.rename(newName))
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
            if (!query.exec("DROP DATABASE " + prefixedDbName)) {
                LOG_QUERY_ERROR_FOR("Database", query);
                LOG_ERROR_FOR("Database", "Unable to drop database");
            }
            if (WarnLogMessages)
                LOG_FOR("Database", QString("Connected to host %1").arg(connector.host()));
            break;
        }
        }
        createOption = CreateDatabase;
    }
    if (QSqlDatabase::contains("__DB_DELETOR" + connectionName))
        QSqlDatabase::removeDatabase("__DB_DELETOR" + connectionName);

    // check server connection
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
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
    } // switch

    // create database if it doesn't exist and user ask for database creation
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    switch (connector.driver()) {
    case SQLite:
    {
        if ((!sqliteFileInfo.exists()) || (sqliteFileInfo.size() == 0)) {
            if (createOption == CreateDatabase) {
                if (!createDatabase(connectionName, sqliteFileInfo.fileName(), connector, createOption)) {
                    LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(prefixedDbName + "@" + fileName).arg("createDatabase returned false"));
                    return false;
                }
            } else { // Warn Only
                LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(prefixedDbName + "@" + fileName).arg("createDatabase not called"));
                return false;
            }
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        // now that the file was created, reload the fileinfo
        sqliteFileInfo.setFile(sqliteFileInfo.absoluteFilePath());
        break;
    }
    case MySQL:
    {
        // can get connection to server ?
        DB.setDatabaseName(prefixedDbName);
        if (!DB.open()) {
            if (createOption == CreateDatabase) {
                if (!createDatabase(connectionName, prefixedDbName, connector, createOption)) {
                    LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(prefixedDbName + "@" + connector.host()).arg(""));
                    return false;
                }
            } else { // Warn Only
                if (WarnLogMessages)
                    LOG_ERROR_FOR("Database", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(prefixedDbName + "@" + connector.host()).arg(""));
                return false;
            }
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        if (WarnLogMessages)
            LOG_FOR("Database", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(prefixedDbName).arg(DB.driverName()));
        break;
    }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // test read access to database
    switch (connector.driver()) {
    case SQLite:
    {
        if (!sqliteFileInfo.isReadable()) {
            LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "ERROR: Database `%1` is not readable. Path: %2")
                          .arg(prefixedDbName, sqliteFileInfo.absoluteFilePath()));
            toReturn = false;
        }
        break;
    }
    case MySQL:
    {
        if (!DB.isOpen()) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            if (!DB.open()) {
                LOG_ERROR_FOR("Database", QCoreApplication::translate("Database",
                                                                      "ERROR: Database %1 is not readable. Host: %2")
                              .arg(prefixedDbName, connector.host()));
                return false;
            }
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        QSqlQuery query("SHOW GRANTS FOR CURRENT_USER;", DB);
        if (!query.isActive()) {
            LOG_ERROR_FOR("Database", QCoreApplication::translate("Database",
                                                                  "ERROR: Database %1 is not readable. Path: %2")
                          .arg(prefixedDbName, connector.host()));
            LOG_QUERY_ERROR_FOR("Database", query);
            return false;
        } else {
            QStringList grants;
            while (query.next()) {
                grants << query.value(0).toString();
            }
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            d_database->m_Grants.insert(connectionName, d_database->getGrants(connectionName, grants));
        }
        break;
    }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // test write access
    if (connector.accessMode() == DatabaseConnector::ReadWrite) {
        switch (connector.driver()) {
        case SQLite:
        {
            if (!sqliteFileInfo.isWritable()) {
                LOG_ERROR_FOR("Database", QCoreApplication::translate("Database",
                                                                      "ERROR: Database %1 is not writable. Path: %2.")
                              .arg(prefixedDbName, fileName));
                toReturn = false;
            }
            break;
        }
        case MySQL:
        {
            // TODO: test write access to MySQL database
            break;
        }
        }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // create connection
    switch (connector.driver())
    {
    case SQLite :
    {
        // Get the SQL database
        if (QSqlDatabase::connectionNames().contains(connectionName)) {
            DB = QSqlDatabase::database(connectionName);
        } else {
            DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
            DB.setDatabaseName(sqliteFileInfo.absoluteFilePath());
        }
        if (!DB.isOpen()) {
            if (!DB.open()) {
                LOG_ERROR_FOR("Database", "Unable to open database");
                return false;
            }
        }
        break;
    }
    case MySQL :
    {
        break;
    }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (WarnLogMessages)
        LOG_FOR("Database", QCoreApplication::translate("Database",  "INFO: database %1 connection = %2")
                .arg(prefixedDbName).arg(DB.isOpen()));

    // test connection
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(prefixedDbName, DB.lastError().text()));
            toReturn = false;
        }
    } else {
        if (WarnLogMessages)
            LOG_FOR("Database", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(prefixedDbName).arg(DB.driverName()));
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // return boolean
    if (toReturn)
        d_database->m_ConnectionName = connectionName;

    return toReturn;
}

/**
 * Creates the database
 */
bool Database::createDatabase(const QString &connectionName,
                              const QString &prefixedDbName,
                              const Utils::DatabaseConnector &connector,
                              CreationOption createOption
                              )
{
    if (connector.driver()==SQLite) {
        return createDatabase(connectionName,
                              prefixedDbName,
                              connector.absPathToSqliteReadWriteDatabase() + QDir::separator() + connectionName + QDir::separator(),
                              Database::TypeOfAccess(connector.accessMode()),
                              connector.driver(),
                              connector.clearLog(),
                              connector.clearPass(),
                              connector.port(),
                              createOption);
    } else {
        return createDatabase(connectionName,
                              prefixedDbName,
                              connector.host(),
                              Database::TypeOfAccess(connector.accessMode()),
                              connector.driver(),
                              connector.clearLog(),
                              connector.clearPass(),
                              connector.port(),
                              createOption);
    }
}

/**
 * Returns the connection name in use
 * \note unit-test available
*/
QString Database::connectionName() const
{
    return d_database->m_ConnectionName;
}

/**
 * Create a database/filename compatible UUID.
 * This code uses QUuid::createUuid().toString() with some string contraction.
 */
QString Database::createUid()
{
    return Utils::createUid();
}

/**  returns the grants according to the database \e connectionName. When using a SQLite driver Grants always == 0. */
Database::Grants Database::grants(const QString &connectionName) const
{
    return d_database->m_Grants.value(connectionName, 0);
}

/**
 * Returns the grants according to the database \e connectionName.
 * Database must be open and connected with a specific user.
 * Creates its own transaction.
*/
Database::Grants Database::getConnectionGrants(const QString &connectionName) // static
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!connectedDatabase(DB, __LINE__))
        return Database::Grant_NoGrant;
    DB.transaction();

    if (DB.driverName()=="QSQLITE") {
        return Grant_All;
    }
    if (DB.driverName()=="QMYSQL") {
        QStringList grants;
        QSqlQuery query("SHOW GRANTS FOR CURRENT_USER;", DB);
        if (!query.isActive()) {
            LOG_ERROR_FOR("Database", "No grants for user on database?");
            LOG_QUERY_ERROR_FOR("Database", query);
            DB.rollback();
            return Grant_NoGrant;
        } else {
            while (query.next()) {
                grants << query.value(0).toString();
            }
        }
        query.finish();
        DB.commit();
        return DatabasePrivate::getGrants(connectionName, grants);
    }
    return Grant_NoGrant;
}


/**
 * Set connectionName to \e c
 * \note unit-test available
 */
void Database::setConnectionName(const QString &c)
{ d_database->m_ConnectionName = c; }

/**
 * Define the driver to use
 * \note unit-test available (SQLite only)
 */
void Database::setDriver(const Database::AvailableDrivers drv)
{ d_database->m_Driver = drv; }

/**
 * Returns the current driver of the database
 * \note unit-test available (SQLite only)
 */
Database::AvailableDrivers Database::driver() const
{ return d_database->m_Driver; }

/**
 * Add a table \e name to the database scheme with the index \e ref.
 * \note Unit-test available.
 */
int Database::addTable(const int & ref, const QString & name)
{
    d_database->m_Tables.insert(ref, name);
    return d_database->m_Tables.key(name);
}

/**
 * Add a field \e name to the database scheme with the
 * index \e fieldref into table indexed \e tableref.\n
 * The field is a type \e type and get the default value \e defaultValue.\n
 * Please take care that \e name can not exceed 50 chars.
 * \sa createTables(), createTable()
 * \note Unit-test available.
 */
int Database::addField(const int & tableref, const int & fieldref, const QString & name, TypeOfField type, const QString & defaultValue)
{
    Q_ASSERT_X(name.length() < 64, "Database", "Name of field can not exceed 63 chars");
    int ref = d_database->index(tableref, fieldref);
    d_database->m_Tables_Fields.insertMulti(tableref, ref);
    d_database->m_Fields.insert(ref , name);
    d_database->m_TypeOfField.insert(ref , type);
    d_database->m_DefaultFieldValue.insert(ref, defaultValue);
    return d_database->fieldFromIndex(ref);
}

/**
 * Add a primary key reference to \e tableref \e fieldref.
 * \sa createTables(), createTable(), addField(), addTable()
 * \note obsolete?
 */
void Database::addPrimaryKey(const int &tableref, const int &fieldref)
{
    d_database->m_PrimKeys.insertMulti(tableref, fieldref);
}

/**
 * Create an index on the specified \e tableRef, \e fieldRef, named \e name.
 * If \e name is not specified a unique name is created.
 */
void Database::addIndex(const int &tableref,
                        const int &fieldref,
                        const int &prefix,
                        const QString &name
                        )
{
    Utils::Field f = this->field(tableref, fieldref);
    addIndex(f, prefix, name);
}

/**
 * Create an index on the specified \e field, named \e name.
 * If \e name is not specified a unique name is created.
 * @param[in] prefix The length in characters of the index to create (the column
 * type must be string)
 */
void Database::addIndex(const Utils::Field &field, const int &prefix, const QString &name)
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
    index.prefix = prefix;
    // Store index
    d_database->m_DbIndexes.append(index);
}

/**
 * Verify that the dynamically scheme passed is corresponding to the
 * real database scheme.
 * Creates its own transaction.
*/
bool Database::checkDatabaseScheme()
{
    if (d_database->m_ConnectionName.isEmpty())
        return false;
    if (d_database->m_Tables.keys().count() == 0)
        return false;
    if (d_database->m_Tables_Fields.keys().count() == 0)
        return false;

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QSqlDatabase DB = QSqlDatabase::database(d_database->m_ConnectionName);
    if (!connectedDatabase(DB, __LINE__))
        return false;
    DB.transaction();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QList<int> list = d_database->m_Tables.keys();
    qSort(list);
    foreach(int i, list) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        QSqlRecord rec = DB.record(d_database->m_Tables.value(i));
        int expected = d_database->m_Tables_Fields.values(i).count();
        int current = rec.count();
        if (current != expected) {
            LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "Database Scheme Error: wrong number of fields for table %1 (expected: %2; current: %3)")
                          .arg(d_database->m_Tables.value(i))
                          .arg(expected)
                          .arg(current)
                          );
            return false;
        }
        QList<int> fields = d_database->m_Tables_Fields.values(i);
        qSort(fields);
        int id = 0;
        foreach(int f, fields) {
            if (d_database->m_Fields.value(f)!= rec.field(id).name()) {
                LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "Database Scheme Error: field number %1 differs: %2 instead of %3 in table %4")
                              .arg(id).arg(d_database->m_Fields.value(f), rec.field(id).name(), d_database->m_Tables.value(i)));
                return false;
            }
            id++;
        }
    }
    DB.commit();
    return true;
}

/**
 * Check the current version of the database using
 * the specified \e field and comparing the database value
 * to the \e expectedVersion.\n
 * Returns \e true if the database version is the expected one.
 * \sa getVersion(), setVersion()
 */
bool Database::checkVersion(const Field &field, const QString &expectedVersion)
{
    return (getVersion(field).compare(expectedVersion)==0);
}

/**
 * Returns the current version of the database using
 * the specified \e field.\n
 * \sa getVersion(), setVersion()
 */
QString Database::getVersion(const Field &field) const
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return QString::null;
    DB.transaction();
    QString value;
    QSqlQuery query(DB);
    if (query.exec(select(field.table, field.field))) {
        if (query.next())
            value = query.value(0).toString();
    }
    query.finish();
    DB.commit();
    return value;
}

/**
 * Returns the current version number of the database using
 * the specified \e field.\n
 * \sa getVersion(), setVersion()
 */
quint32 Database::getVersionNumber(const Field &field) const
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__)){
        return quint32(0);
    }
    DB.transaction();
    quint32 value;
    QSqlQuery query(DB);
    if (query.exec(select(field.table, field.field))) {
        if (query.next()){
            value = query.value(0).toUInt();
        }
    }
    query.finish();
    DB.commit();
    return value;
}

/**
 * Set the database version to \e version text (QString) using the field
 * \e field.
 * \warning The table containing this field will be totally
 * deleted and a new row will be inserted with the new version value.
 * \sa getVersion(), setVersion()
 */
bool Database::setVersion(const Field &field, const QString &version)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);

    // Delete all values in the version table
    query.prepare(prepareDeleteQuery(field.table));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Database", query);
        query.finish();
        DB.rollback();
        return false;
    }

    // Insert a new row with the new version
    query.prepare(prepareInsertQuery(field.table));
    FieldList fields = this->fields(field.table);
    foreach(const Field f, fields)
        query.bindValue(f.field, QVariant());
    query.bindValue(field.field, version);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Database", query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/**
 * Set the database version to \e version number (integer) using the field
 * \e field.
 * \warning The table containing this field will be totally
 * deleted and a new row will be inserted with the new version value.
 * \sa getVersion(), setVersion()
 */
bool Database::setVersion(const Field &field, const int &version)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);

    // Delete all values in the version table
    query.prepare(prepareDeleteQuery(field.table));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Database", query);
        query.finish();
        DB.rollback();
        return false;
    }

    // Insert a new row with the new version
    query.prepare(prepareInsertQuery(field.table));
    FieldList fields = this->fields(field.table);
    foreach(const Field f, fields)
        query.bindValue(f.field, QVariant());
    query.bindValue(field.field, version);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Database", query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/**
 * Returns the current schema version number of the database or 0
 * The version number is a positive integer.
 * Version number is the value of the field VERSION_NUMBER of the last row of
 * the table SCHEMA_CHANGES.
 * By definition, this number must be a positive, non null integer.
 * If the table SCHEMA_CHANGES doesn't exist this function returns 0
 */
quint32 Database::getSchemaVersionNumber(const QString &dbname) const
{
    QSqlDatabase DB = QSqlDatabase::database(dbname);
    if (!connectedDatabase(DB, __LINE__)) {
        return 0;
    }
    QSqlQuery query(DB);
    DB.transaction();
    quint32 value = 0;
    query.clear();
    if (query.exec("SELECT `VERSION_NUMBER` FROM `SCHEMA_CHANGES` ORDER BY `VERSION_NUMBER` DESC LIMIT 1")) {
        if (query.next()) {
            value = query.value(0).toUInt();
        }
    } else {
        LOG_QUERY_ERROR_FOR(dbname, query);
    }
    query.finish();
    DB.commit();
    return value;
}

/**
 * Set the database version to \e version text (QString) using the field
 * \e field.
 * \warning The table containing this field will be totally
 * deleted and a new row will be inserted with the new version value.
 * \sa getVersion(), setVersion()
 */
bool Database::setSchemaVersion(const int &version, const QString &dbname)
{
    QString scriptName = QString("initial%1").arg(dbname);
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);
    // Insert a new row with the version
    if (driver()==MySQL) {
    query.prepare("INSERT INTO `SCHEMA_CHANGES` (`VERSION_NUMBER`, `SCRIPT_NAME`, `TIMESTAMP_UTC_APPLIED`) "
                      "VALUES (:version, :script, UTC_TIMESTAMP())");
        query.bindValue(":version", version);
        query.bindValue(":script", scriptName);
    } else if (driver()==SQLite) {
        QString v = QString::number(version);
        QString timestamp =  QDateTime::currentDateTimeUtc().toString("YYYY-MM-DD HH:MM:SS");
        query.prepare("INSERT INTO SCHEMA_CHANGES(VERSION_NUMBER, SCRIPT_NAME, TIMESTAMP_UTC_APPLIED) "
                          "VALUES(?, ?, ?)");
            query.bindValue(0, v);
            query.bindValue(1, scriptName);
            query.bindValue(2, timestamp);
    } else {
        LOG_ERROR_FOR("Database", "Driver not recognized");
        return false;
    }
    if (!query.exec()) {
        QString dbName = database().databaseName();
        LOG_QUERY_ERROR_FOR(dbName, query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/**
 * Return the field name for the table \e tableref field \e fieldref
 * \sa addField()
 * \note Unit-test available.
 */
QString Database::fieldName(const int &tableref, const int &fieldref) const
{
    if (!d_database->m_Tables.contains(tableref))
        return QString::null;
    if (!d_database->m_Tables_Fields.keys().contains(tableref))
        return QString::null;
    if (!d_database->m_Fields.keys().contains(fieldref + (tableref * 1000)))
        return QString::null;

    return d_database->m_Fields.value(d_database->index(tableref, fieldref));
}

/**
 * Return a complete reference to a field for the
 * table \e tableref field \e fieldref
 * \sa addField()
 * \note Unit-test available.
 */
Field Database::field(const int &tableref, const int &fieldref) const
{
    Field ret;
    ret.table = tableref;
    ret.field = fieldref;
    ret.tableName = table(tableref);
    ret.fieldName = fieldName(tableref, fieldref);
    ret.type = typeOfField(tableref, fieldref);
    return ret;
}

/**
 * Returns all fields of a table as \e FieldList
 * \note Unit-test available.
 */
FieldList Database::fields(const int tableref) const
{
    FieldList fields;
    for(int i = 0; i < (d_database->m_Tables_Fields.values(tableref).count()); ++i) {
        fields << field(tableref, i);
    }
    return fields;
}

/**
 * Return all fields name of a table
 * \note Unit-test available.
 */
QStringList Database::fieldNames(const int &tableref) const
{
    if (!d_database->m_Tables.contains(tableref))
        return QStringList();
    if (!d_database->m_Tables_Fields.keys().contains(tableref))
        return QStringList();

    QList<int> list = d_database->m_Tables_Fields.values(tableref);
    qSort(list);
    QStringList toReturn;
    foreach(int i, list)
        toReturn << d_database->m_Fields.value(i);
    return toReturn;
}

/**
 * Return all fields names of a table by a sql query and
 * not according to tableref.
 * This permits to test the real number of fields of the
 * sql table regarding to code table references. \n
 * \note Creates its own transaction.
*/
QStringList Database::fieldNamesSql(const int &tableref) const
{
    if (!d_database->m_Tables.contains(tableref))
        return QStringList();
    if (!d_database->m_Tables_Fields.keys().contains(tableref))
        return QStringList();
    QStringList fieldNamesList;
    QString tableString = table(tableref);

    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return QStringList();
    DB.transaction();

    QSqlQuery query(DB);
    QString req;
    if (database().driverName().contains("MYSQL")) {
        req = QString("SHOW COLUMNS FROM %1").arg(tableString);
    }
    if (database().driverName().contains("SQLITE")) {
        req = QString("PRAGMA table_info('%1');")
                .arg(tableString);
    }
    if (query.exec(req)) {
        while (query.next()) {
            fieldNamesList << query.value(Name_PragmaValue).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
        Utils::warningMessageBox("Warning",QString("Unable to get the fields of %1").arg(tableString));
        DB.rollback();
        return QStringList();
    }
    DB.commit();
    return fieldNamesList;
}

/**
 * Returns the type of field TypeOfField registered
 * when using addField()
 * \note Unit-test available.
 */
Database::TypeOfField Database::typeOfField(const int tableref, const int fieldref) const
{
    int ref = d_database->index(tableref, fieldref);
    return Database::TypeOfField(d_database->m_TypeOfField.value(ref , FieldUndefined));
}

/**
 * Returns the table name for \e tableref
 * \note Unit-test available.
 */
QString Database::table(const int &tableref) const
{
    return d_database->m_Tables.value(tableref, QString());
}

/**
 * Returns all tables registered with addTable()
 * \note Unit-test available.
 */
QStringList Database::tables() const
{
    return d_database->m_Tables.values();
}

/**
 * Create a where clause on the table \e tableref using conditions mapped into a hash.
 * Conditions: key = fieldReference , value = whereClauseString.
 * If the field identifiant is -1, the condition is appended as is to the where clause.
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
        if (i.key() == -1) {
            where.append(QString(" (%1) AND ")
                         .arg(i.value()));
        } else {
            int index = d_database->index(tableref, i.key());
            if (!d_database->m_Fields.keys().contains(index))
                continue;
            where.append(QString(" (`%1`.`%2` %3) AND ")
                         .arg(d_database->m_Tables[tableref])
                         .arg(d_database->m_Fields.value(index))
                         .arg(i.value()));
        }
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
QString Database::getWhereClause(const FieldList &fields, WhereClauseType type) const
{
    QString where = "";
    QString clause;
    if (type==AND)
        clause = "AND";
    else if (type==OR)
        clause = "OR ";

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
        where += QString("(`%1`.`%2` %3) %4 ")
                .arg(tab)
                .arg(f)
                .arg(fields.at(i).whereCondition)
                .arg(clause);
    }
    where.chop(5);
    if (fields.count() > 1)
        where = QString("(%1)").arg(where);
    if (WarnSqlCommands)
        qWarning() << where;
    return where;
}

/** Create a where clause on the \e fields. */
QString Database::getWhereClause(const Field &field) const
{
    return getWhereClause(Utils::FieldList() << field);
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
    if (!conditions.isEmpty()) {
        toReturn = QString("SELECT %1 FROM `%2` WHERE %3")
                .arg(tmp)
                .arg(table(tableref))
                .arg(getWhereClause(tableref, conditions));
    } else {
        toReturn = QString("SELECT %1 FROM `%2`")
                .arg(tmp)
                .arg(table(tableref));
    }
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
    QList<int> list = d_database->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int &i, list)
        tmp += "`" + table(tableref) + "`.`" + d_database->m_Fields.value(i) + "`, ";
    //        tmp += "`" + d_database->m_Fields.value(i)+ "`, ";
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
    QList<int> list = d_database->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int & i, list)
        tmp += "`" + table(tableref) + "`.`" + d_database->m_Fields.value(i) + "`, ";
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
    QString toReturn = select(tableref, fields, conditions);
    // SELECT ... FROM ... -> SELECT DISTINCT (...) FROM ...
    toReturn = toReturn.replace("SELECT", "SELECT DISTINCT").replace("SELECT DISTINCT DISTINCT", "SELECT DISTINCT");
    //    toReturn = toReturn.replace("SELECT ", "SELECT DISTINCT (");
    //    toReturn = toReturn.replace("SELECT DISTINCT (DISTINCT", "SELECT DISTINCT");
    //    toReturn = toReturn.replace(" FROM", ") FROM");
    return toReturn;
}

/**
   Return a SELECT DISTINCT SQL command.
  \sa select()
*/
QString Database::selectDistinct(const int &tableref, const int &fieldref, const QHash<int, QString> &conditions) const
{
    return select(tableref, fieldref, conditions).replace("SELECT", "SELECT DISTINCT").replace("SELECT DISTINCT DISTINCT", "SELECT DISTINCT");
}

/**
   Return a SELECT DISTINCT SQL command.
  \sa select()
*/
QString Database::selectDistinct(const int & tableref, const int & fieldref) const
{
    return select(tableref, fieldref).replace("SELECT", "SELECT DISTINCT").replace("SELECT DISTINCT DISTINCT", "SELECT DISTINCT");
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
    return d_database->getSelectCommand(select, joins, conditions);
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
    for(int i = 0; i < (d_database->m_Tables_Fields.values(tableref).count()); ++i) {
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

QString Database::select(const Field &select, const JoinList &joins, const FieldList &conditions) const
{
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


QString Database::selectLast(const Field &fieldref) const
{
    return QString("SELECT %1 FROM %2 ORDER BY %1 DESC LIMIT 1")
            .arg(field(fieldref.table, fieldref.field).fieldName)
            .arg(table(fieldref.table));
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
 * Executes a COUNT SQL command on the table \e tableref, field \e fieldref
 * with the filter \e filter. Filter should not contain the "WHERE" keyword.
 * Creates its own transaction.
 * \return int The counted items, -1 in case of an error.
*/
int Database::count(const int & tableref, const int & fieldref, const QString &filter) const
{
    int count = -1;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return count;
    DB.transaction();

    QString req = QString("SELECT count(`%1`) FROM `%2`").arg(d_database->m_Fields.value(d_database->index(tableref, fieldref))).arg(d_database->m_Tables[tableref]);
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            count = query.value(0).toInt();
        } else {
            LOG_QUERY_ERROR_FOR("Database", query);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
    }
    query.finish();
    (count==-1) ? DB.rollback() : DB.commit();
    return count;
}

/**
 * Executes a COUNT complex SQL command.
 * Creates its own transaction.
 * \return int The counted items, -1 in case of an error.
 * \sa select(), count(), countDistinct()
*/
int Database::count(const FieldList &select, const JoinList &joins, const FieldList &conditions) const
{
    int count = -1;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return count;
    DB.transaction();

    QString req = d_database->getSelectCommand(select, joins, conditions, DatabasePrivate::Count);
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            count = query.value(0).toInt();
        } else {
            LOG_QUERY_ERROR_FOR("Database", query);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
    }
    query.finish();
    (count==-1) ? DB.rollback() : DB.commit();
    return count;
}

/**
 * Executes a COUNT complex SQL command with a distinct clause on the field to retrieve.
 * Creates its own transaction.
 * \return int The counted items, -1 in case of an error.
 * \sa select(), count()
*/
int Database::countDistinct(const FieldList &select, const JoinList &joins, const FieldList &conditions) const
{
    int count = -1;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return count;
    DB.transaction();

    QString req = d_database->getSelectCommand(select, joins, conditions, DatabasePrivate::CountDistinct);
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            count = query.value(0).toInt();
        } else {
            LOG_QUERY_ERROR_FOR("Database", query);
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
    }
    query.finish();
    (count==-1) ? DB.rollback() : DB.commit();
    return count;
}

/**
 * Return a MAX SQL command on the table \e tableref, field \e fieldref with
 * the filter \e filter. Filter should not contain the "WHERE" keyword.
 * Creates its own transaction.
*/
QVariant Database::max(const int &tableref, const int &fieldref, const QString &filter) const
{
    QVariant toReturn;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return toReturn;
    DB.transaction();

    QString req = QString("SELECT max(%1) FROM %2")
            .arg(d_database->m_Fields.value(d_database->index(tableref, fieldref)))
            .arg(d_database->m_Tables[tableref]);
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            toReturn = query.value(0);
        } else {
            LOG_QUERY_ERROR_FOR("Database", query);
            query.finish();
            DB.rollback();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
        query.finish();
        DB.rollback();
    }
    query.finish();
    DB.commit();
    return toReturn;
}

/**
 * Return a MAX SQL command with grouping on the table \e tableref, field \e fieldref,
 * grouped by field \e groupBy with the filter \e filter.
 * Filter should not contain the "WHERE" keyword.
 * Creates its own transaction.
*/
QVariant Database::max(const int &tableref, const int &fieldref, const int &groupBy, const QString &filter) const
{
    QVariant toReturn;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return toReturn;
    DB.transaction();
    QString req = QString("SELECT max(%1) FROM %2 GROUP BY %3")
            .arg(d_database->m_Fields.value(d_database->index(tableref, fieldref)))
            .arg(d_database->m_Tables[tableref])
            .arg(d_database->m_Fields.value(d_database->index(tableref, groupBy)));
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            toReturn = query.value(0);
            DB.commit();
        } else {
            LOG_QUERY_ERROR_FOR("Database", query);
            DB.rollback();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
        DB.rollback();
    }
    return toReturn;
}

/**
 * Return a MIN SQL command with grouping on the table \e tableref, field \e fieldref,
 * grouped by field \e groupBy with the filter \e filter.
 * Filter should not contain the "WHERE" keyword.
 * Creates its own transaction.
*/
QVariant Database::min(const int &tableref, const int &fieldref, const QString &filter) const
{
    QVariant toReturn;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return toReturn;
    DB.transaction();

    QString req = QString("SELECT MIN(%1) FROM %2")
            .arg(d_database->m_Fields.value(d_database->index(tableref, fieldref)))
            .arg(d_database->m_Tables[tableref]);
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            toReturn = query.value(0);
            DB.commit();
        } else {
            LOG_QUERY_ERROR_FOR("Database", query);
            DB.rollback();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
        DB.rollback();
    }
    return toReturn;
}

/**
 * Return a MAX SQL command on the table \e tableref, field \e fieldref with
 * the filter \e filter. Filter should not contain the "WHERE" keyword.
*/
QString Database::maxSqlCommand(const int &tableref, const int &fieldref, const QString &filter) const
{
    QString req = QString("SELECT max(%1) FROM %2")
            .arg(d_database->m_Fields.value(d_database->index(tableref, fieldref)))
            .arg(d_database->m_Tables[tableref]);
    if (!filter.isEmpty())
        req += " WHERE " + filter;
    if (WarnSqlCommands)
        qWarning() << req;
    return req;
}

/**
 * Return a TOTAL SQL command on the table \e tableref, field \e fieldref with
 * the filter \e filter.
*/
QString Database::totalSqlCommand(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const
{
    QString toReturn;
    if (where.count()) {
        toReturn = QString("SELECT SUM(`%1`) FROM `%2` WHERE %3")
                .arg(d_database->m_Fields.value(d_database->index(tableRef, fieldRef)))
                .arg(d_database->m_Tables.value(tableRef))
                .arg(getWhereClause(tableRef, where));
    } else  {
        toReturn = QString("SELECT SUM(`%1`) FROM `%2`")
                .arg(d_database->m_Fields.value(d_database->index(tableRef, fieldRef)))
                .arg(d_database->m_Tables.value(tableRef));
    }
    return toReturn;
}

/** Return a TOTAL SQL command on the table \e tableref, field \e fieldref. */
QString Database::totalSqlCommand(const int tableref, const int fieldref) const
{
    QString toReturn;
    toReturn = QString("SELECT SUM(`%1`) FROM `%2`")
            .arg(d_database->m_Fields.value(d_database->index(tableref, fieldref)))
            .arg(d_database->m_Tables.value(tableref));
    return toReturn;
}

/**
 * Execute and return a SUM sql command on the table \e tableRef, field \e fieldRef, with
 * or without the \e where filter (which can be empty).
 * Creates its own transaction.
*/
double Database::sum(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const
{
    double toReturn = 0.0;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return toReturn;
    DB.transaction();

    QSqlQuery query(DB);
    QString req;
    if (where.isEmpty())
        req = totalSqlCommand(tableRef, fieldRef);
    else
        req = totalSqlCommand(tableRef, fieldRef, where);

    if (query.exec(req)) {
        if (query.next()) {
            toReturn = query.value(0).toDouble();
            DB.commit();
        } else {
            LOG_QUERY_ERROR_FOR("Database", query);
            DB.rollback();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Database", query);
        DB.rollback();
    }
    return toReturn;
}

/**
 * Execute and return a SUM sql command on the table \e tableRef and the field \e fieldRef.
 * Creates its own transaction.
*/
double Database::sum(const int tableRef, const int fieldRef) const
{
    QHash<int, QString> where;
    return sum(tableRef, fieldRef, where);
}

/** Return a SQL command usable for QSqlQuery::prepareInsertQuery(). Fields are ordered. */
QString Database::prepareInsertQuery(const int tableref) const
{
    QString toReturn;
    QString fields;
    QString values;
    QList<int> list = d_database->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int & i, list)
    {
        fields.append("`"+ d_database->m_Fields.value(i) + "`, ");
        values.append("? , ");
    }
    fields.chop(2);
    values.chop(2);
    toReturn = QString("INSERT INTO `%1` (%2) VALUES (%3)")
            .arg(table(tableref))
            .arg(fields)
            .arg(values);
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

QString Database::getLastExecutedQuery(const QSqlQuery& query)
{
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext())
    {
        it.next();
        str.replace(it.key(),it.value().toString());
    }
    return str;
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

/**
 * Create table \e tableref in the database according to its description.\n
 * Manages internal transaction.
 * \sa Utils::Database::addField(), Utils::Database::addTable(), Utils::Database::addIndex(), Utils::Database::addPrimaryKey(), Utils::Database::checkDatabaseScheme()
*/
bool Database::createTable(const int &tableref) const
{
    if (!d_database->m_Tables.contains(tableref))
        return false;
    if (! d_database->m_Tables_Fields.keys().contains(tableref))
        return false;
    if (d_database->m_ConnectionName.isEmpty())
        return false;

    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;
    bool insideTransaction = true;
    if (!d_database->_transaction) {
        DB.transaction();
        d_database->_transaction = true;
        insideTransaction = false;
    }

    // create query
    QStringList req;
    req = d_database->getSQLCreateTable(tableref);

    if (!executeSQL(req, DB)) {
        if (!insideTransaction) {
            d_database->_transaction = false;
            DB.rollback();
        }
        return false;
    } else {
        if (!insideTransaction) {
            d_database->_transaction = false;
            DB.commit();
        }
    }
    return true;
}

/**
 * Create all the tables in the database.\n
 * Manages internal transaction.
 * \sa Utils::Database::createTable(), Utils::Database::addField(), Utils::Database::addTable(), Utils::Database::addIndex(), Utils::Database::addPrimaryKey(), Utils::Database::checkDatabaseScheme()
*/
bool Database::createTables() const
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

    bool insideTransaction = true;
    if (!d_database->_transaction) {
        DB.transaction();
        d_database->_transaction = true;
        insideTransaction = false;
    }

    QList<int> list = d_database->m_Tables.keys();
    qSort(list);
    foreach(int i, list) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(!createTable(i)) {
            LOG_ERROR_FOR("Database", QCoreApplication::translate("Database", "Can not create table %1").arg(table(i)));
            if (!insideTransaction) {
                DB.rollback();
                d_database->_transaction = false;
            }
            return false;
        }
    }
    if (!insideTransaction) {
        DB.commit();
        d_database->_transaction = false;
    }
    return true;
}

/**
 * Alter an existing table adding a new field.\n
 * Manages internal transaction.
 * @param tableRef: table reference,
 * @param newFieldRef: "to create field" reference,
 * @param TypeOfField: Unused
 * @param nullOption: default null value. \warning nullOption must not include any 'DEFAULT', 'SET DEFAULT', 'DROP DEFAULT' sql commands.
 * @return true if the sql command was correctly executed
 * \sa Utils::Database::addTable(), Utils::Database::addField()
 * \note Fields are only added at the end of the table.
*/
bool Database::alterTableForNewField(const int tableRef, const int newFieldRef, const int TypeOfField, const QString &nullOption)
{
    Q_UNUSED(TypeOfField);
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;
    QString defaultSql;
    if (!nullOption.isEmpty()) {
        // TODO: manage NULL, numbers, string (that needs to be escaped)
        if (driver()==MySQL)
            defaultSql = QString("DEFAULT %1").arg(nullOption);
        else if (driver()==SQLite)
            defaultSql = QString("DEFAULT %1").arg(nullOption);
    } else {
        const QString &defaultValue = d_database->m_DefaultFieldValue.value(d_database->index(tableRef, newFieldRef));
        if (!defaultValue.isEmpty()) {
            // TODO: manage NULL, numbers, string (that needs to be escaped)
            if (driver()==MySQL)
                defaultSql = QString("DEFAULT %1").arg(defaultValue);
            else if (driver()==SQLite)
                defaultSql = QString("DEFAULT %1").arg(defaultValue);
        }
    }

    QString type = d_database->getTypeOfField(d_database->index(tableRef, newFieldRef));
    QString req;
    req = QString("ALTER TABLE `%1`"
                  "  ADD `%2` %3 %4;")
            .arg(table(tableRef), fieldName(tableRef, newFieldRef), type, defaultSql);

    if (WarnSqlCommands)
        qWarning() << req;

    DB.transaction();
    QSqlQuery query(DB);
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        LOG_FOR("Database",QString("Unable to add the fields %1").arg(fieldName(tableRef, newFieldRef)));
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/**
 * Modify the type of an existing column of a table in a MySQL database
 * Default value will be set automatically by MySQL
 */
bool Database::modifyMySQLColumnType(const int & tableref, const int & fieldref)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;
    QString req;
    QString newType = d_database->getTypeOfField(d_database->index(tableref, fieldref));
    req = QString("ALTER TABLE `%1` MODIFY COLUMN `%2` %3;")
            .arg(table(tableref), fieldName(tableref, fieldref), newType);
    DB.transaction();
    QSqlQuery query(DB);
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        //LOG_FOR("Database", QString("Unable to modify column %1 type to %2").arg(fieldName(tableref, fieldref), type));
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/**                                                                             
 * Modify the type of an existing column of a table in a MySQL database
 * Default value is set by QString & defaultValue
 */
bool Database::modifyMySQLColumnType(const int & tableref, const int & fieldref,
                                     const QString & defaultValue)
{                                                                               
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;
    QString newType = d_database->getTypeOfField(d_database->index(tableref, fieldref));
    QString req;
    // Handle default value
    req = QString("ALTER TABLE `%1` MODIFY COLUMN `%2` %3 DEFAULT %4;")
            .arg(table(tableref)).arg(fieldName(tableref, fieldref)).arg(newType).arg(defaultValue);
    DB.transaction();
    QSqlQuery query(DB);
    if (!query.exec(req)) {
        LOG_QUERY_ERROR_FOR("Database", query);
        //LOG_FOR("Database", QString("Unable to modify column %1 type to %2").arg(fieldName(tableref, fieldref), type));
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/**
 * @brief mySQLUserHostNames return a QStringList list of host names correponding to a MySQL user name
 * @param userName is the MySQL user name (it should be identical to FMF login)
 * @return
 */
QStringList Database::mySQLUserHostNames(const QString & userName)

{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return QStringList();
    QString req;
    req = QString("SELECT Host FROM mysql.user where user= '%1';").arg(userName);
    DB.transaction();

    QStringList hostNames;
    QSqlQuery query(req, DB);
    if (!query.isActive()) {
        LOG_ERROR_FOR("Database", "No host name for user?");
        LOG_QUERY_ERROR_FOR("Database", query);
        DB.rollback();
        return QStringList();
    } else {
        while (query.next()) {
            hostNames << query.value(0).toString();
        }
    }
    query.finish();
    DB.commit();
    qWarning() << "mySQLUserHostNames() -> hostNames:" << hostNames;
    return hostNames;
}

/**
 * Vacuum the database (for SQLite only). Execute the 'VACUUM' sql command on the database.
 * \warning SQLite can not vacuum inside a transaction. Be sure that the database is not
 * in transaction.
 */
bool Database::vacuum(const QString &connectionName)
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!connectedDatabase(DB, __LINE__))
        return false;
    QSqlQuery query(DB);
    if (!query.exec("VACUUM")) {
        LOG_QUERY_ERROR_FOR("Database", query);
        return false;
    }
    return true;
}

/**
 * Execute simple SQL commands on the QSqlDatabase \e DB. Return \e true if all was fine.\n
 * Creates a transaction on the database \e DB. \n
 * Lines starting with \e -- are ignored.
*/
bool Database::executeSQL(const QStringList &list, QSqlDatabase &DB)
{
    if (!connectedDatabase(DB, __LINE__))
        return false;
    DB.transaction();
    QSqlQuery query(DB);
    foreach(QString r, list) {
        r = r.trimmed();
        //ignore empty lines
        if (r.isEmpty())
            continue;

        // ignore comments starting with "--"
        if (r.startsWith("--")) {
            continue;
        }

        // No SQLite extra commands
        if (r.startsWith("."))
            continue;

        // No BEGIN, No COMMIT
        if (r.startsWith("BEGIN", Qt::CaseInsensitive)
                || r.startsWith("COMMIT", Qt::CaseInsensitive))
            continue;

        if (!query.exec(r)) {
            LOG_QUERY_ERROR_FOR("Database", query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();
    }
    DB.commit();
    return true;
}

/**
 * Execute SQL commands on the QSqlDatabase \e DB. \n
 * \warning All SQL commands must be separated by a \e ; followed by a linefeed. \n
 * Creates a transaction on the database \e DB. \n
 * \warning The string is split with the ; and line feed. All lines starting with
 * \e -- are ignored. Remember to add a ; at the end of your comment lines.
*/
bool Database::executeSQL(const QString &req, QSqlDatabase &DB)
{
    if (req.isEmpty())
        return false;
    if (!connectedDatabase(DB, __LINE__))
        return false;
    // TODO: manage ; inside "" or ''
    QStringList list = req.trimmed().split(";\n", QString::SkipEmptyParts);
    return executeSQL(list, DB);
}

/**
 * Execute simple SQL commands stored in a file on the QSqlDatabase connected as \e connectionName.
 * Line starting with '-- ' are ignored.\n
 * All SQL commands must end with a \e ; followed by a linefeed. \n
 * Creates a transaction on the database \e DB. \n
 * Files content \b must \b be \b utf8 encoded.
 * \sa executeSQL(), importCsvToDatabase()
*/
bool Database::executeSqlFile(const QString &connectionName, const QString &fileName, QString *error)
{
    if (error)
        error->clear();

    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!connectedDatabase(DB, __LINE__)) {
        if (error)
            error->append(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(DB.connectionName()).arg(DB.lastError().text()));
        return false;
    }

    if (!QFile::exists(fileName)) {
        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
        if (error)
            error->append(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR_FOR("Database", tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(fileName));
        if (error)
            error->append(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(fileName));
        return false;
    }

    // execute all sql queries
    QString req = QString::fromUtf8(file.readAll());
    return executeSQL(req, DB);
}

/**
 * @brief Database::executeQueryFile
 * @param file
 * @param db
 * TODO: test & use Utils::Database::executeSqlFile()
 * @return
 */
bool Database::executeQueryFile(QFile &file, QSqlDatabase &db)
{
    WARN_FUNC;
    QSqlQuery query(db);
    //Read query file content
    file.open(QIODevice::ReadOnly);
    QString queryStr(file.readAll());
    file.close();

    //Check if SQL Driver supports Transactions
    if(db.driver()->hasFeature(QSqlDriver::Transactions)) {
        //Replace comments and tabs and new lines with space
        queryStr = queryStr.replace(QRegularExpression("(\\/\\*(.|\\n)*?\\*\\/|^--.*\\n|\\t|\\n)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::MultilineOption), " ");
        //Remove waste spaces
        queryStr = queryStr.trimmed();

        //Extracting queries
        QStringList qList = queryStr.split(';', QString::SkipEmptyParts);

        //Initialize regular expression for detecting special queries (`begin transaction` and `commit`).
        //NOTE: I used new regular expression for Qt5 as recommended by Qt documentation.
        QRegularExpression re_transaction("\\bbegin.transaction.*", QRegularExpression::CaseInsensitiveOption);
        QRegularExpression re_commit("\\bcommit.*", QRegularExpression::CaseInsensitiveOption);

        //Check if query file is already wrapped with a transaction
        bool isStartedWithTransaction = re_transaction.match(qList.at(0)).hasMatch();
        if(!isStartedWithTransaction) {
            db.transaction();     //<=== not wrapped with a transaction, so we wrap it with a transaction.
        }
        //Execute each individual queries
        foreach(const QString &s, qList) {
            if (re_transaction.match(s).hasMatch()) {    //<== detecting special query
                db.transaction();
            }
            else if(re_commit.match(s).hasMatch()) {    //<== detecting special query
                db.commit();
            } else {
                query.exec(s);                        //<== execute normal query
                if(query.lastError().type() != QSqlError::NoError) {
                    LOG_QUERY_ERROR_FOR(file.fileName(), query);
                    db.rollback();                    //<== rollback the transaction if there is any problem
                }
            }
        }
        if (!isStartedWithTransaction) {
            db.commit();          //<== ... completing of wrapping with transaction
        }

        //Sql Driver doesn't supports transaction
    } else {

        //...so we need to remove special queries (`begin transaction` and `commit`)
        queryStr = queryStr.replace(QRegularExpression("(\\bbegin.transaction.*;|\\bcommit.*;|\\/\\*(.|\\n)*?\\*\\/|^--.*\\n|\\t|\\n)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::MultilineOption), " ");
        queryStr = queryStr.trimmed();

        //Execute each individual queries
        QStringList qList = queryStr.split(';', QString::SkipEmptyParts);
        foreach(const QString &s, qList) {
            query.exec(s);
            if(query.lastError().type() != QSqlError::NoError) {
                LOG_QUERY_ERROR_FOR(file.fileName(), query);
            }
        }
    }
    return true;
}

/**
 * Import a CSV structured file \e fileName into a database \e connectionName,
 * table \e table, using the specified \e separator, and \e ignoreFirstLine or not.\n
 * Creates a transaction on the database.
*/
bool Database::importCsvToDatabase(const QString &connectionName, const QString &fileName, const QString &table, const QString &separator, bool ignoreFirstLine)
{
    // get database
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!connectedDatabase(DB, __LINE__))
        return false;
    DB.transaction();

    // get table field's name
    if (!DB.tables().contains(table)) {
        LOG_ERROR_FOR("Database", "No table found");
        DB.rollback();
        return false;
    }

    QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);
    if (content.isEmpty())
        return false;
    QStringList lines = content.split("\n", QString::SkipEmptyParts);
    content.clear();
    int start = 0;
    if (ignoreFirstLine)
        start = 1;

    // prepare the sql query
    QSqlRecord record = DB.record(table);
    QString req = "INSERT INTO " + table + " (\n";
    for(int i = 0; i < record.count(); ++i) {
        req += "`" + record.fieldName(i) + "`, ";
    }
    req.chop(2);
    req += ")\n VALUES (";

    QSqlQuery query(DB);
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
                // TODO: this needs some improvements (string, numbers, " chars...)
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
        if (!query.exec(req + reqValues)) {
            LOG_QUERY_ERROR_FOR("Database", query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();
    }
    DB.commit();
    return true;
}

/** Create the SQL command for the table creation \e tableref. \sa addTable(), addField() */
QStringList DatabasePrivate::getSQLCreateTable(const int &tableref)
{
    QString toReturn;
    toReturn = QString("CREATE TABLE IF NOT EXISTS `%1` (").arg(m_Tables.value(tableref));
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
                fieldLine.append(QString("%1 %2")
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
            case Database::FieldIsTwoChars :
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
                        // When we switch to >=5.6.5 we will be able to use DATETIME:
                        // "The exception is that you can specify CURRENT_TIMESTAMP
                        // as the default for a TIMESTAMP
                        // or (as of MySQL 5.6.5) DATETIME column."
                        defVal = "NULL";
                    } else if (defVal.endsWith("()")) {
                        defVal = defVal.remove("()");
                    }
                    fieldLine.append(QString("%1 %2 DEFAULT %3")
                                     .arg(fieldName)
                                     .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                     .arg(defVal));
                } else {
                    fieldLine.append(QString("%1 %2 DEFAULT '%3'")
                                     .arg(fieldName)
                                     .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                     .arg(m_DefaultFieldValue.value(i)));
                }
                break;
            }
            case Database::FieldIsBoolean :
            case Database::FieldIsInteger :
            case Database::FieldIsLongInteger :
            case Database::FieldIsUnsignedInteger:
            case Database::FieldIsUnsignedLongInteger:
            case Database::FieldIsReal :
            case Database::FieldIsTimeStamp :
            {
                fieldLine.append(QString("%1 %2 NULL DEFAULT %3")
                                 .arg(fieldName)
                                 .arg(getTypeOfField(i)).toUpper()// .leftJustified(20, ' '))
                                 .arg(m_DefaultFieldValue.value(i)));
                break;
            }
            case Database::FieldIsIsoUtcDateTime :
                fieldLine.append(QString("%1 %2")
                                 .arg(fieldName)
                                 .arg(getTypeOfField(i)));
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
    toReturn.append(fieldLine.join(", "));

    foreach(int field, m_PrimKeys.values(tableref)) {
        int ref = index(tableref, field);
        if (m_TypeOfField.value(ref) != Database::FieldIsUniquePrimaryKey) {
            toReturn.append(QString(", PRIMARY KEY(%1)").arg(m_Fields.value(ref)));
        }
    }
    toReturn.append(");");

    QStringList indexes;
    QString prefix;
    for(int i = 0; i < m_DbIndexes.count(); ++i) {
        const DbIndex &idx = m_DbIndexes.at(i);
        if (idx.prefix > 0 && m_Driver==Database::MySQL) {
            prefix = QString("(%1)").arg(idx.prefix);
        } else {
            prefix.clear();
        }
        if (idx.field.table==tableref) {
            indexes << QString("CREATE INDEX %1 ON %2 (%3%4);")
                       .arg(idx.name)
                       .arg(idx.field.tableName)
                       .arg(idx.field.fieldName)
                       .arg(prefix);
        }
    }

    if (WarnCreateTableSqlCommand)
        qWarning() << toReturn << "Indexes: " << indexes;

    return QStringList() << toReturn << indexes;
}

/** Return the SQL type of field for the field \e fieldref.*/
QString DatabasePrivate::getTypeOfField(const int &fieldref) const
{
    QString toReturn;
    switch (Database::TypeOfField(m_TypeOfField.value(fieldref)))
    {
    case Database::FieldIsUUID :
        toReturn = "VARCHAR(32)";
        break;
    case Database::FieldIsBoolean :
        toReturn = "INT(1)";
        break;
    case Database::FieldIsLongText :
        toReturn = "VARCHAR(2000)";
        break;
    case Database::FieldIsShortText :
        toReturn = "VARCHAR(200)";
        break;
    case Database::FieldIsTwoChars :
        toReturn = "VARCHAR(2)";
        break;
    case Database::FieldIsLanguageText :
        toReturn = "VARCHAR(2)";
        break;
    case Database::FieldIs19Chars :
        toReturn = "VARCHAR(19)";
        break;
    case Database::FieldIs32Chars :
        toReturn = "VARCHAR(32)";
        break;
    case Database::FieldIs255Chars :
        toReturn = "VARCHAR(255)";
        break;
    case Database::FieldIsBlob :
        if (m_Driver==Database::SQLite) {
            toReturn = "BLOB"; // 1,000,000,000 max size
        } else if (m_Driver==Database::MySQL) {
            toReturn = "LONGBLOB"; // 4Go max size
        }
        break;
    case Database::FieldIsDate :
        toReturn = "DATE";
        break;
    case Database::FieldIsTime:
        toReturn = "TIME";
        break;
    case Database::FieldIsDateTime:
        toReturn = "DATETIME";
        break;
    case Database::FieldIsTimeStamp:
        toReturn = "TIMESTAMP NULL";
        break;
    case Database::FieldIsIsoUtcDateTime:
        toReturn = "VARCHAR(20)";
        break;
    case Database::FieldIsOneChar :
        toReturn = "VARCHAR(1)";
        break;
    case Database::FieldIsInteger :
        toReturn = "INTEGER";
        break;
    case Database::FieldIsUniquePrimaryKey :
        if (m_Driver==Database::SQLite) {
            toReturn = "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT";
        } else if (m_Driver==Database::MySQL) {
            toReturn = "INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT";
        }
        break;
    case Database::FieldIsLongInteger :
        toReturn = "INT(11)";
        break;
    case Database::FieldIsUnsignedInteger:
        toReturn = "INTEGER UNSIGNED";
        break;
    case Database::FieldIsUnsignedLongInteger:
        if (m_Driver==Database::SQLite) {
            toReturn = "UNSIGNED BIGINT";
        } else if (m_Driver==Database::MySQL) {
            toReturn = "BIGINT UNSIGNED";
        }
        break;
    case Database::FieldIsReal :
        toReturn = "DOUBLE";
        break;
    default : toReturn = QString::null; break;
    }
    return toReturn;
}

/**  Used for debugging and information purpose. */
void Database::toTreeWidget(QTreeWidget *tree) const
{
    QFont bold;
    bold.setBold(true);
    tree->clear();

    QSqlDatabase DB = QSqlDatabase::database(d_database->m_ConnectionName);

    // General information
    QTreeWidgetItem *db = new QTreeWidgetItem(tree, QStringList() << "General information");
    db->setFont(0, bold);
    db->setFirstColumnSpanned(true);
    new QTreeWidgetItem(db, QStringList() << "Connection Name" << d_database->m_ConnectionName);
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
    drv->setFirstColumnSpanned(true);
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

    // Grants
    QTreeWidgetItem *grants = new QTreeWidgetItem(tree, QStringList() << "Grants");
    grants->setFont(0, bold);
    grants->setFirstColumnSpanned(true);
    Database::Grants g = d_database->m_Grants.value(d_database->m_ConnectionName);
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

QDebug operator<<(QDebug dbg, const Utils::Database &database)
{
    QSqlDatabase DB = database.database();
    QString msg = "Database(";
    msg += QString("connection: %1, name: %2, driver: %3, open: %4, canOpen: %5")
            .arg(DB.connectionName())
            .arg(DB.databaseName())
            .arg(DB.driverName())
            .arg(DB.isOpen())
            .arg(DB.open());
    for(int i = 0; i>=0; ++i) {
        const QString &table = database.table(i);
        if (table.isNull())
            break;
        msg += QString("\n          table: %1").arg(table);
        for(int j=0; j>=0; ++j) {
            const QString &field = database.field(i, j).fieldName;
            if (field.isNull())
                break;
            msg += QString("\n            field: %1").arg(field);
        }
    }
    // TODO: improve this: add a complet sqldatabase schema of the database
    dbg.nospace() << msg;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Utils::Database *database)
{
    return operator<<(dbg, *database);
}
