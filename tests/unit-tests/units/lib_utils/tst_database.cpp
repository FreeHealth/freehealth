/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <utils/global.h>
#include <utils/database.h>
#include <utils/databaseconnector.h>

#include "../../autotest.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QTest>

/**
 * Run test on:
 * - Utils::Database
 */

class tst_UtilsDatabase : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
    }

    void test_databasePrefix()
    {
        Utils::Database::setDatabasePrefix("pref_");
        QVERIFY(Utils::Database::prefixedDatabaseName(Utils::Database::MySQL, "DbName") == "pref_fmf_DbName");
        QVERIFY(Utils::Database::prefixedDatabaseName(Utils::Database::PostSQL, "DbName") == "pref_fmf_DbName");
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

    void cleanupTestCase()
    {
    }
};

DECLARE_TEST(tst_UtilsDatabase)
#include "tst_database.moc"


