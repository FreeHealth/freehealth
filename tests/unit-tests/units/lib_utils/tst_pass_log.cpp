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

/**
 * Run test on:
 * QString createXml( const QString &mainTag, const QHash<QString,QString> &data, const int indent = 0, const bool valueToBase64 = false );
 * bool    readXml( const QString &xmlContent, const QString &generalTag, QHash<QString,QString> &readData, const bool valueFromBase64 = false );
*/
class tst_UtilsLoginAndPass : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase()
    {
    }

    void testLoginFunctions()
    {
        QString clear = "jhqsd_sdfkjh132234.45125kkjhoanlcvok";
        QString control = "amhxc2Rfc2Rma2poMTMyMjM0LjQ1MTI1a2tqaG9hbmxjdm9r";
        QCOMPARE(Utils::loginForSQL(clear), control);
        QCOMPARE(Utils::loginFromSQL(control), clear);
//        QCOMPARE(Utils::loginFromSQL(control), QVariant(clear));
    }

    void testPasswordFunctions()
    {
        QString clear = "jhqsd_sdfkjh132234.45125kkjhoanlcvok";
        QString key = "ijbkdsf2452kqjsnfaAERAz";
//        qWarning() << Utils::cryptPassword(clear)   // 7mR6sSYyStkB0yD6KkGcAHtE2m4=
//                   << Utils::crypt(clear)           // NWQ1ZjVhNGEyNzM1MWQxMjEwNWM1OTJiNWEwMzQ0NDQ0NTAzMWU0Mjc5NTQxOTFhMWQzMzA4MWUxOTU2NWUwOTU0NDE0NDUy
//                   << Utils::crypt(clear, key)      // MGIzZjAxMWEwNTZkMjExZTNjMDEyMzU4N2Y2Nzc4NDA1MDYzNWU0ZTU3NWM2ODVkM2EzZTNjM2IzZTM5MDE1MTAyMjExZjAy
                         ;
        QCOMPARE(Utils::cryptPassword(clear), QString("7mR6sSYyStkB0yD6KkGcAHtE2m4="));
//        QCOMPARE(Utils::crypt(clear), QByteArray("NWQ1ZjVhNGEyNzM1MWQxMjEwNWM1OTJiNWEwMzQ0NDQ0NTAzMWU0Mjc5NTQxOTFhMWQzMzA4MWUxOTU2NWUwOTU0NDE0NDUy"));
        QCOMPARE(Utils::crypt(clear, key), QByteArray("MGIzZjAxMWEwNTZkMjExZTNjMDEyMzU4N2Y2Nzc4NDA1MDYzNWU0ZTU3NWM2ODVkM2EzZTNjM2IzZTM5MDE1MTAyMjExZjAy"));

        QCOMPARE(Utils::decrypt(QByteArray("NWQ1ZjVhNGEyNzM1MWQxMjEwNWM1OTJiNWEwMzQ0NDQ0NTAzMWU0Mjc5NTQxOTFhMWQzMzA4MWUxOTU2NWUwOTU0NDE0NDUy")), clear);
        QCOMPARE(Utils::decrypt(QByteArray("MGIzZjAxMWEwNTZkMjExZTNjMDEyMzU4N2Y2Nzc4NDA1MDYzNWU0ZTU3NWM2ODVkM2EzZTNjM2IzZTM5MDE1MTAyMjExZjAy"), QString("ijbkdsf2452kqjsnfaAERAz")), clear);

    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_UtilsLoginAndPass)
#include "tst_pass_log.moc"



