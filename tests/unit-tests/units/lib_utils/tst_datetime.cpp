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

#include "../../autotest.h"

#include <QDebug>
#include <QTest>
#include <QCryptographicHash>

/**
 * Run test on:
 * QDateTime roundDateTime(const QDateTime &date, const int minutesRound);
 * bool inRange(const int min, const int max, const int value);
 * bool inRange(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QModelIndex &value);
*/
class tst_DateTime : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase()
    {
    }

    void testRoundDateTime()
    {
        QDateTime dt = QDateTime(QDate::currentDate(), QTime(12,12,12));
        QDateTime rd = QDateTime(QDate::currentDate(), QTime(12,15,00));
        QCOMPARE(Utils::roundDateTime(dt, 15), rd);

        dt = QDateTime(QDate::currentDate(), QTime(12,15,12));
        rd = QDateTime(QDate::currentDate(), QTime(12,30,00));
        QCOMPARE(Utils::roundDateTime(dt, 15), rd);

        dt = QDateTime(QDate::currentDate(), QTime(12,45,00,01));
        rd = QDateTime(QDate::currentDate(), QTime(13,00,00));
        QCOMPARE(Utils::roundDateTime(dt, 15), rd);

        dt = QDateTime(QDate::currentDate(), QTime(12,44,59,99));
        rd = QDateTime(QDate::currentDate(), QTime(12,45,00));
        QCOMPARE(Utils::roundDateTime(dt, 15), rd);

        dt = QDateTime(QDate::currentDate(), QTime(12,01,10,01));
        rd = QDateTime(QDate::currentDate(), QTime(12,15,00));
        QCOMPARE(Utils::roundDateTime(dt, 15), rd);

        int div = 15;
        int max = 60*div - 2;
        dt = QDateTime(QDate::currentDate(), QTime(12,0,1));
        for(int i = 0; i < max; ++i) {
            dt = dt.addMSecs(1000);
            rd = QDateTime(QDate::currentDate(), QTime(12,15,00));
            QCOMPARE(Utils::roundDateTime(dt, div), rd);
        }
        dt = dt.addMSecs(2000);
        rd = QDateTime(QDate::currentDate(), QTime(12,30,00));
        QCOMPARE(Utils::roundDateTime(dt, div), rd);
    }

    void testInRange()
    {
        // TODO: write me
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_DateTime)
#include "tst_datetime.moc"

