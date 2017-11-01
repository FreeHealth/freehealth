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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <utils/global.h>
#include <utils/database.h>
#include <utils/databaseconnector.h>
#include <utils/log.h>
#include <utils/randomizer.h>

#include "../../autotest.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QTest>

class TestBase : public Utils::Database
{
public:
    TestBase() {}
    ~TestBase() {}

    bool createDatabase(const QString &connectionName,
                        const QString &prefixedDbName,
                        const Utils::DatabaseConnector &connector,
                        CreationOption createOption
                        )
    {
        Q_UNUSED(createOption);

        setConnectionName(connectionName);
        setDriver(connector.driver());

        // create an empty database and connect
        QSqlDatabase DB;
        if (connector.driver() == SQLite) {
            QString pathOrHostName = sqliteFileName(connectionName, prefixedDbName, connector);
            DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
            if (!QDir(pathOrHostName).exists())
                if (!QDir().mkpath(pathOrHostName)) {
                    return false;
                }
            DB.setDatabaseName(QDir::cleanPath(QString("%1/%2").arg(pathOrHostName).arg(prefixedDbName)));
            if (!DB.open()) {
                return false;
            }


//        } else if (driver == MySQL) {
//            // TODO: test grants here or before ?
//            if (QSqlDatabase::connectionNames().contains(connectionName)) {
//                DB = QSqlDatabase::database(connectionName);
//            } else {
//                DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
//                DB.setHostName(pathOrHostName);
//                DB.setUserName(login);
//                DB.setPassword(pass);
//                DB.setPort(port);
//            }
//            DB.setDatabaseName("mysql");
//            if (!DB.open()) {
//                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
//                return false;
//            }

//            createMySQLDatabase(dbName);
//            // change database connection
//            DB.setDatabaseName(dbName);

//            if (!DB.open()) {
//                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
//                return false;
//            }
        }

        // create db structure
        if (!createTables())
            return false;

        return true;
    }


};



/**
 * Run test on:
 * - Utils::Database
 */

class tst_UtilsDatabase : public QObject
{
    Q_OBJECT

    TestBase db;
    QString _tmpPath, _connectionName;
    const int loop = 10;

private slots:
    void initTestCase()
    {
        _tmpPath = QString("%1/freemedforms/unit-test/database/%2/")
                .arg(QDir::tempPath())
                .arg(Utils::createUid());
        QVERIFY(QDir().mkpath(_tmpPath));
        _connectionName = "unit-test";
    }

    void test_databasePrefix()
    {
        Utils::Database::setDatabasePrefix("pref_");
        QVERIFY(Utils::Database::prefixedDatabaseName(Utils::Database::MySQL, "DbName") == "pref_fmf_DbName");
        QVERIFY(Utils::Database::prefixedDatabaseName(Utils::Database::SQLite, "DbName") == "pref_DbName");
        Utils::Database::setDatabasePrefix(QString::null);
    }

    void test_sqliteFileName()
    {
        QString roPath = "RO/";
        QString rwPath = "RW/";
        QString globPref = "globPref";
        QString connectorPref = "c_Pref";
        QString connection = "_C_";
        QString db = "_Db_";
        QString expected;

        Utils::DatabaseConnector c;
        c.setDriver(Utils::Database::SQLite);
        c.setAbsPathToReadOnlySqliteDatabase(roPath);
        c.setAbsPathToReadWriteSqliteDatabase(rwPath);
        c.setGlobalDatabasePrefix(connectorPref);
        Utils::Database::setDatabasePrefix(globPref);

        //  RO
        c.setAccessMode(Utils::DatabaseConnector::ReadOnly);
        c.setSqliteUsesExactFile(false);
        // not exact file: RO path + connection + dbName {.db}
        expected = QDir::cleanPath(QString("%1/%2/%3.db").arg(roPath).arg(connection).arg(db));
        QVERIFY(Utils::Database::sqliteFileName(connection, db, c) == expected);
        c.setSqliteUsesExactFile(true);
        // exact file: RO path + dbName {.db}
        expected = QDir::cleanPath(QString("%1/%2.db").arg(roPath).arg(db));
        QVERIFY(Utils::Database::sqliteFileName(connection, db, c) == expected);

        //  RW
        c.setAccessMode(Utils::DatabaseConnector::ReadWrite);
        c.setSqliteUsesExactFile(false);
        // not exact file: RW path + connection + {prefix}dbName {.db}
        expected = QDir::cleanPath(QString("%1/%2/%3%4.db").arg(rwPath).arg(connection).arg(globPref).arg(db));
        QVERIFY(Utils::Database::sqliteFileName(connection, db, c) == expected);
        expected = QDir::cleanPath(QString("%1/%2/%3.db").arg(rwPath).arg(connection).arg(db));
        QVERIFY(Utils::Database::sqliteFileName(connection, db, c, false) == expected);

        c.setSqliteUsesExactFile(true);
        // exact file: RW path + dbName {.db}
        expected = QDir::cleanPath(QString("%1/%2.db").arg(rwPath).arg(db));
        QVERIFY(Utils::Database::sqliteFileName(connection, db, c) == expected);

        Utils::Database::setDatabasePrefix(QString::null);
        c.setSqliteUsesExactFile(false);
        // not exact file: RW path + connection + {prefix}dbName {.db}
        expected = QDir::cleanPath(QString("%1/%2/%3.db").arg(rwPath).arg(connection).arg(db));
        QVERIFY(Utils::Database::sqliteFileName(connection, db, c) == expected);
    }

    void test_tableAndFields()
    {
        // Test with memory SQLite db
        db.addTable(0,  "t0");
        db.addTable(1,  "t1");
        db.addTable(2,  "Version");

        db.addField(0, 0, "f0", Utils::Database::FieldIsUniquePrimaryKey);
        db.addField(0, 1, "f1", Utils::Database::FieldIsUUID);
        db.addField(0, 2, "f2", Utils::Database::FieldIsBoolean);
        db.addField(0, 3, "f3", Utils::Database::FieldIsLongText);
        db.addField(0, 4, "f4", Utils::Database::FieldIsShortText);
        db.addField(0, 5, "f5", Utils::Database::FieldIsDateTime);
        db.addIndex(0, 0);
        db.addIndex(0, 1);
        db.addIndex(0, 2);

        db.addField(1, 0, "f1.0", Utils::Database::FieldIsUniquePrimaryKey);
        db.addField(1, 1, "f1.1", Utils::Database::FieldIsReal);

        db.addField(2, 0, "version", Utils::Database::FieldIsShortText);

        QVERIFY(db.table(0) == "t0");
        QVERIFY(db.table(1) == "t1");
        QStringList tables;
        tables << "t0" << "t1" << "Version";
        QVERIFY(db.tables().count() == tables.count());
        // FIXME: using QHash can lead to non correctly sorted list

        QStringList table0Fields;
        table0Fields << "f0" << "f1" << "f2" << "f3" << "f4" << "f5";
        QStringList table1Fields;
        table1Fields << "f1.0" << "f1.1";
        QVERIFY(db.fieldNames(0) == table0Fields);
        QVERIFY(db.fieldNames(1) == table1Fields);
        QVERIFY(db.fieldName(0,0) == "f0");
        QVERIFY(db.fieldName(0,1) == "f1");
        QVERIFY(db.fieldName(0,2) == "f2");
        QVERIFY(db.fieldName(0,3) == "f3");
        QVERIFY(db.fieldName(0,4) == "f4");
        QVERIFY(db.fieldName(0,5) == "f5");
        QVERIFY(db.fieldName(1,0) == "f1.0");
        QVERIFY(db.fieldName(1,1) == "f1.1");

        QVERIFY(db.typeOfField(0,0) == Utils::Database::FieldIsUniquePrimaryKey);
        QVERIFY(db.typeOfField(0,1) == Utils::Database::FieldIsUUID);
        QVERIFY(db.typeOfField(0,2) == Utils::Database::FieldIsBoolean);
        QVERIFY(db.typeOfField(0,3) == Utils::Database::FieldIsLongText);
        QVERIFY(db.typeOfField(0,4) == Utils::Database::FieldIsShortText);
        QVERIFY(db.typeOfField(0,5) == Utils::Database::FieldIsDateTime);
        QVERIFY(db.typeOfField(1,0) == Utils::Database::FieldIsUniquePrimaryKey);
        QVERIFY(db.typeOfField(1,1) == Utils::Database::FieldIsReal);

        QVERIFY(db.field(0,0).type == Utils::Database::FieldIsUniquePrimaryKey);
        QVERIFY(db.field(0,1).type == Utils::Database::FieldIsUUID);
        QVERIFY(db.field(0,2).type == Utils::Database::FieldIsBoolean);
        QVERIFY(db.field(0,3).type == Utils::Database::FieldIsLongText);
        QVERIFY(db.field(0,4).type == Utils::Database::FieldIsShortText);
        QVERIFY(db.field(0,5).type == Utils::Database::FieldIsDateTime);
        QVERIFY(db.field(1,0).type == Utils::Database::FieldIsUniquePrimaryKey);
        QVERIFY(db.field(1,1).type == Utils::Database::FieldIsReal);

        QVERIFY(db.field(0,0).table == 0);
        QVERIFY(db.field(0,1).table == 0);
        QVERIFY(db.field(0,2).table == 0);
        QVERIFY(db.field(0,3).table == 0);
        QVERIFY(db.field(0,4).table == 0);
        QVERIFY(db.field(0,5).table == 0);
        QVERIFY(db.field(1,0).table == 1);
        QVERIFY(db.field(1,0).table == 1);

        QVERIFY(db.field(0,0).field == 0);
        QVERIFY(db.field(0,1).field == 1);
        QVERIFY(db.field(0,2).field == 2);
        QVERIFY(db.field(0,3).field == 3);
        QVERIFY(db.field(0,4).field == 4);
        QVERIFY(db.field(0,5).field == 5);
        QVERIFY(db.field(1,0).field == 0);
        QVERIFY(db.field(1,1).field == 1);

        QVERIFY(db.field(0,0).fieldName == "f0");
        QVERIFY(db.field(0,1).fieldName == "f1");
        QVERIFY(db.field(0,2).fieldName == "f2");
        QVERIFY(db.field(0,3).fieldName == "f3");
        QVERIFY(db.field(0,4).fieldName == "f4");
        QVERIFY(db.field(0,5).fieldName == "f5");
        QVERIFY(db.field(1,0).fieldName == "f1.0");
        QVERIFY(db.field(1,1).fieldName == "f1.1");

        QVERIFY(db.field(0,1).type == Utils::Database::FieldIsUUID);
        QVERIFY(db.field(0,2).type == Utils::Database::FieldIsBoolean);
        QVERIFY(db.field(0,3).type == Utils::Database::FieldIsLongText);
        QVERIFY(db.field(0,4).type == Utils::Database::FieldIsShortText);
        QVERIFY(db.field(0,5).type == Utils::Database::FieldIsDateTime);
        QVERIFY(db.field(1,0).type == Utils::Database::FieldIsUniquePrimaryKey);
        QVERIFY(db.field(1,1).type == Utils::Database::FieldIsReal);
    }

    void test_databaseCreation()
    {
        Utils::DatabaseConnector ctr;
        ctr.setAbsPathToReadWriteSqliteDatabase(_tmpPath);
        ctr.setAccessMode(Utils::DatabaseConnector::ReadWrite);
        ctr.setDriver(Utils::Database::SQLite);
        ctr.setClearLog("log");
        ctr.setClearPass("pass");
        QVERIFY(db.createConnection(_connectionName, "unit-test", ctr, Utils::Database::CreateDatabase));
    }

    void test_sqlInjection()
    {
        for(int i = 0; i < loop; ++i) {
            QDateTime dt = QDateTime::currentDateTime();
            QString longText = Utils::createUid();
            QString shortText = Utils::createUid();
            QString uid = Utils::createUid();

            // Insert to sql
            QSqlQuery query(db.database());
            query.prepare(db.prepareInsertQuery(0));
            query.bindValue(0, QVariant()); // PrimKey
            query.bindValue(1, uid); // UUID
            query.bindValue(2, 1); // boolean
            query.bindValue(3, longText);
            query.bindValue(4, shortText);
            query.bindValue(5, dt); // DateTime
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                QVERIFY(false);
            }
            int lastId = query.lastInsertId().toInt();
            QVERIFY(lastId == (i+1));
            query.finish();

            // Get from sql
            QHash<int, QString> where;
            where.insert(1, QString("='%1'").arg(uid));
            if (!query.exec(db.select(0, where))) {
                LOG_QUERY_ERROR(query);
                QVERIFY(false);
            }
            int n = 0;
            while (query.next()) {
                ++n;
                QVERIFY(query.value(0).toInt() == lastId);
                QVERIFY(query.value(1).toString() == uid);
                QVERIFY(query.value(2).toInt() == 1);
                QVERIFY(query.value(3).toString() == longText);
                QVERIFY(query.value(4).toString() == shortText);
                QVERIFY(query.value(5).toDateTime() == dt);
            }
            QVERIFY(n == 1);
        }
    }

    void test_sqlUpdate()
    {
        for(int i = 0; i < loop; ++i) {
            QDateTime dt = QDateTime::currentDateTime();
            QString longText = Utils::createUid();
            QString shortText = Utils::createUid();
            QString uid = Utils::createUid();

            // Insert to sql
            QSqlQuery query(db.database());
            query.prepare(db.prepareInsertQuery(0));
            query.bindValue(0, QVariant()); // PrimKey
            query.bindValue(1, uid); // UUID
            query.bindValue(2, 1); // boolean
            query.bindValue(3, longText);
            query.bindValue(4, shortText);
            query.bindValue(5, dt); // DateTime
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                QVERIFY(false);
            }
            int lastId = query.lastInsertId().toInt();
            query.finish();

            // Update sql: change boolean from 1 to 0
            QHash<int, QString> where;
            where.insert(1, QString("='%1'").arg(uid));
            query.prepare(db.prepareUpdateQuery(0, 2, where));
            query.bindValue(0, 0);
            QVERIFY(query.exec());
            query.finish();

            // Get from sql
            where.clear();
            where.insert(1, QString("='%1'").arg(uid));
            if (!query.exec(db.select(0, where))) {
                LOG_QUERY_ERROR(query);
                QVERIFY(false);
            }
            int n = 0;
            while (query.next()) {
                ++n;
                QVERIFY(query.value(0).toInt() == lastId);
                QVERIFY(query.value(1).toString() == uid);
                QVERIFY(query.value(2).toInt() == 0);
                QVERIFY(query.value(3).toString() == longText);
                QVERIFY(query.value(4).toString() == shortText);
                QVERIFY(query.value(5).toDateTime() == dt);
            }
            QVERIFY(n == 1);

            // Update sql: change boolean from 1 to 0
            where.clear();
            QDateTime newDt = QDateTime::currentDateTime();
            where.insert(1, QString("='%1'").arg(uid));
            query.prepare(db.prepareUpdateQuery(0, 5, where));
            query.bindValue(0, newDt);
            QVERIFY(query.exec());
            query.finish();

            // Get from sql
            where.clear();
            where.insert(1, QString("='%1'").arg(uid));
            if (!query.exec(db.select(0, where))) {
                LOG_QUERY_ERROR(query);
                QVERIFY(false);
            }
            n = 0;
            while (query.next()) {
                ++n;
                QVERIFY(query.value(0).toInt() == lastId);
                QVERIFY(query.value(1).toString() == uid);
                QVERIFY(query.value(2).toInt() == 0);
                QVERIFY(query.value(3).toString() == longText);
                QVERIFY(query.value(4).toString() == shortText);
                QVERIFY(query.value(5).toDateTime() == newDt);
            }
            QVERIFY(n == 1);
        }
    }

    void test_versionning()
    {
        Utils::Randomizer random;
        QString v = random.randomVersionNumber();
        Utils::Field vField(2,0);
        QVERIFY(db.setVersion(vField, v) == true);
        QVERIFY(db.checkVersion(vField, v) == true);
        for(int i = 0; i < loop; ++i) {
            QString wrong = v;
            while (wrong == v) {
                wrong = random.randomVersionNumber();
            }
            QVERIFY(db.checkVersion(vField, wrong) == false);
        }
        QString newV = v;
        while (newV == v) {
            newV = random.randomVersionNumber();
        }
        QVERIFY(db.setVersion(vField, newV) == true);
        QVERIFY(db.checkVersion(vField, newV) == true);
        for(int i = 0; i < loop; ++i) {
            QString wrong = newV;
            while (wrong == newV) {
                wrong = random.randomVersionNumber();
            }
            QVERIFY(db.checkVersion(vField, wrong) == false);
        }
    }

    void cleanupTestCase()
    {
        Utils::removeDirRecursively(QString("%1/freemedforms").arg(QDir::tempPath()));
    }
};

DECLARE_TEST(tst_UtilsDatabase)
#include "tst_database.moc"


