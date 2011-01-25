/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Utils::Database
 \brief this class is a base class for databases. It manages scheme and creates SQL queries.
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

#include <translationutils/constanttranslations.h>

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

enum { WarnSqlCommands=false , WarnLogMessages = false };

using namespace Utils;
using namespace Utils::Internal;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {
class DatabasePrivate
{
public:
    DatabasePrivate();
    ~DatabasePrivate() {}
    QString getSQLCreateTable(const int & tableref);
    QString getTypeOfField(const int & fieldref) const;

    void getGrants(const QString &connection, const QStringList &grants)
    {
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
        m_Grants.insert(connection, g);
//        qWarning() << g;
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
    Utils::Log::addMessage("Database", QString("Available drivers: %1").arg(QSqlDatabase::drivers().join(" ; ")));
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
/**
  \brief Return the pointer to the QSqlDatabase in use.
*/
QSqlDatabase Database::database() const
{ return QSqlDatabase::database(d->m_ConnectionName); }

/**
  \brief Create the connection to the database.
  If database does not exists, according to the \e createOption, createDatabase() is called.
  An error is returned if :
  - Driver is not available
  - Can not connect to server (wrong host/log/pass)
  - Can not create database is it doesn't exists and user asked to create it
  - Can not read database if asked to be readable
  - Can not write in database if asked to be writable
  \param connectionName = name of the connect
  \param dbName = name of the database
  \param pathOrHostName = path to the SQLite file or name of the host name to connect to.
  \param acces = type of acces requiered.
  \param driver = type of driver requiered
  \param login = login to the server (not used for SQLite)
  \param password = login to the server (not used for SQLite)
  \param createOption = what to do if the database does not exist.
*/
bool Database::createConnection(const QString & connectionName, const QString & dbName,
                                   const QString & pathOrHostName,
                                   TypeOfAccess access, AvailableDrivers driver,
                                   const QString &login, const QString &password,
                                   const int port,
                                   CreationOption createOption
                                   )
{   
    if (WarnLogMessages)
        qDebug() << __FILE__ << QString::number(__LINE__) << connectionName
                << dbName
                <<pathOrHostName
                << access
                << driver
                << login
                << password
                << QString::number(port) ;
    bool toReturn = true;
    d->m_ConnectionName = "";

    // does driver is available
    switch (driver) {
    case SQLite :
        {
            if (WarnLogMessages)
                Utils::Log::addMessage("Database", QString("Trying to connect database %1 with %2 driver")
                                   .arg(dbName)
                                   .arg("SQLite"));
            if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
                Log::addError("Database", QCoreApplication::translate("Database",
                                                                      "ERROR : %1 driver is not available")
                              .arg("SQLite"), __FILE__, __LINE__);
                return false;
            }
            break;
        }
    case MySQL:
        {
            if (WarnLogMessages)
                Utils::Log::addMessage("Database", QString("Trying to connect database %1 with %2 driver")
                                   .arg(dbName)
                                   .arg("MySQL"));
            if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
                Log::addError("Database", QCoreApplication::translate("Database",
                                                                      "ERROR : %1 driver is not available")
                              .arg("MySQL"), __FILE__, __LINE__);
                return false;
            }
            break;
        }
    case PostSQL :
        {
            if (WarnLogMessages)
                Utils::Log::addMessage("Database", QString("Trying to connect database %1 with %2 driver")
                                   .arg(dbName)
                                   .arg("PostGre SQL"));
            if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
                Log::addError("Database", QCoreApplication::translate("Database",
                                                                      "ERROR : %1 driver is not available")
                              .arg("PostGreSQL"), __FILE__, __LINE__);
                return false;
            }
        }
    }

    // does connection already exists ?
    if (QSqlDatabase::contains(connectionName)) {
        if (WarnLogMessages)
            Log::addMessage("Database", QCoreApplication::translate("Database",
                                                                "WARNING : %1 database already in use")
                        .arg(connectionName));
        d->m_ConnectionName = connectionName;
        return true;
    }

    QSqlDatabase DB;
    QString fileName = pathOrHostName + QDir::separator() + dbName;

    // check server connection
    switch (driver) {
    case SQLite: break;
    case MySQL:
        {
            DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
            DB.setHostName(pathOrHostName);
            DB.setUserName(login);
            DB.setPassword(password);
            DB.setPort(port);
            bool ok = DB.open();
            if (!ok) {
                Utils::Log::addError("Database", QString("Unable to connect to the server %1 - %2")
                                     .arg(pathOrHostName).arg(DB.lastError().text()),
                                     __FILE__, __LINE__);
                return false;
            }
            if (WarnLogMessages)
                Utils::Log::addMessage("Database", QString("Connected to host %1").arg(pathOrHostName));
            break;
        }
    case PostSQL:
        {
            /** \todo Check PostSQL connection */
            break;
        }
    }

    // create database is not exists and user ask for database creation
    switch (driver) {
    case SQLite:
        {
            if ((!QFile(fileName).exists()) ||
                (QFileInfo(fileName).size() == 0)) {
                if (createOption == CreateDatabase) {
                    if (!createDatabase(connectionName, dbName, pathOrHostName, access, driver, login, password, port, createOption)) {
                        Log::addError("Database", QCoreApplication::translate("Database",
                                                                              "ERROR : %1 database does not exist and can not be created. Path = %2").arg(dbName, pathOrHostName), __FILE__, __LINE__);
                        return false;
                    }
                } else { // Warn Only
                    if (WarnLogMessages)
                        Log::addMessage("Database", QCoreApplication::translate("Database",
                                                                            "ERROR : %1 database does not exist and can not be created. Path = %2").arg(dbName, pathOrHostName));
                    return false;
                }
            }
            break;
        }
    case MySQL:
        {
            // can get connection to server ?
            DB.setDatabaseName(dbName);
            bool ok = DB.open();
            if (!ok) {
                Utils::Log::addError("Database", QString("Unable to connect to the database %1 - %2")
                                     .arg(dbName).arg(DB.lastError().text()),
                                     __FILE__, __LINE__);
                if (createOption == CreateDatabase) {
                    if (!createDatabase(connectionName, dbName, pathOrHostName, access, driver, login, password, port, createOption)) {
                        Log::addError("Database", QCoreApplication::translate("Database",
                                      "ERROR : %1 database does not exist and can not be created. Path = %2")
                                      .arg(dbName, pathOrHostName), __FILE__, __LINE__);
                        return false;
                    }
                } else { // Warn Only
                    if (WarnLogMessages)
                        Log::addMessage("Database", QCoreApplication::translate("Database",
                                        "ERROR : %1 database does not exist and can not be created. Path = %2")
                                        .arg(dbName, pathOrHostName));
                    return false;
                }
            }
            if (WarnLogMessages)
                Utils::Log::addMessage("Database", QString("Connected to database %1").arg(dbName));
            break;
        }
    case PostSQL:
        {
            /** \todo Test database existence */
            break;
        }
    }

    // test read access to database
    switch (driver) {
    case SQLite:
        {
            if (!QFileInfo(fileName).isReadable()) {
                Log::addError("Database", QCoreApplication::translate("Database", "ERROR : Database %1 is not readable. Path : %2")
                              .arg(dbName, pathOrHostName), __FILE__, __LINE__);
                toReturn = false;
            }
            break;
        }
    case MySQL:
        {
            if (!DB.open()) {
                Log::addError("Database", QCoreApplication::translate("Database",
                              "ERROR : Database %1 is not readable. Path : %2")
                              .arg(dbName, pathOrHostName), __FILE__, __LINE__);
                return false;
            }
            QSqlQuery query("SHOW GRANTS FOR CURRENT_USER;", DB);
            if (!query.isActive()) {
                Log::addError("Database", QCoreApplication::translate("Database",
                              "ERROR : Database %1 is not readable. Path : %2")
                              .arg(dbName, pathOrHostName), __FILE__, __LINE__);
                Log::addQueryError("Database", query);
                return false;
            } else {
                QStringList grants;
                while (query.next()) {
                    grants << query.value(0).toString();
                }
                d->getGrants(connectionName, grants);
//                qWarning() << grants;
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
    if (access == ReadWrite) {
        switch (driver) {
        case SQLite:
            {
                if (!QFileInfo(fileName).isWritable()) {
                    Log::addError("Database", QCoreApplication::translate("Database",
                                  "ERROR : Database %1 is not writable. Path : %2.")
                                  .arg(dbName, pathOrHostName), __FILE__, __LINE__);
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
    switch (driver)
    {
        case SQLite :
        {
             DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
             DB.setDatabaseName(QDir::cleanPath(fileName));
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
        Log::addMessage("Database", QCoreApplication::translate("Database",  "INFO : database %1 connection = %2")
                       .arg(connectionName).arg(DB.isOpen()));

    // test connection
    if (!DB.isOpen()) {
        Log::addError("Database", QCoreApplication::translate("Database",
                                                                    "WARNING : can not open database %1 : %2 \n %3 ")
                         .arg(connectionName, DB.lastError().driverText(), DB.lastError().databaseText()), __FILE__, __LINE__);
        toReturn = false;
    }
    else {
        if (WarnLogMessages)
            Log::addMessage("Database", QCoreApplication::translate("Database", "INFO : database %1 installed. Path : %2")
                           .arg(connectionName, pathOrHostName));
    }
    // return boolean
    if (toReturn)
        d->m_ConnectionName = connectionName;
    return toReturn;
}

/** \brief Returns the connectionName in use */
QString Database::connectionName() const
{
    return d->m_ConnectionName;
}

/** \brief returns the grants according to the database \e connectionName. When using a SQLite driver Grants always == 0. */
Database::Grants Database::grants(const QString &connectionName) const
{
    return d->m_Grants.value(connectionName, 0);
}

/** \brief Set connectionName to \e c */
void Database::setConnectionName(const QString & c)
{ d->m_ConnectionName = c; }

/** \brief Define the driver to use */
void Database::setDriver(const Database::AvailableDrivers &drv)
{ d->m_Driver = drv; }

/** \brief Add a table \e name to the database scheme with the index \e ref */
int Database::addTable(const int & ref, const QString & name)
{
    d->m_Tables.insert(ref, name);
    return d->m_Tables.key(name);
}

/** \brief Add a field \e name to the database scheme with the index \e fieldref into table indexed \e tableref.\n
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

/** \brief Verify that the dynamically scheme passed is corresponding to the real database scheme. */
bool Database::checkDatabaseScheme()
{
    if (d->m_ConnectionName.isEmpty())
        return false;
    if (d->m_Tables.keys().count() == 0)
        return false;
    if (d->m_Tables_Fields.keys().count() == 0)
        return false;

    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);
    DB.open();

    QList<int> list = d->m_Tables.keys();
    qSort(list);
    foreach(int i, list) {
        QSqlRecord rec = DB.record(d->m_Tables.value(i));
        if (rec.count() != d->m_Tables_Fields.values(i).count()) {
            Log::addError("Database", QCoreApplication::translate("Database", "Database Scheme Error : wrong number of fields for table %1")
                                   .arg(d->m_Tables.value(i)), __FILE__, __LINE__);
            return false;
        }
        QList<int> fields = d->m_Tables_Fields.values(i);
        qSort(fields);
        int id = 0;
        foreach(int f, fields) {
            if (d->m_Fields.value(f)!= rec.field(id).name()) {
                Log::addError("Database", QCoreApplication::translate("Database", "Database Scheme Error : field number %1 differs : %2 instead of %3 in table %4")
                                   .arg(id).arg(d->m_Fields.value(f), rec.field(id).name(), d->m_Tables.value(i)), __FILE__, __LINE__);
                return false;
            }
            id++;
        }
    }
    return true;
}

/**
   \brief Return the field name for the table \e tableref field \e fieldref
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
   \brief Return a complete reference to a field for the table \e tableref field \e fieldref
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

/** \brief Return all fields of a table as \e FieldList */
FieldList Database::fields(const int tableref) const
{
    FieldList ret;
    if (!d->m_Tables.contains(tableref))
        return ret;
    if (!d->m_Tables_Fields.keys().contains(tableref))
        return ret;

    QList<int> fieldRefs = d->m_Tables_Fields.values(tableref);
    qSort(fieldRefs);
    QString tableName = table(tableref);
    foreach(int i, fieldRefs) {
        Field f;
        f.table = tableref;
        f.field = i;
        f.tableName = tableName;
        f.fieldName = fieldName(tableref, i);
        ret << f;
   }
    return ret;
}

/** \brief Return all fields name of a table */
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
  \brief Create a where clause on the table \e tableref using conditions mapped into a hash.
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
  \brief Create a where clause on the \e fields.
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

        where.append(QString(" (`%1`.`%2` %3) AND ")
                      .arg(tab)
                      .arg(f)
                      .arg(fields.at(i).whereCondition));
    }
    where.chop(5);
    if (fields.count() > 1)
        where = QString("(%1)").arg(where);
    if (WarnSqlCommands)
        qWarning() << where;
    return where;
}

/**
  \brief Create a join statement on \e join.field1.tableName using fields equality.
  \code
    Join join(t1, f1, t2, f2, joinType);
    QString sqlJoin = db.join(join);
    // will return: JOIN T1NAME ON T1NAME.F1NAME=T2NAME.F2NAME
  \endcode
*/
QString Database::join(const Join &join) const
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
    s += join.field1.tableName + " ON " ;
    s += QString("`%1`.`%2`=`%3`.`%4` ")
         .arg(join.field1.tableName, join.field1.fieldName)
         .arg(join.field2.tableName, join.field2.fieldName);
    return s;
}

/**
  \brief Return a SELECT SQL command with the table \e tableref, field \e fieldref and where conditions \e conditions.
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
  \brief Return a SELECT SQL command with the table \e tableref, field \e fieldref.
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
  \brief Return a SELECT SQL command.
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
  \brief Return a SELECT SQL command.
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
  \brief Return a SELECT SQL command for the table \e tableref with conditions \e conditions.
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
  \brief Return a SELECT SQL command for the table \e tableref.
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
  \brief Return a SELECT DISTINCT SQL command.
  \sa select()
*/
QString Database::selectDistinct(const int & tableref, const int & fieldref, const QHash<int, QString> & conditions) const
{
    return select(tableref, fieldref, conditions).replace("SELECT", "SELECT DISTINCT");
}

/**
  \brief Return a SELECT DISTINCT SQL command.
  \sa select()
*/
QString Database::selectDistinct(const int & tableref, const int & fieldref) const
{
    return select(tableref, fieldref).replace("SELECT", "SELECT DISTINCT");
}

/**
  \brief Create a complex SELECT command with jointures and conditions.
  Jointures must be ordered as needed in the SQL command.
*/
QString Database::select(const FieldList &select, const JoinList &joins, const FieldList &conditions) const
{
    QString fields, from;
    QStringList tables;
    // calculate fields
    for(int i=0; i < select.count(); ++i) {
        fields += QString("`%1`.`%2`, ").arg(select.at(i).tableName).arg(select.at(i).fieldName);
        tables << select.at(i).tableName;
    }
    if (fields.isEmpty())
        return fields;
    fields.chop(2);
    tables.removeDuplicates();

    // Calculate conditions
    QString w = getWhereClause(conditions);
    for(int i=0; i < conditions.count(); ++i) {
        tables.removeAll(conditions.at(i).tableName);
    }

    // Calculate joins
    QString j;
    for(int i=0; i < joins.count(); ++i) {
        j += join(joins.at(i)) + "\n";
        tables.removeAll(joins.at(i).field1.tableName);
    }

    foreach(const QString &tab, tables) {
        from += QString("`%1`, ").arg(tab);
    }
    from.chop(2);

    return QString("SELECT %1 FROM %2 \n %3 WHERE %4").arg(fields, from, j, w);
}

/**
  \brief Return a SELECT SQL structured field equality.
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
  \brief Return a a COUNT SQL command on the table \e tableref, field \e fieldref with the filter \e filter.
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
            Utils::Log::addQueryError("Database", q);
        }
    } else {
        Utils::Log::addQueryError("Database", q);
    }
    return -1;
}

/**
  \brief Return a MAX SQL command on the table \e tableref, field \e fieldref with the filter \e filter.
  Filter whould be not contains the "WHERE" word.
*/
double Database::max(const int &tableref, const int &fieldref, const QString &filter) const
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
            return q.value(0).toDouble();
        } else {
            Utils::Log::addQueryError("Database", q);
        }
    } else {
        Utils::Log::addQueryError("Database", q);
    }
    return 0;
}

/**
  \brief Return a MAX SQL command with grouping on the table \e tableref, field \e fieldref, grouped by field \e groupBy with the filter \e filter.
  Filter whould be not contains the "WHERE" word.
*/
double Database::max(const int &tableref, const int &fieldref, const int &groupBy, const QString &filter) const
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
            return q.value(0).toDouble();
        } else {
            Utils::Log::addQueryError("Database", q);
        }
    } else {
        Utils::Log::addQueryError("Database", q);
    }
    return 0;
}

/**
  \brief Return a TOTAL SQL command on the table \e tableref, field \e fieldref with the filter \e filter.
  Filter whould be not contains the "WHERE" word.
*/
QString Database::total(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const
{
    QString toReturn;
    if (where.count()) {
        toReturn = QString("SELECT total(`%1`) FROM `%2` WHERE %3")
                   .arg(d->m_Fields.value(fieldRef + tableRef * 1000))
                   .arg(d->m_Tables.value(tableRef))
                   .arg(getWhereClause(tableRef, where));
    } else  {
        toReturn = QString("SELECT total(`%1`) FROM `%2`")
                   .arg(d->m_Fields.value(fieldRef + tableRef * 1000))
                   .arg(d->m_Tables.value(tableRef));
    }
    return toReturn;
}

/**  \brief Return a TOTAL SQL command on the table \e tableref, field \e fieldref. */
QString Database::total(const int tableRef, const int fieldRef) const
{
    QString toReturn;
    toReturn = QString("SELECT total(`%1`) FROM `%2`")
               .arg(d->m_Fields.value(fieldRef + tableRef * 1000))
               .arg(d->m_Tables.value(tableRef));
    return toReturn;
}

/**  \brief Return a SQL command usable for QSqlQuery::prepareInsertQuery(). Fields are ordered. */
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

/**  \brief Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
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

/**  \brief Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
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

/**  \brief Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
QString Database::prepareUpdateQuery(const int tableref, const int fieldref)
{
    QString toReturn;
    toReturn = QString("UPDATE `%1` SET `%2` = ?")
               .arg(table(tableref))
               .arg(fieldName(tableref, fieldref));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/** \brief Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
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

/** \brief Return a SQL command usable for QSqlQuery::prepareUpdateQuery(). Fields are ordered. */
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

/**  \brief Return a SQL command usable for QSqlQuery::prepareDeleteQuery(). Fields are ordered. */
QString Database::prepareDeleteQuery(const int tableref, const QHash<int,QString> & conditions)
{
    QString toReturn;
    toReturn = QString("DELETE FROM `%1` \n WHERE %2")
               .arg(table(tableref))
               .arg(getWhereClause(tableref, conditions));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

/**  \brief Create table \e tableref in the database. */
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
    if (!DB.open())
        return false;

    // create query
    QString req;
    req = d->getSQLCreateTable(tableref);

    return executeSQL(QStringList() << req, DB);
}

/**  \brief Create all the tables in the database. */
bool Database::createTables() const
{
    bool toReturn = true;
    QList<int> list = d->m_Tables.keys();
    qSort(list);
    foreach(const int & i, list)
        if(!createTable(i)) {
            toReturn = false;
            Utils::Log::addError("Database", QCoreApplication::translate("Database", "Can not create table %1").arg(table(i)),
                                 __FILE__, __LINE__);
        }
    return toReturn;
}

/**  \brief Execute simple SQL commands on the QSqlDatabase \e DB. */
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

/**  \brief Execute simple SQL commands on the QSqlDatabase \e DB. */
bool Database::executeSQL(const QString &req, const QSqlDatabase & DB)
{
    if (req.isEmpty())
        return false;
    /** \todo manage ; inside "" or '' */
    QStringList list = req.split(";\n", QString::SkipEmptyParts);
    return executeSQL(list, DB);
}

/**
  \brief Execute simple SQL commands stored in a file on the QSqlDatabase connected as \e connectionName.
  Line starting with '-- ' are ignored.\n
  All SQL commands must end with a ;
*/
bool Database::executeSqlFile(const QString &connectionName, const QString &fileName, QProgressDialog *dlg)
{
    if (!QFile::exists(fileName)) {
        Utils::Log::addError("Database", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName),
                             __FILE__, __LINE__);
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utils::Log::addError("Database", tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(fileName), __FILE__, __LINE__);
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
    if (!DB.open())
        return false;
//    if (!DB.transaction()) {
//        Utils::Log::addError("Tools", "Can not create transaction. Tools::executeSqlFile()", __FILE__, __LINE__);
//        return false;
//    }

    req.clear();
    QStringList queries;
    // Reconstruct req : removes comments
    foreach(const QString &s, list) {
        if (s.startsWith("--")) {
            continue;
        }
        req += s + " \n";
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
            Utils::Log::addQueryError("Database", query, __FILE__, __LINE__);
//            DB.rollback();
            return false;
        }

        if (dlg)
            dlg->setValue(dlg->value()+1);
    }
//    DB.commit();
    return true;
}

/**  \brief Import a CSV structured file \e fielName into a database \e connectionName, table \e table, using the speficied \e separator, and \e ingoreFirstLine or not.*/
bool Database::importCsvToDatabase(const QString &connectionName, const QString &fileName, const QString &table, const QString &separator, bool ignoreFirstLine)
{
    QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);
    if (content.isEmpty())
        return false;
    QStringList lines = content.split("\n", QString::SkipEmptyParts);
    content.clear();
    int start = 0;
    if (ignoreFirstLine)
        start = 1;

    // get database
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        if (!db.open()) {
            Utils::Log::addError("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(db.connectionName(), db.lastError().text()));
            return false;
        }
    }

    // get table field's name
    if (!db.tables().contains(table)) {
        Utils::Log::addError("Database", "No table found");
        return false;
    }
    // prepare the sql query
    QSqlRecord record = db.record(table);
    QString req = "INSERT INTO " + table + " (\n";
    for(int i = 0; i < record.count(); ++i) {
        req += record.fieldName(i) + ", \n";
    }
    req.chop(3);
    req += "\n) VALUES (";

    db.transaction();

    for(int i = start; i < lines.count(); ++i) {
        QStringList values = lines.at(i).split(separator, QString::KeepEmptyParts);
//        qWarning() << lines.at(i) << separator << values;
        QString reqValues;
        foreach(const QString &val, values) {
            if (val.isEmpty()) {
                reqValues += "NULL, ";
            } else {
                if (val.startsWith("'") && val.endsWith("'")) {
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
            Utils::Log::addQueryError("Database", query, __FILE__, __LINE__);
        }
//        qWarning() << lines.at(i) << req + reqValues << values;
    }

    db.commit();

    return true;
}

QString DatabasePrivate::getSQLCreateTable(const int & tableref)
{
    QString toReturn;
    toReturn = QString("CREATE TABLE IF NOT EXISTS `%1` (\n").arg(m_Tables.value(tableref));
    QList<int> list = m_Tables_Fields.values(tableref);
    qSort(list);

    foreach(int i, list) {

        // Manage NULL value
        if (m_DefaultFieldValue.value(i) == "NULL") {
            if (Database::TypeOfField(m_TypeOfField.value(i)) != Database::FieldIsUniquePrimaryKey) {
                toReturn.append(QString("%1 \t %2 DEFAULT NULL, \n")
                                .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
                                .arg(getTypeOfField(i)));// .leftJustified(20, ' '))
            } else {
                toReturn.append(QString("%1 \t %2, \n")
                                .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
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
                toReturn.append(QString("%1 \t %2 DEFAULT '%3', \n")
                                .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
                                .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                .arg(m_DefaultFieldValue.value(i)));
                break;
            case Database::FieldIsDate :
                if (m_DefaultFieldValue.value(i).startsWith("CUR"))
                    toReturn.append(QString("%1 \t %2 DEFAULT %3, \n")
                                .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
                                .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                .arg(m_DefaultFieldValue.value(i)));
                else
                    toReturn.append(QString("%1 \t %2 DEFAULT '%3', \n")
                                .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
                                .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                .arg(m_DefaultFieldValue.value(i)));
                break;
            case Database::FieldIsBoolean :
            case Database::FieldIsInteger :
            case Database::FieldIsLongInteger :
            case Database::FieldIsReal :
                toReturn.append(QString("%1 \t %2 DEFAULT %3, \n")
                                .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
                                .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                .arg(m_DefaultFieldValue.value(i)));
                break;
            default :
                    toReturn.append(QString("%1 \t %2 DEFAULT '%3', \n")
                                    .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
                                    .arg(getTypeOfField(i))// .leftJustified(20, ' '))
                                    .arg(m_DefaultFieldValue.value(i)));
            break;

        }
        }
    }
    toReturn.chop(3);
    toReturn.append("\n); \n\n");

    if (WarnSqlCommands)
        qWarning() << toReturn;

    return toReturn;
}

QString DatabasePrivate::getTypeOfField(const int & fieldref) const
{
    QString toReturn;
    switch (Database::TypeOfField(m_TypeOfField.value(fieldref)))
    {
        case Database::FieldIsUUID :
            toReturn = "varchar(40)";
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

/** \brief Used for the debugging. */
void Database::warn() const
{
    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);
    if (WarnLogMessages)
        Log::addMessage("Database", QString("Connection name : %1, Database Name : %2, Driver : %3, Opened : %4, Can open : %5 ")
                       .arg(d->m_ConnectionName, DB.databaseName(), DB.driverName())
                       .arg(DB.isOpen())
                       .arg(DB.open()));

    foreach(int i, d->m_Tables.keys())
    {
        if (WarnLogMessages)
            Log::addMessage("Database", QString("Tables = %1 : %2").arg(i).arg(d->m_Tables[i]));
        QList<int> list = d->m_Tables_Fields.values(i);
        qSort(list);
        foreach(int f, list)
            if (WarnLogMessages)
                Log::addMessage("Database", QString("    Fields = %1 : %2 %3 %4")
                               .arg(f)
                               .arg(d->m_Fields[f], d->getTypeOfField(f), d->m_DefaultFieldValue[i]));

    }
}

/** \brief Used for debugging and informations purpose. */
void Database::toTreeWidget(QTreeWidget *tree)
{
    QFont bold;
    bold.setBold(true);
    tree->clear();

    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);

    // General informations
    QTreeWidgetItem *db = new QTreeWidgetItem(tree, QStringList() << "General informations");
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
    QTreeWidgetItem *drv = new QTreeWidgetItem(tree, QStringList() << "Driver informations");
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
