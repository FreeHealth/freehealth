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
#include <utils/versionnumber.h>

#include "../../autotest.h"

#include <QDebug>
#include <QTest>

Q_DECLARE_METATYPE(Utils::VersionNumber)

class tst_UtilsVersionNumber : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase()
    {
    }

    void testOperator_data()
    {
        QTest::addColumn<Utils::VersionNumber>("version1");
        QTest::addColumn<Utils::VersionNumber>("version2");

        QTest::newRow("test-major1") << Utils::VersionNumber("0.4.3")
                               << Utils::VersionNumber("0.4.5");

        QTest::newRow("test-major2") << Utils::VersionNumber("0.6.3")
                               << Utils::VersionNumber("0.8.1");

        QTest::newRow("test-major3") << Utils::VersionNumber("1.2.3")
                               << Utils::VersionNumber("3.1.5");

        QTest::newRow("test-alpha1") << Utils::VersionNumber("0.5.6~alpha2")
                               << Utils::VersionNumber("0.5.6~alpha3");

        QTest::newRow("test-alpha2") << Utils::VersionNumber("0.5.6~alpha2")
                               << Utils::VersionNumber("0.5.6~beta1");

        QTest::newRow("test-alpha3") << Utils::VersionNumber("0.5.6~alpha4")
                               << Utils::VersionNumber("0.5.6~RC");

        QTest::newRow("test-beta1") << Utils::VersionNumber("0.5.6~beta3")
                               << Utils::VersionNumber("0.5.6~beta4");

        QTest::newRow("test-beta2") << Utils::VersionNumber("0.5.6~beta3")
                               << Utils::VersionNumber("0.5.6~RC1");

        QTest::newRow("test-beta2") << Utils::VersionNumber("0.5.6~beta3")
                               << Utils::VersionNumber("0.5.6");

        QTest::newRow("test-rc1") << Utils::VersionNumber("0.5.6~rc3")
                               << Utils::VersionNumber("0.5.6~rc4");

        QTest::newRow("test-rc2") << Utils::VersionNumber("0.5.6~rc3")
                               << Utils::VersionNumber("0.5.6");
    }

    void testOperator()
    {
        QFETCH(Utils::VersionNumber, version1);
        QFETCH(Utils::VersionNumber, version2);

        QVERIFY((version1 > version2) == false);
        QVERIFY((version2 > version1) == true);

        QVERIFY((version1 < version2) == true);
        QVERIFY((version2 < version1) == false);

        QVERIFY((version1 >= version2) == false);
        QVERIFY((version2 >= version1) == true);

        QVERIFY((version1 <= version2) == true);
        QVERIFY((version2 <= version1) == false);

        QVERIFY((version2 != version1) == true);
    }

};

DECLARE_TEST(tst_UtilsVersionNumber)
#include "tst_versionnumber.moc"
