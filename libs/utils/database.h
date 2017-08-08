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
#ifndef UTILS_DATABASE_H
#define UTILS_DATABASE_H

#include <utils/global_exporter.h>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QHash>
#include <QFlags>

#include <QDebug>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QProgressDialog;
class QFile;
QT_END_NAMESPACE

/**
 * \file database.h
 * \author Eric Maeker
 * \version 0.10.1
 * \date 19 April 2015
*/

namespace Utils {
class DatabaseConnector;

namespace Internal {
class DatabasePrivate;
}

struct Field {
    Field() : table(-1), field(-1), type(-1), orCondition(false) {}

    Field(const Field &f) :
        table(f.table), field(f.field), type(f.type),
        tableName(f.tableName), fieldName(f.fieldName), whereCondition(f.whereCondition),
        orCondition(f.orCondition) {}

    Field(const int table, const int field, const QString &tableName, const QString &fieldName) :
        table(table), field(field), type(-1), tableName(tableName), fieldName(fieldName),
        orCondition(false) {}

    Field(const int table, const int field, const QString &where = QString::null, const bool isOrCondition = false) :
        table(table), field(field), type(-1), whereCondition(where), orCondition(isOrCondition) {}

    //    Field &operator=(const Field &f) const {Field ret(f.table, f.field, f.tableName, f.fieldName); ret.type=f.type; ret.whereCondition=f.whereCondition; return ret;}

    bool operator==(const Field &f) const {return table==f.table && field==f.field && type==f.type;}

    int table;
    int field;
    int type;
    QString tableName;
    QString fieldName;
    QString whereCondition;
    bool orCondition;
};
typedef QList<Field> FieldList;

struct Join {
    Join() : type(-1) {}

    Join(const Field &field1, const Field &field2, const int type = 0) :
        field1(field1), field2(field2), type(type) {}

    Join(const int t1, const int f1, const int t2, const int f2, const int joinType = 0)
    {
        field1.table = t1;
        field1.field = f1;
        field2.table = t2;
        field2.field = f2;
        type = joinType;
    }

    bool isNull() const {return (field1.field < 0) && (field2.field < 0); }

    Field field1;
    Field field2;
    int type;
};
typedef QList<Join> JoinList;

class UTILS_EXPORT Database
{
public:

    /** \brief Describe the desired type of database access (read only, read and write...). */
    enum TypeOfAccess
    {
        ReadOnly = 0,
        ReadWrite
    };

    /**
       \brief Simplified Enum for database driver selection.
       Currently, only SQLite is take into account.
    */
    enum AvailableDrivers
    {
        SQLite,   /*!< \brief Uses SQLite driver. */
        MySQL,    /*!< \brief Uses MySQL driver. */
    };

    /**
       \brief Specifies what to do when database does not exist.
       \sa createConnection(), createDatabase
    */
    enum CreationOption  // if database does not exist :
    {
        WarnOnly = 0,
        CreateDatabase,
        DeleteAndRecreateDatabase // connected to Core::ICommandLine -> --clear-user-databases param
    };

    /**
      \brief Determine the type of the field into the Database scheme .
      \sa addField()
    */
    enum TypeOfField
    {
        FieldUndefined = 0,
        FieldIsInteger,
        FieldIsLongInteger,
        FieldIsUnsignedInteger,
        FieldIsUnsignedLongInteger, // unsigned bigint
        // TODO: FieldIsUnsignedLongInteger does not correctly works on SQLite, needs testing with MySQL
        FieldIsLongText,            // varchar(2000)
        FieldIsShortText,           // varchar(200)
        FieldIsOneChar,
        FieldIsTwoChars,
        FieldIs19Chars,             // MySQL UTC Timestamp
        FieldIs32Chars,             // Phone numbers
        FieldIs255Chars,            // Maximum Linux filename length
        FieldIsLanguageText,
        FieldIsDate,
        FieldIsTime,
        FieldIsDateTime,
        FieldIsTimeStamp,
        FieldIsIsoUtcDateTime,      // varchar(20) "YYYY-MM-DDTHH:mm:ssZ"
        FieldIsBlob,                // SQLite: 1,000,000,000 max size
        // MySQL:  4Go max size
        FieldIsUUID,                // varchar(32) (see Utils::createUid())
        FieldIsBoolean,
        FieldIsUniquePrimaryKey,
        FieldIsReal
    };

    enum TypeOfJoin {
        SimpleJoin = 0,
        OuterJoin,
        LeftJoin,
        InnerJoin,
        NaturalJoin,
        CrossJoin
    };

    /** \brief Describe the grants on database/server (mainly used for MySQL connections). */
    enum Grant {
        Grant_NoGrant          = 0x000000,
        Grant_Select           = 0x000001,
        Grant_Update           = 0x000002,
        Grant_Insert           = 0x000004,
        Grant_Delete           = 0x000008,
        Grant_Create           = 0x000010,
        Grant_Drop             = 0x000020,
        Grant_Index            = 0x000040,
        Grant_Alter            = 0x000080,
        Grant_CreateTmpTables  = 0x000100,
        Grant_LockTables       = 0x000200,
        Grant_Execute          = 0x000400,
        Grant_CreateView       = 0x000800,
        Grant_ShowView         = 0x001000,
        Grant_CreateRoutine    = 0x002000,
        Grant_AlterRoutine     = 0x004000,
        Grant_CreateUser       = 0x008000,
        Grant_Options          = 0x010000,
        Grant_Process          = 0x020000,
        Grant_Trigger          = 0x040000,
        Grant_ShowDatabases    = 0x080000,
        Grant_Reload           = 0x100000,
        Grant_All              = Grant_Select|Grant_Update|Grant_Insert
        |Grant_Delete|Grant_Create|Grant_Drop
        |Grant_Index|Grant_Alter|Grant_CreateTmpTables
        |Grant_LockTables|Grant_Execute|Grant_CreateView
        |Grant_ShowView|Grant_CreateRoutine
        |Grant_AlterRoutine|Grant_CreateUser
        |Grant_Options|Grant_Process|Grant_Trigger
        |Grant_ShowDatabases|Grant_Reload
    };
    /** \brief Enumerate the values of fields got by Sqlite PRAGMA command */
    enum PragmaValues
    {
        Cid_PragmaValue = 0,
        Name_PragmaValue ,
        Type_PragmaValue ,
        OptionNull_PragmaValue ,
        DefaultValue_PragmaValue ,
        Pk_PragmaValue ,
        PragmaValues_MaxParam
    };

    Q_DECLARE_FLAGS(Grants, Grant)

    enum WhereClauseType {
        AND = 0,
        OR
    };

    Database();
    virtual ~Database();

    static void logAvailableDrivers();

    // connection
    virtual bool createConnection(const QString &connectionName,
                                  const QString &nonPrefixedDbName,
                                  const Utils::DatabaseConnector &connector,
                                  const CreationOption createOption = WarnOnly);


    /**
        \brief This member is called by createConnection() if the asked database does not exist.
               By default it does nothing and return an error state (false).
    */
    virtual bool createDatabase(const QString &connectionName,
                                const QString &prefixedDbName,
                                const Utils::DatabaseConnector &connector,
                                CreationOption createOption
                                );

    virtual bool createDatabase(const QString &/*connectionName*/ ,
                                const QString &/*prefixedDbName*/,
                                const QString &/*pathOrHostName*/,
                                TypeOfAccess /*access*/,
                                AvailableDrivers /*driver*/,
                                const QString &/*login*/,
                                const QString &/*pass*/,
                                const int /*port*/,
                                CreationOption /*createOption*/
                                ) { return false; }

    static void setDatabasePrefix(const QString &prefix) {_prefix = prefix;}
    static QString prefixedDatabaseName(AvailableDrivers driver, const QString &dbName);

    // MySQL Specific members
    virtual bool createMySQLDatabase(const QString &dbName);
    virtual bool createMySQLUser(const QString &log, const QString &clearPass, const Grants grants, const QString &databasePrefix = QString::null);
    virtual bool dropMySQLUser(const QString &log, const QString &userHost = QString::null);
    virtual bool changeMySQLUserOwnPassword(const QString &login, const QString &newPassword);
    virtual bool changeMySQLOtherUserPassword(const QString &login, const QString &newPassword);
    virtual bool modifyMySQLColumnType(const int &tableref, const int &fieldref,
                                       const QString &defaultValue = QString());
    virtual bool modifyMySQLColumnType(const int &tableref, const int &fieldref);
    virtual QStringList mySQLUserHostNames(const QString & userName);

    // All drivers members
    virtual QSqlDatabase database() const;
    virtual QString connectionName() const;
    static QString createUid();
    static QString sqliteFileName(const QString &connectionName,
                                  const QString &nonPrefixedDbName,
                                  const Utils::DatabaseConnector &connector,
                                  bool addGlobalPrefix = true);

    // Grants
    Grants grants(const QString &connectionName) const;
    static Grants getConnectionGrants(const QString &connectionName);

    // Manage database scheme (use enums for the int references)
    virtual int addTable(const int &ref, const QString &name);
    virtual int addField(const int &tableref, const int &fieldref, const QString &name, TypeOfField type = FieldUndefined, const QString &defaultValue = "NULL");
    virtual void addPrimaryKey(const int &tableref, const int &fieldref);
    virtual void addIndex(const int &tableref,
                          const int &fieldref,
                          const int &prefix = 0,
                          const QString &name = QString::null);
    virtual void addIndex(const Utils::Field &field,
                          const int &prefix = 0,
                          const QString &name = QString::null);

    virtual bool checkDatabaseScheme();
    virtual bool checkVersion(const Field &field, const QString &expectedVersion);
    virtual QString getVersion(const Field &field) const;
    virtual quint32 getVersionNumber(const Field &field) const;
    virtual bool setVersion(const Field &field, const QString &version);
    virtual bool setVersion(const Field &field, const int &version);
    virtual bool setSchemaVersion(const int &version, const QString &dbname);

    virtual QString fieldName(const int &tableref, const int &fieldref) const;
    virtual Field field(const int &tableref, const int &fieldref) const;

    virtual QStringList fieldNames(const int &tableref) const;
    virtual QStringList fieldNamesSql(const int &tableref) const;
    virtual TypeOfField typeOfField(const int tableref, const int fieldref) const;
    virtual FieldList fields(const int tableref) const;

    virtual QString table(const int &tableref) const;
    virtual QStringList tables() const;

    virtual QString getWhereClause(const int &tableref, const QHash<int, QString> &conditions) const;
    virtual QString getWhereClause(const FieldList &fields, WhereClauseType type = AND) const;
    virtual QString getWhereClause(const Field &field) const;

    virtual QString joinToSql(const Join &join) const;

    virtual QString select(const int &tableref, const int &fieldref, const QHash<int, QString> &conditions) const;
    virtual QString select(const int &tableref, const QList<int> &fieldsref, const QHash<int, QString> &conditions) const;
    virtual QString select(const int &tableref, const QList<int> &fieldsref) const;
    virtual QString select(const int &tableref, const QHash<int, QString> &conditions) const;
    virtual QString select(const int &tableref) const;
    virtual QString select(const int &tableref, const int &fieldref) const;
    virtual QString selectDistinct(const int &tableref, const int &fieldref, const QHash<int, QString> &conditions) const;
    virtual QString selectDistinct(const int &tableref, const QList<int> &fields, const QHash<int, QString> &conditions) const;
    virtual QString selectDistinct(const int &tableref, const int &fieldref) const;

    virtual QString select(const FieldList &select, const JoinList &joins) const;
    virtual QString select(const FieldList &select, const JoinList &joins, const FieldList &conditions) const;
    virtual QString select(const FieldList &select, const JoinList &joins, const Field &condition) const;
    virtual QString select(const int tableref, const JoinList &joins, const FieldList &conditions) const;
    virtual QString select(const int tableref, const Join &join, const Field &condition) const;
    virtual QString select(const int tableref, const Join &join, const FieldList &conditions) const;
    virtual QString select(const Field &select, const JoinList &joins, const Field &condition) const;
    virtual QString select(const FieldList &select, const Join &join, const Field &condition) const;
    virtual QString select(const Field &select, const Join &join, const FieldList &conditions) const;
    virtual QString select(const Field &select, const JoinList &joins, const FieldList &conditions) const;
    virtual QString select(const Field &select, const Join &join, const Field &conditions) const;
    virtual QString selectLast(const Field &fieldref) const;

    virtual QString fieldEquality(const int tableRef1, const int fieldRef1, const int tableRef2, const int fieldRef2) const;

    virtual int count(const int &tableref, const int &fieldref, const QString &filter = QString()) const;
    virtual int count(const FieldList &select, const JoinList &joins, const FieldList &conditions) const;
    virtual int countDistinct(const FieldList &select, const JoinList &joins, const FieldList &conditions) const;
    virtual QVariant max(const int &tableref, const int &fieldref, const QString &filter = QString()) const;
    virtual QVariant max(const int &tableref, const int &fieldref, const int &groupBy, const QString &filter = QString()) const;
    virtual QVariant min(const int &tableref, const int &fieldref, const QString &filter) const;

    virtual QString maxSqlCommand(const int &tableref, const int &fieldref, const QString &filter = QString()) const;
    virtual QString totalSqlCommand(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const;
    virtual QString totalSqlCommand(const int tableRef, const int fieldRef) const;
    double sum(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const;
    double sum(const int tableRef, const int fieldRef) const;

    virtual QString prepareInsertQuery(const int tableref) const;
    virtual QString getLastExecutedQuery(const QSqlQuery& query);

    virtual QString prepareUpdateQuery(const int tableref, const int fieldref, const QHash<int, QString> &conditions);
    virtual QString prepareUpdateQuery(const int tableref, const int fieldref);
    virtual QString prepareUpdateQuery(const int tableref, const QHash<int, QString> &conditions);
    virtual QString prepareUpdateQuery(const int tableref, const QList<int> &fieldref, const QHash<int, QString> &conditions);
    virtual QString prepareUpdateQuery(const int tableref);

    virtual QString prepareDeleteQuery(const int tableref);
    virtual QString prepareDeleteQuery(const int tableref, const QHash<int,QString> &conditions);

    virtual bool createTable(const int &tableref) const;
    virtual bool createTables() const;

    virtual bool alterTableForNewField(const int tableRef, const int newFieldRef, const int TypeOfField = FieldUndefined, const QString & nullOption = QString::null);

    static bool vacuum(const QString &connectionName);
    static bool executeSQL(const QStringList &list, QSqlDatabase &DB);
    static bool executeSQL(const QString &req, QSqlDatabase &DB);
    static bool executeSqlFile(const QString &connectionName, const QString &fileName, QString *error = 0);
    static bool executeQueryFile(QFile &file, QSqlDatabase &db);
    static bool importCsvToDatabase(const QString &connectionName, const QString &fileName, const QString &table, const QString &separator = QString(";"), bool ignoreFirstLine = false);
    virtual void toTreeWidget(QTreeWidget *tree) const;

    virtual void setConnectionName(const QString &c);

protected:
    virtual void setDriver(const AvailableDrivers d);
    virtual AvailableDrivers driver() const;

private:
    Internal::DatabasePrivate *d_database;
    static QString _prefix;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Utils::Database::Grants)

QDebug operator<<(QDebug dbg, const Utils::Database &database);
QDebug operator<<(QDebug dbg, const Utils::Database *database);

#endif // UTILS_DATABASE_H
