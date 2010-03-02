/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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

#include <QDir>
#include <QSqlRecord>
#include <QSqlField>
#include <QCoreApplication>
#include <QHash>
#include <QMultiHash>
#include <QMap>

using namespace Utils;
using namespace Utils::Internal;

namespace Utils {
namespace Internal {
class DatabasePrivate
{
public:
    DatabasePrivate();
    ~DatabasePrivate() {}
    QString getSQLCreateTable(const int & tableref, const Database::AvailableDrivers driver);
    QString getTypeOfField(const int & fieldref, const Database::AvailableDrivers)const;

public:
    QHash<int, QString>        m_Tables;         // tables are not sorted
    QMultiHash<int, int>       m_Tables_Fields;  // links are not sorted
    QMap<int, QString>         m_Fields;         // fields should be sorted from first to last one using ref
    QHash<int, int>            m_TypeOfField;
    QHash<int, QString>        m_DefaultFieldValue;
    int                        m_LastCorrectLogin;
    bool                       m_initialized;
    QString                    m_ConnectionName;
};
}
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

DatabasePrivate::DatabasePrivate()
{
    m_initialized      = false;
    m_LastCorrectLogin = -1;
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
  \brief Create the connection to the database. If database does not exists createDatabase() is called.
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
                                   const QString & login, const QString & password,
                                   CreationOption createOption
                                  )
{
    bool toReturn = true;
    d->m_ConnectionName = "";

    // does driver is available
    if ((driver == SQLite) && (! QSqlDatabase::isDriverAvailable("QSQLITE"))) {
        Log::addError("Database", QCoreApplication::translate("Database",
                                                                    "ERROR : %1 driver is not available").arg("SQLite"));
        return false;
    } else if ((driver == MySQL) && (! QSqlDatabase::isDriverAvailable("QMYSQL"))) {
        Log::addError("Database", QCoreApplication::translate("Database",
                                                                    "ERROR : %1 driver is not available").arg("MySQL"));
        return false;
    } else if ((driver == PostSQL) && (! QSqlDatabase::isDriverAvailable("QPSQL"))) {
        Log::addError("Database", QCoreApplication::translate("Database",
                                                                    "ERROR : %1 driver is not available").arg("PostGreSQL"));
        return false;
    }

    // does connection already exists ?
    if (QSqlDatabase::contains(connectionName)) {
        Log::addMessage("Database", QCoreApplication::translate("Database",
                                                                      "WARNING : %1 database already in use").arg(connectionName));
        d->m_ConnectionName = connectionName;
        return true;
    }

    // test dbname file if user if asking SQLite driver
    if (driver == SQLite) {
        if ((! QFile(pathOrHostName + QDir::separator() + dbName).exists())||
             (QFileInfo(pathOrHostName + QDir::separator() + dbName).size() == 0)) {
            if (createOption == CreateDatabase){
                if (! createDatabase(connectionName, dbName, pathOrHostName, access, driver, login, password, createOption)) {
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
    }

    // test read access to database
    if (!QFileInfo(pathOrHostName + QDir::separator() + dbName).isReadable()) {
        Log::addError("Database", QCoreApplication::translate("Database", "ERROR : Database %1 is not readable. Path : %2")
                         .arg(dbName, pathOrHostName));
        toReturn = false;
    }

    // test write access
    // TODO manage MySQL
    if (driver == SQLite) {
        if ((access == ReadWrite) && (!QFileInfo(pathOrHostName + QDir::separator() + dbName).isWritable())) {
            Log::addError("Database", QCoreApplication::translate("Database", "ERROR : Database %1 is not writable. Path : %2.")
                             .arg(dbName, pathOrHostName));
            toReturn = false;
        }
    }

    // create connection
    QSqlDatabase DB;
    switch (driver)
    {
        case SQLite :
        {
             DB = QSqlDatabase::addDatabase("QSQLITE" , connectionName);
             DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
             break;
         }
        case MySQL :
        {
             DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
             DB.setDatabaseName(dbName);
             DB.setHostName(pathOrHostName);
             DB.setUserName(login);
             DB.setPassword(password);
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

/** \brief Set connectionName to \e c */
void Database::setConnectionName(const QString & c)
{ d->m_ConnectionName = c; }

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
    /** \todo need to be tested */
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
    if (! d->m_Tables.contains(tableref))
        return QString::null;
    return d->m_Tables.value(tableref);
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
    QHashIterator< int, QString> i(conditions);
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
    return where;
}

QString Database::select(const int & tableref, const int & fieldref, const QHash<int, QString> & conditions)const
{
    QString toReturn;
    toReturn = QString("SELECT %1 FROM `%2` WHERE %3")
            .arg(field(tableref, fieldref))
            .arg(table(tableref))
            .arg(getWhereClause(tableref, conditions));
    return toReturn;
}

QString Database::selectDistinct(const int & tableref, const int & fieldref, const QHash<int, QString> & conditions)const
{
    return select(tableref, fieldref, conditions).replace("SELECT", "SELECT DISTINCT");
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
    return toReturn;
}

QString Database::prepareInsertQuery(const int & tableref)const
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
    toReturn = QString("INSERT INTO `%1` \n(%2) \nVALUES(%3);")
            .arg(table(tableref))
            .arg(fields)
            .arg(numbers);
    return toReturn;
}

QString Database::prepareUpdateQuery(const int & tableref, int fieldref, QHash<int, QString> conditions)
{
    QString toReturn;
    toReturn = QString("UPDATE `%1` SET `%2` = ? WHERE %4")
               .arg(table(tableref))
               .arg(field(tableref, fieldref))
               .arg(getWhereClause(tableref, conditions));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    return toReturn;
}

QString Database::prepareUpdateQuery(const int & tableref, QHash<int, QString> conditions)
{
    QString toReturn;
    QString tmp;
    foreach(const QString & f, fields(tableref))
        tmp += QString ("`%1`=? , ").arg(f);
    tmp.chop(2);
    toReturn = QString("UPDATE `%1` \nSET %2 \nWHERE %4")
               .arg(table(tableref))
               .arg(tmp)
               .arg(getWhereClause(tableref, conditions));
    // UPDATE tbl_name [, tbl_name ...]
    // SET col_name1=expr1 [, col_name2=expr2 ...]
    // WHERE where_definition
    return toReturn;
}

bool Database::executeSQL(const QStringList & list, const QSqlDatabase & DB)const
{
    foreach(const QString & r, list)
    {
        if (r.isEmpty())continue;
        QSqlQuery q(r, DB);
        if (! q.isActive()) {
            Log::addQueryError("Database", q);
            return false;
        }
    }
    return true;
}

bool Database::executeSQL(const QString & req, const QSqlDatabase & DB)const
{
    if (req.isEmpty())
        return false;
    QSqlQuery q(req, DB);
    if (! q.isActive()) {
        Log::addQueryError("Database", q);
        return false;
    }
    return true;
}

bool Database::createTable(const int & tableref)const
{
    if (! d->m_Tables.contains(tableref))
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
    req = d->getSQLCreateTable(tableref, SQLite);

    return executeSQL(QStringList() << req, DB);
}

bool Database::createTables() const
{
    bool toReturn = true;
    QList<int> list = d->m_Tables.keys();
    qSort(list);
    foreach(const int & i, list)
        if(! createTable(i))
            toReturn = false;
    return toReturn;
}


QString DatabasePrivate::getSQLCreateTable(const int & tableref, const Database::AvailableDrivers driver)
{
    QString toReturn;
    toReturn = QString("CREATE TABLE IF NOT EXISTS `%1` (\n").arg(m_Tables.value(tableref));
    QList<int> list = m_Tables_Fields.values(tableref);
    qSort(list);
    foreach(int i, list)
        toReturn.append(QString("%1 \t %2 %3, \n")
                         .arg(QString("`%1`").arg(m_Fields.value(i)))//.leftJustified(55, ' '))
                         .arg(getTypeOfField(i, driver))// .leftJustified(20, ' '))
                         .arg(m_DefaultFieldValue.value(i)));
    toReturn.chop(3);
    toReturn.append("\n); \n\n");
    return toReturn;
}

QString DatabasePrivate::getTypeOfField(const int & fieldref, const Database::AvailableDrivers)const
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
        case Database::FieldIsInteger :
            toReturn = "integer";
            break;
        case Database::FieldIsUniquePrimaryKey :
            toReturn = "integer not null primary key";
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
                               .arg(d->m_Fields[f], d->getTypeOfField(f, SQLite), d->m_DefaultFieldValue[i]));

    }
}
