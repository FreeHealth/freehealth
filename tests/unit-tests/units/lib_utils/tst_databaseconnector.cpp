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
#include <utils/databaseconnector.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/randomizer.h>

#include "../../autotest.h"

#include <QDebug>
#include <QTest>

/**
 * Run test on:
 * Utils::DatabaseConnector
*/
class tst_UtilsDatabaseConnector : public QObject
{
    Q_OBJECT
public:
    const int loop = 100;
    Utils::Randomizer r;

private slots:
    void initTestCase()
    {
        if (Utils::isRunningOnMac())
            r.setPathToFiles(qApp->applicationDirPath() + "/../../../../../global_resources/textfiles/");
        else
            r.setPathToFiles(qApp->applicationDirPath() + "/../../global_resources/textfiles/");
    }

    void test_connector()
    {
        for(int i = 0; i < loop; ++i) {
            Utils::DatabaseConnector connector;
            QVERIFY(connector.isValid() == false);
            connector.setDriver(Utils::Database::SQLite);
            connector.setClearLog(r.randomName());
            connector.setClearPass(r.randomString(r.randomInt(10,20)));
            connector.setHost(QString("%1%2").arg(r.randomString(r.randomInt(10,20))).arg(":/.?&$"));
            connector.setPort(r.randomInt(0, 1000));
            connector.setAbsPathToReadOnlySqliteDatabase(r.randomString(r.randomInt(10,20)));
            connector.setAbsPathToReadWriteSqliteDatabase(r.randomString(r.randomInt(10,20)));
            connector.setAccessMode(Utils::DatabaseConnector::ReadWrite);
            connector.setGlobalDatabasePrefix(r.randomString(r.randomInt(10,20)));

            // Test settings serialization
            // We have a compilation option defined WITH_LOGINANDPASSWORD_CACHING
            // When this option is activated, the login & the password are not stored in the
            // Utils::DatabaseConnector serialization
            Utils::DatabaseConnector connector2;
            connector2.fromSettings(connector.forSettings());
            QVERIFY(connector.forSettings() == connector2.forSettings());
            QVERIFY(connector.driver() == connector2.driver());
            QVERIFY(connector.host() == connector2.host());
            QVERIFY(connector.port() == connector2.port());
            // QVERIFY(connector.absPathToSqliteReadOnlyDatabase() == connector2.absPathToSqliteReadOnlyDatabase());
            QVERIFY(connector.absPathToSqliteReadWriteDatabase() == connector2.absPathToSqliteReadWriteDatabase());
            QVERIFY(connector.accessMode() == connector2.accessMode());
            QVERIFY(connector.isDriverValid() == connector2.isDriverValid());
            QVERIFY(connector.useExactFile() == connector2.useExactFile());
            QVERIFY(connector.globalDatabasePrefix() == connector2.globalDatabasePrefix());

#ifdef WITH_LOGINANDPASSWORD_CACHING
            QVERIFY(connector.clearLog() == connector2.clearLog());
            QVERIFY(connector.clearPass() == connector2.clearPass());
            QVERIFY(connector.cryptedLog() == connector2.cryptedLog());
            QVERIFY(connector.cryptedPass() == connector2.cryptedPass());
            QVERIFY(connector2.clearLog().isEmpty() == false);
            QVERIFY(connector2.clearPass().isEmpty() == false);
            QVERIFY(connector2.cryptedLog().isEmpty() == false);
            QVERIFY(connector2.cryptedPass().isEmpty() == false);
#else
            QVERIFY(connector.clearLog() != connector2.clearLog());
            QVERIFY(connector.clearPass() != connector2.clearPass());
            QVERIFY(connector.cryptedLog() != connector2.cryptedLog());
            QVERIFY(connector.cryptedPass() != connector2.cryptedPass());
            QVERIFY(connector2.clearLog().isEmpty());
            QVERIFY(connector2.clearPass().isEmpty());
            QVERIFY(connector2.cryptedLog().isEmpty());
            QVERIFY(connector2.cryptedPass().isEmpty());
#endif

            // Test clear
            connector2.clear();
            QVERIFY(connector2.driver() == Utils::Database::SQLite);
            QVERIFY(connector2.clearLog().isEmpty());
            QVERIFY(connector2.clearPass().isEmpty());
            QVERIFY(connector2.cryptedLog().isEmpty());
            QVERIFY(connector2.cryptedPass().isEmpty());
            QVERIFY(connector2.host().isEmpty());
            QVERIFY(connector2.port() == -1);
            QVERIFY(connector2.absPathToSqliteReadOnlyDatabase().isEmpty());
            QVERIFY(connector2.absPathToSqliteReadWriteDatabase().isEmpty());
            QVERIFY(connector2.accessMode() == Utils::DatabaseConnector::ReadWrite);
            QVERIFY(connector2.isDriverValid() == true);
            QVERIFY(connector2.isValid() == false);
            QVERIFY(connector2.globalDatabasePrefix().isEmpty());

            // Test operator==()
            connector2.setDriver(connector.driver());
            connector2.setClearLog(connector.clearLog());
            connector2.setClearPass(connector.clearPass());
            connector2.setHost(connector.host());
            connector2.setPort(connector.port());
            connector2.setAbsPathToReadOnlySqliteDatabase(connector.absPathToSqliteReadOnlyDatabase());
            connector2.setAbsPathToReadWriteSqliteDatabase(connector.absPathToSqliteReadWriteDatabase());
            connector2.setAccessMode(connector.accessMode());
            connector2.setGlobalDatabasePrefix(connector.globalDatabasePrefix());
            QVERIFY(connector == connector2);

            // Test operator=()
            connector2.clear();
            connector2 = connector;
            QVERIFY(connector == connector2);

        }

        // TODO: Test old version (without globalprefix)
        // Utils::DatabaseConnector connector;
        // connector.fromSettings("Njg3NzExNjY3NzBkNzQ3OTZhMTU1ODM3NzcwZDFi");
        // connector.fromSettings("Njg3NzExNjY3NzBkNDc1NjMzNTkwNTAwNTg0NDVmNjYyMzUwNzYwNzUzNTM3NDZmNGM1NTM2MTYwZDYzNmU2YTAzMmE3NDZmNGM=");
        // qDebug() << connector;
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_UtilsDatabaseConnector)
#include "tst_databaseconnector.moc"


