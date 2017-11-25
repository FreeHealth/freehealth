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
#include <utils/databasecsvimportator.h>
#include <utils/global.h>
#include <utils/database.h>

#include "../../autotest.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QTest>

// %1 for absPath to tests dir from the bin path
// %1 for the number of unit test
static const char *CSV_FILE_PATH = "%1/tests/auto/libutils/autotests/lib_utils/csv_tests/Test%2/%3";

static const char *CREATE_DB_SQL = "createdb.sql";
static const char *CSV_FILE = "test.csv";
static const char *EXPECTED = "expected_result.txt";

/**
 * Run test on:
*/
class tst_UtilsDatabaseCsvImport : public QObject
{
    Q_OBJECT
public:
    QString absPath;

private slots:
    void initTestCase()
    {
        absPath = qApp->applicationDirPath();
        if (Utils::isRunningOnMac())
            absPath.append("/../../../");
        absPath.append("/../../");
    }

    void testDatabaseImport()
    {
        int i = 1;
        while (QDir(QString(CSV_FILE_PATH).arg(absPath).arg(i).arg("")).exists()) {
            QString connection = QString("UnitTest_%1").arg(i);
            // Create a fake db in memory
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection);
            db.setDatabaseName(":memory:");
            db.open();

            // Create databae structure
            QString sqlFileName = QString(CSV_FILE_PATH).arg(absPath).arg(i).arg(CREATE_DB_SQL);
            QString error;
            Utils::Database::executeSqlFile(connection, sqlFileName, &error);
            if (!error.isEmpty())
                qWarning() << error;
            QVERIFY(error.isEmpty());

            // Create the import job
            Utils::ImportationJob job;
            job.absFilePath = QString(CSV_FILE_PATH).arg(absPath).arg(i).arg(CSV_FILE);
            job.databaseConnectionName = connection;
            job.tableName = "UNIT_TEST";
            job.fieldSeparator = ',';
            job.textEscapingChar = '~';
            job.omitFirstLine = true;

            Utils::DatabaseCsvImportator import;
            import.addJob(job);
            Utils::ImportationResult result = import.startJobs();
            QVERIFY(result.hasError==false);

            // Read expected value
            QString content = Utils::readTextFile(QString(CSV_FILE_PATH).arg(absPath).arg(i).arg(EXPECTED), Utils::DontWarnUser);
            // Remove all comment lines --
            QStringList lines = content.split("\n");
            content.clear();
            bool first = true;
            QString separator;
            foreach(const QString &line, lines) {
                if (line.startsWith("--"))
                    continue;
                if (first) {
                    separator = line;
                    separator += "\n";
                    first = false;
                    continue;
                }
                content += line;
                content += "\n";
            }

            // Check all values
            QStringList values = content.split(separator, QString::KeepEmptyParts);
            QSqlQuery query(db);
            QString req;
            req = "SELECT * FROM `UNIT_TEST`";
            int id = 0;
            if (query.exec(req)) {
                while (query.next()) {
                    QCOMPARE(query.value(0).toString(), values.at(id));
                    ++id;
                    QCOMPARE(query.value(1).toString(), values.at(id));
                    ++id;
                    QCOMPARE(query.value(2).toString(), values.at(id));
                    ++id;
                }
            }

            QWARN(QString("--------------- Test succeeded: %1").arg(connection).toUtf8());
            ++i;
        }
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_UtilsDatabaseCsvImport)
#include "tst_databasecsvimport.moc"


