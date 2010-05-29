/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Database
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
  \ingroup toolkit
  \ingroup database_toolkit
*/

#include "database.h"
#include "log.h"

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

enum {WarnSqlCommands=false};

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
    Utils::Log::addMessage("Database", QString("Available drivers : %1").arg(tmp));
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
    bool toReturn = true;
    d->m_ConnectionName = "";

    // does driver is available
    switch (driver) {
    case SQLite :
        {
            Utils::Log::addMessage("Database", QString("Trying to connect database %1 with %2 driver")
                                   .arg(dbName)
                                   .arg("SQLite"));
            if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
                Log::addError("Database", QCoreApplication::translate("Database",
                                                                      "ERROR : %1 driver is not available")
                              .arg("SQLite"));
                return false;
            }
            break;
        }
    case MySQL:
        {
            Utils::Log::addMessage("Database", QString("Trying to connect database %1 with %2 driver")
                                   .arg(dbName)
                                   .arg("MySQL"));
            if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
                Log::addError("Database", QCoreApplication::translate("Database",
                                                                      "ERROR : %1 driver is not available")
                              .arg("MySQL"));
                return false;
            }
            break;
        }
    case PostSQL :
        {
            Utils::Log::addMessage("Database", QString("Trying to connect database %1 with %2 driver")
                                   .arg(dbName)
                                   .arg("PostGre SQL"));
            if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
                Log::addError("Database", QCoreApplication::translate("Database",
                                                                      "ERROR : %1 driver is not available")
                              .arg("PostGreSQL"));
                return false;
            }
        }
    }

    // does connection already exists ?
    if (QSqlDatabase::contains(connectionName)) {
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
                                     .arg(pathOrHostName).arg(DB.lastError().text()));
                return false;
            }
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
                                                                              "ERROR : %1 database does not exist and can not be created. Path = %2").arg(dbName, pathOrHostName));
                        return false;
                    }
                } else { // Warn Only
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
                                     .arg(dbName).arg(DB.lastError().text()));
                if (createOption == CreateDatabase) {
                    if (!createDatabase(connectionName, dbName, pathOrHostName, access, driver, login, password, port, createOption)) {
                        Log::addError("Database", QCoreApplication::translate("Database",
                                      "ERROR : %1 database does not exist and can not be created. Path = %2")
                                      .arg(dbName, pathOrHostName));
                        return false;
                    }
                } else { // Warn Only
                        Log::addMessage("Database", QCoreApplication::translate("Database",
                                        "ERROR : %1 database does not exist and can not be created. Path = %2")
                                        .arg(dbName, pathOrHostName));
                        return false;
                }
            }
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
                              .arg(dbName, pathOrHostName));
                toReturn = false;
            }
            break;
        }
    case MySQL:
        {
            if (!DB.open()) {
                Log::addError("Database", QCoreApplication::translate("Database",
                              "ERROR : Database %1 is not readable. Path : %2")
                              .arg(dbName, pathOrHostName));
                return false;
            }
            QSqlQuery query("SHOW GRANTS FOR CURRENT_USER;", DB);
            if (!query.isActive()) {
                Log::addError("Database", QCoreApplication::translate("Database",
                              "ERROR : Database %1 is not readable. Path : %2")
                              .arg(dbName, pathOrHostName));
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
                                  .arg(dbName, pathOrHostName));
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

    Log::addMessage("Database", QCoreApplication::translate("Database",  "INFO : database %1 connection = %2")
                       .arg(connectionName).arg(DB.open()));

    // test connection
    if (!DB.isOpen()) {
        Log::addError("Database", QCoreApplication::translate("Database",
                                                                    "WARNING : can not open database %1 : %2 \n %3 ")
                         .arg(connectionName, DB.lastError().driverText(), DB.lastError().databaseText()));
        toReturn = false;
    }
    else {
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
    return d->m_Fields.key(name)- (tableref * 1000);
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
                                   .arg(d->m_Tables.value(i)));
            return false;
        }
        QList<int> fields = d->m_Tables_Fields.values(i);
        qSort(fields);
        int id = 0;
        foreach(int f, fields) {
            if (d->m_Fields.value(f)!= rec.field(id).name())
            {
                Log::addError("Database", QCoreApplication::translate("Database", "Database Scheme Error : field number %1 differs : %2 instead of %3")
                                   .arg(id).arg(d->m_Fields.value(f), rec.field(id).name()));
                return false;
            }
            id++;
        }
    }
    return true;
}


QString Database::field(const int & tableref, const int & fieldref)const
{
    if (! d->m_Tables.contains(tableref))
        return QString::null;
    if (! d->m_Tables_Fields.keys().contains(tableref))
        return QString::null;
    if (! d->m_Fields.keys().contains(fieldref + (tableref * 1000)))
        return QString::null;

    return d->m_Fields.value(fieldref + (tableref * 1000));
}

QStringList Database::fields(const int & tableref)const
{
    if (! d->m_Tables.contains(tableref))
        return QStringList();
    if (! d->m_Tables_Fields.keys().contains(tableref))
        return QStringList();

    QList<int> list = d->m_Tables_Fields.values(tableref);
    qSort(list);
    QStringList toReturn;
    foreach(int i, list)
       toReturn << d->m_Fields.value(i);
    return toReturn;
}

QString Database::table(const int & tableref)const
{
    return d->m_Tables.value(tableref, QString());
}

QStringList Database::tables() const
{
    return d->m_Tables.values();
}

/** \brief Create a where clause. */
QString Database::getWhereClause(const int & tableref, const QHash<int, QString> & conditions)const
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

QString Database::select(const int & tableref, const int & fieldref, const QHash<int, QString> & conditions) const
{
    QString toReturn;
    toReturn = QString("SELECT %1 FROM `%2` WHERE %3")
            .arg(field(tableref, fieldref))
            .arg(table(tableref))
            .arg(getWhereClause(tableref, conditions));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

QString Database::select(const int & tableref, const int & fieldref) const
{
    QString toReturn;
    toReturn = QString("SELECT %1 FROM `%2`")
            .arg(field(tableref, fieldref))
            .arg(table(tableref));
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

QString Database::selectDistinct(const int & tableref, const int & fieldref, const QHash<int, QString> & conditions) const
{
    return select(tableref, fieldref, conditions).replace("SELECT", "SELECT DISTINCT");
}

QString Database::selectDistinct(const int & tableref, const int & fieldref) const
{
    return select(tableref, fieldref).replace("SELECT", "SELECT DISTINCT");
}

int Database::count(const int & tableref, const int & fieldref, const QString &filter) const
{
    QString req = QString("SELECT count(%1) FROM %2").arg(d->m_Fields.value(1000 * tableref + fieldref)).arg(d->m_Tables[tableref]);
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

double Database::max(const int &tableref, const int &fieldref, const int &groupBy, const QString &filter) const
{
    QString req = QString("SELECT max(%1) FROM %2 GROUP BY %3")
                  .arg(d->m_Fields.value(1000 * tableref + fieldref))
                  .arg(d->m_Tables[tableref])
                  .arg(d->m_Fields.value(1000 * tableref + groupBy));
    if (!filter.isEmpty())
        req += " WHERE " + filter;
//    if (WarnSqlCommands)
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

QString Database::select(const int & tableref, const QList<int> &fieldsref, const QHash<int, QString> & conditions)const
{
    QString toReturn;
    QString tmp;
    foreach(const int & i, fieldsref)
        tmp += "`" + field(tableref, i)+ "`, ";
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

QString Database::select(const int & tableref,const  QList<int> &fieldsref)const
{
    QString toReturn;
    QString tmp;
    foreach(const int & i, fieldsref)
        tmp += "`" + field(tableref, i)+ "`, ";
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

QString Database::select(const int & tableref, const QHash<int, QString> & conditions)const
{
    QString toReturn;
    QString tmp;
    QList<int> list = d->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int & i, list)
        tmp += "`" + d->m_Fields.value(i)+ "`, ";
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

QString Database::select(const int & tableref) const
{
    QString toReturn;
    QString tmp;
    QList<int> list = d->m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(const int & i, list)
        tmp += "`" + d->m_Fields.value(i)+ "`, ";
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

QString Database::prepareInsertQuery(const int & tableref) const
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

QString Database::prepareUpdateQuery(const int & tableref, int fieldref, const QHash<int, QString> &conditions)
{
    QString toReturn;
    toReturn = QString("UPDATE `%1` SET `%2` = ? WHERE %4")
               .arg(table(tableref))
               .arg(field(tableref, fieldref))
               .arg(getWhereClause(tableref, conditions));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

QString Database::prepareUpdateQuery(const int & tableref, int fieldref)
{
    QString toReturn;
    toReturn = QString("UPDATE `%1` SET `%2` = ?")
               .arg(table(tableref))
               .arg(field(tableref, fieldref));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    if (WarnSqlCommands)
        qWarning() << toReturn;
    return toReturn;
}

QString Database::prepareUpdateQuery(const int & tableref, const QHash<int, QString> &conditions)
{
    QString toReturn;
    QString tmp;
    foreach(const QString &f, fields(tableref))
        tmp += QString ("`%1`=? , ").arg(f);
    tmp.chop(2);
    toReturn = QString("UPDATE `%1` SET \n%2 \nWHERE %4")
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

QString Database::prepareUpdateQuery(const int & tableref)
{
    QString toReturn;
    QString tmp;
    foreach(const QString &f, fields(tableref))
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

bool Database::executeSQL(const QStringList & list, const QSqlDatabase & DB) const
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

bool Database::executeSQL(const QString &req, const QSqlDatabase & DB) const
{
    if (req.isEmpty())
        return false;
    /** \todo manage ; inside "" or '' */
    QStringList list = req.split(";\n", QString::SkipEmptyParts);
    return executeSQL(list, DB);
}

bool Database::createTable(const int & tableref) const
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

bool Database::createTables() const
{
    bool toReturn = true;
    QList<int> list = d->m_Tables.keys();
    qSort(list);
    foreach(const int & i, list)
        if(!createTable(i)) {
            toReturn = false;
            Utils::Log::addError("Database", QCoreApplication::translate("Database", "Can not create table %1").arg(table(i)));
        }
    return toReturn;
}

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

QString Database::total(const int tableRef, const int fieldRef) const
{
    QString toReturn;
    toReturn = QString("SELECT total(`%1`) FROM `%2`")
               .arg(d->m_Fields.value(fieldRef + tableRef * 1000))
               .arg(d->m_Tables.value(tableRef));
    return toReturn;
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

/** \brief Used for the debugging. */
void Database::warn() const
{
    QSqlDatabase DB = QSqlDatabase::database(d->m_ConnectionName);
    Log::addMessage("Database", QString("Connection name : %1, Database Name : %2, Driver : %3, Opened : %4, Can open : %5 ")
                       .arg(d->m_ConnectionName, DB.databaseName(), DB.driverName())
                       .arg(DB.isOpen())
                       .arg(DB.open()));

    foreach(int i, d->m_Tables.keys())
    {
        Log::addMessage("Database", QString("Tables = %1 : %2").arg(i).arg(d->m_Tables[i]));
        QList<int> list = d->m_Tables_Fields.values(i);
        qSort(list);
        foreach(int f, list)
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
