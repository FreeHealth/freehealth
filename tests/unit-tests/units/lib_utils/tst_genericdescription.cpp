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
#include <utils/genericdescription.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <utils/versionnumber.h>

#include "../../autotest.h"

#include <QDebug>
#include <QTest>

namespace {
class MyTest : public Utils::GenericDescription
{
public:
    enum NonTr {
        NonTr1 = NonTranslatableExtraData + 1,
        NonTr2,
        NonTr3,
        NonTr4,
        NonTr5
    };
    enum Tr {
        Tr1 = TranslatableExtraData + 1,
        Tr2,
        Tr3,
        Tr4,
        Tr5
    };

    MyTest() : Utils::GenericDescription("MyTest")
    {
        addNonTranslatableExtraData(NonTr1, "NonTr1");
        addNonTranslatableExtraData(NonTr2, "NonTr2");
        addNonTranslatableExtraData(NonTr3, "NonTr3");
        addNonTranslatableExtraData(NonTr4, "NonTr4");
        addNonTranslatableExtraData(NonTr5, "NonTr5");
        addTranslatableExtraData(Tr1, "Tr1");
        addTranslatableExtraData(Tr2, "Tr2");
        addTranslatableExtraData(Tr3, "Tr3");
        addTranslatableExtraData(Tr4, "Tr4");
        addTranslatableExtraData(Tr5, "Tr5");
    }
};
}

/**
 * Run test on:
 * - Utils::GenericDescription
 * - Utils::GenericInformation
*/
class tst_UtilsGenericDescription: public QObject
{
    Q_OBJECT
public:
    Utils::Randomizer r;
    const int loop = 1;

private slots:
    void initTestCase()
    {
        if (Utils::isRunningOnMac())
            r.setPathToFiles(qApp->applicationDirPath() + "/../../../../../global_resources/textfiles/");
        else
            r.setPathToFiles(qApp->applicationDirPath() + "/../../global_resources/textfiles/");
    }

    void test_genericdescription_basics()
    {
        QHash<int, QVariant> data;
        for(int i = 0; i < loop; ++i) {
            // non translatable data
            data.insert(Utils::GenericDescription::Uuid, Utils::createUid());
            data.insert(Utils::GenericDescription::Version, "0.9.0");
            data.insert(Utils::GenericDescription::Author, r.randomName());
            data.insert(Utils::GenericDescription::Country, r.randomString(r.randomInt(10)));
            data.insert(Utils::GenericDescription::URL, r.randomWords(r.randomInt(1, 10)));
            data.insert(Utils::GenericDescription::AbsFileName, r.randomString(r.randomInt(10, 100)));
            data.insert(Utils::GenericDescription::Vendor, r.randomWords(r.randomInt(1,10)));
            data.insert(Utils::GenericDescription::Validity, r.randomBool());
            data.insert(Utils::GenericDescription::EHRCompatVersion, r.randomVersionNumber());
            data.insert(Utils::GenericDescription::FreeDiamsCompatVersion, r.randomVersionNumber());
            data.insert(Utils::GenericDescription::FreeAccountCompatVersion, r.randomVersionNumber());
            data.insert(Utils::GenericDescription::CreationDate, r.randomDateTime(QDateTime::currentDateTime().addYears(-1)));
            data.insert(Utils::GenericDescription::LastModificationDate, r.randomDateTime(QDateTime::currentDateTime().addYears(-1)));
            data.insert(Utils::GenericDescription::GeneralIcon, r.randomString(r.randomInt(1, 10)));
            Utils::GenericDescription descr("MyTestingTag");
            Utils::GenericDescription descr2("MyTestingTag");
            QHashIterator<int, QVariant> it(data);
            while (it.hasNext()) {
                it.next();
                descr.setData(it.key(), it.value());
                QVERIFY(descr.data(it.key()) == it.value());
            }
            while (it.hasPrevious()) {
                it.previous();
                descr2.setData(it.key(), it.value());
                QVERIFY(descr2.data(it.key()) == it.value());
            }
            QVERIFY(descr == descr2);
            QVERIFY(!descr.toXml().isEmpty());
            descr2.fromXmlContent(descr.toXml());
            QVERIFY(descr == descr2);

            // translatable data
            QStringList lang;
            lang << "xx" << "fr" << "de" << "en" << "sp" << "za";
            foreach(const QString &l, lang) {
                descr.setData(Utils::GenericDescription::Label, r.randomWords(r.randomInt(1, 10)), l);
                descr.setData(Utils::GenericDescription::Category, r.randomWords(r.randomInt(1, 10)), l);
                if (r.randomBool() || l == "xx")
                    descr.setData(Utils::GenericDescription::ToolTip, r.randomWords(r.randomInt(1, 10)), l);
                descr.setData(Utils::GenericDescription::Specialties, r.randomWords(r.randomInt(1, 3)), l);
                descr.setData(Utils::GenericDescription::TypeName, r.randomWords(r.randomInt(1, 3)), l);
                descr.setData(Utils::GenericDescription::ShortDescription, r.randomWords(r.randomInt(1, 10)), l);
                descr.setData(Utils::GenericDescription::HtmlDescription, QString(r.randomWords(r.randomInt(1, 10)) + "<>&é\"'!%ù*$`£"), l);
                if (r.randomBool() || l == "xx")
                    descr.setData(Utils::GenericDescription::HtmlSynthesis, r.randomWords(r.randomInt(1, 10)), l);
                descr.setData(Utils::GenericDescription::LicenseName, r.randomWords(r.randomInt(1, 10)), l);
                descr.setData(Utils::GenericDescription::LicenseTerms, r.randomWords(r.randomInt(1, 10)), l);
                descr.setData(Utils::GenericDescription::WebLink, r.randomWords(r.randomInt(1, 10)), l);
            }
            QVERIFY(descr != descr2);
            QVERIFY(!descr.toXml().isEmpty());
            descr2.fromXmlContent(descr.toXml());
            QVERIFY(descr == descr2);
        }
    }

    void test_genericdescription_with_extra()
    {
        QHash<int, QVariant> data;
        for(int i = 0; i < loop; ++i) {
            // non translatable data
            data.insert(Utils::GenericDescription::Uuid, Utils::createUid());
            data.insert(Utils::GenericDescription::Version, "0.9.0");
            data.insert(Utils::GenericDescription::Author, r.randomName());
            data.insert(Utils::GenericDescription::Country, r.randomString(r.randomInt(10)));
            data.insert(Utils::GenericDescription::URL, r.randomWords(r.randomInt(1, 10)));
            data.insert(Utils::GenericDescription::AbsFileName, r.randomString(r.randomInt(10, 100)));
            data.insert(Utils::GenericDescription::Vendor, r.randomWords(r.randomInt(1,10)));
            data.insert(Utils::GenericDescription::Validity, r.randomBool());
            data.insert(Utils::GenericDescription::EHRCompatVersion, r.randomVersionNumber());
            data.insert(Utils::GenericDescription::FreeDiamsCompatVersion, r.randomVersionNumber());
            data.insert(Utils::GenericDescription::FreeAccountCompatVersion, r.randomVersionNumber());
            data.insert(Utils::GenericDescription::CreationDate, r.randomDateTime(QDateTime::currentDateTime().addYears(-1)));
            data.insert(Utils::GenericDescription::LastModificationDate, r.randomDateTime(QDateTime::currentDateTime().addYears(-1)));
            data.insert(Utils::GenericDescription::GeneralIcon, r.randomString(r.randomInt(1, 10)));
            // EXTRA
            data.insert(MyTest::NonTr1, r.randomString(r.randomInt(1, 10)));
            data.insert(MyTest::NonTr2, r.randomString(r.randomInt(1, 10)));
            data.insert(MyTest::NonTr3, r.randomString(r.randomInt(1, 10)));
            data.insert(MyTest::NonTr4, r.randomDate(2008));
            data.insert(MyTest::NonTr5, r.randomDateTime(QDateTime::currentDateTime().addYears(-10)));
            MyTest descr;
            MyTest descr2;
            QHashIterator<int, QVariant> it(data);
            while (it.hasNext()) {
                it.next();
                descr.setData(it.key(), it.value());
                QVERIFY(descr.data(it.key()) == it.value());
            }
            while (it.hasPrevious()) {
                it.previous();
                descr2.setData(it.key(), it.value());
                QVERIFY(descr2.data(it.key()) == it.value());
            }
            QVERIFY(descr == descr2);
            QVERIFY(!descr.toXml().isEmpty());
            descr2.fromXmlContent(descr.toXml());
            QVERIFY(descr == descr2);

            // translatable data
            QStringList lang;
            lang << "xx" << "fr" << "de" << "en" << "sp" << "za";
            foreach(const QString &l, lang) {
                descr.setData(Utils::GenericDescription::Label, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(Utils::GenericDescription::Label, descr.data(Utils::GenericDescription::Label, l), l);

                descr.setData(Utils::GenericDescription::Category, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(Utils::GenericDescription::Category, descr.data(Utils::GenericDescription::Category, l), l);

                if (r.randomBool() || l == "xx") {
                    descr.setData(Utils::GenericDescription::ToolTip, r.randomWords(r.randomInt(1, 10)), l);
                    descr2.setData(Utils::GenericDescription::ToolTip, descr.data(Utils::GenericDescription::ToolTip, l), l);
                }
                descr.setData(Utils::GenericDescription::Specialties, r.randomWords(r.randomInt(1, 3)), l);
                descr2.setData(Utils::GenericDescription::Specialties, descr.data(Utils::GenericDescription::Specialties, l), l);
                descr.setData(Utils::GenericDescription::TypeName, r.randomWords(r.randomInt(1, 3)), l);
                descr2.setData(Utils::GenericDescription::TypeName, descr.data(Utils::GenericDescription::TypeName, l), l);
                descr.setData(Utils::GenericDescription::ShortDescription, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(Utils::GenericDescription::ShortDescription, descr.data(Utils::GenericDescription::ShortDescription, l), l);
                descr.setData(Utils::GenericDescription::HtmlDescription, QString(r.randomWords(r.randomInt(1, 10)) + "<>&é\"'!%ù*$`£"), l);
                descr2.setData(Utils::GenericDescription::HtmlDescription, descr.data(Utils::GenericDescription::HtmlDescription, l), l);
                if (r.randomBool() || l == "xx") {
                    descr.setData(Utils::GenericDescription::HtmlSynthesis, r.randomWords(r.randomInt(1, 10)), l);
                    descr2.setData(Utils::GenericDescription::HtmlSynthesis, descr.data(Utils::GenericDescription::HtmlSynthesis, l), l);
                }
                descr.setData(Utils::GenericDescription::LicenseName, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(Utils::GenericDescription::LicenseName, descr.data(Utils::GenericDescription::LicenseName, l), l);
                descr.setData(Utils::GenericDescription::LicenseTerms, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(Utils::GenericDescription::LicenseTerms, descr.data(Utils::GenericDescription::LicenseTerms, l), l);
                descr.setData(Utils::GenericDescription::WebLink, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(Utils::GenericDescription::WebLink, descr.data(Utils::GenericDescription::WebLink, l), l);
                QVERIFY(descr == descr2);

                // EXTRA
                descr.setData(MyTest::Tr1, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(MyTest::Tr1, descr.data(MyTest::Tr1, l), l);
                QVERIFY(descr == descr2);

                descr.setData(MyTest::Tr2, r.randomWords(r.randomInt(1, 10)), l);
                descr.setData(MyTest::Tr3, r.randomWords(r.randomInt(1, 10)), l);
                descr2.setData(MyTest::Tr3, descr.data(MyTest::Tr3, l), l);
                descr2.setData(MyTest::Tr2, descr.data(MyTest::Tr2, l), l);
                QVERIFY(descr == descr2);
            }
            descr2.fromXmlContent(descr.toXml());
            QVERIFY(descr == descr2);
            QVERIFY(!descr2.data(MyTest::Tr1, "xx").toString().isEmpty());
            QVERIFY(!descr2.data(MyTest::Tr2, "xx").toString().isEmpty());
            QVERIFY(!descr2.data(MyTest::Tr3, "xx").toString().isEmpty());
        }
    }

    void test_updatedescription_xmlbasics()
    {
        // TODO: write this
        Utils::GenericUpdateInformation info1;
        Utils::VersionNumber ver1(r.randomVersionNumber());
        Utils::VersionNumber ver2(r.randomVersionNumber());
        if (ver1 > ver2) {
            info1.setToVersion(ver2.versionString());
            info1.setFromVersion(ver1.versionString());
        } else {
            info1.setToVersion(ver1.versionString());
            info1.setFromVersion(ver2.versionString());
        }
        info1.setAuthor(r.randomName());
        info1.setDateTime(r.randomDateTime(QDateTime::currentDateTime().addYears(-1)));
        info1.setText(r.randomWords(r.randomInt(1, 10)), "xx");
        info1.setText(r.randomWords(r.randomInt(1, 10)), "fr");
        info1.setText(r.randomWords(r.randomInt(1, 10)), "de");

        //    QDomDocument doc;
        //    QDomElement element = doc.createElement(xmlTagName());
        //    doc.appendChild(element);
        //    if (!toDomElement(&element, &doc))
        //        return QString::null;

//        qWarning() << info1.toXml();
//        QList<Utils::GenericUpdateInformation> fromXml = Utils::GenericUpdateInformation::fromXml(info1.toXml());
    }

    void cleanupTestCase()
    {
    }
};

// TODO: test with Utils::GenericUpdateInformation

DECLARE_TEST(tst_UtilsGenericDescription)
#include "tst_genericdescription.moc"



