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
    }

    void cleanupTestCase()
    {
    }
};

DECLARE_TEST(tst_UtilsDatabase)
#include "tst_database.moc"


