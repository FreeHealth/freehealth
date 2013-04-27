/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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

#include "../../autotest.h"

#include <QDebug>
#include <QTest>
#include <QCryptographicHash>

/**
 * Run test on:
 * QString createXml( const QString &mainTag, const QHash<QString,QString> &data, const int indent = 0, const bool valueToBase64 = false );
 * bool    readXml( const QString &xmlContent, const QString &generalTag, QHash<QString,QString> &readData, const bool valueFromBase64 = false );
*/
class tst_UtilsXml : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase()
    {
    }

    void testHashFromToXml()
    {
        QHash<QString,QString> toXml;
        toXml.insert("TAG1", "VALUE IS A <SCRIPT>");
        toXml.insert("TAG2", "VALUE IS A 23423.2342");
        toXml.insert("TAG3", ";qsdf;&amp;&lt;&gt;");
        QString xml = Utils::createXml("MAIN", toXml, 2, false);
        QString control = "<MAIN>\n"
                "  <TAG1>VALUE IS A &lt;SCRIPT></TAG1>\n"
                "  <TAG2>VALUE IS A 23423.2342</TAG2>\n"
                "  <TAG3>;qsdf;&amp;amp;&amp;lt;&amp;gt;</TAG3>\n"
              "</MAIN>\n";
        QCOMPARE(xml, control);

        QHash<QString,QString> fromXml;
        Utils::readXml(xml, "MAIN", fromXml, false);
        QCOMPARE(toXml, fromXml);

        // Base64 encoded
        control = "<MAIN>\n"
                "  <TAG1>VkFMVUUgSVMgQSA8U0NSSVBUPg==</TAG1>\n"
                "  <TAG2>VkFMVUUgSVMgQSAyMzQyMy4yMzQy</TAG2>\n"
                "  <TAG3>O3FzZGY7JmFtcDsmbHQ7Jmd0Ow==</TAG3>\n"
                "</MAIN>\n";
        xml = Utils::createXml("MAIN", toXml, 2, true);
        QCOMPARE(xml, control);

        Utils::readXml(xml, "MAIN", fromXml, true);
        QCOMPARE(toXml, fromXml);
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_UtilsXml)
#include "tst_xml.moc"


