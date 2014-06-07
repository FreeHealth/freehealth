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

#include "../../autotest.h"

#include <utils/global.h>
#include <datapackutils/pack.h>
#include <datapackutils/packdescription.h>
#include <datapackutils/server.h>

#include <QDebug>
#include <QTest>
#include <QCryptographicHash>

/**
 * Unit-tests for XML serialization of:
 * - DataPack::Pack
 * - DataPack::Server
*/

using namespace DataPack;

class tst_DataPack_Xml : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase()
    {
    }

    void testPackDescriptionToXml()
    {
        // Create a PackDescription (based on Utils::GenericDescription)
        DataPack::PackDescription desc;
        QHash<int, QString> values;

        // Fill Utils values
        QString uid;
        for(int i = 0; i < DataPack::PackDescription::MaxParam; ++i) {
            uid = Utils::createUid();
            values.insert(i, uid);
        }
        // Add PackDescription::ExtraNonTranslated
        values.insert(DataPack::PackDescription::Size, "2349234");
        desc.setData(DataPack::PackDescription::Size, "2349234");
        uid = Utils::createUid();
        values.insert(DataPack::PackDescription::UnzipToPath, uid);
        desc.setData(DataPack::PackDescription::UnzipToPath, uid);
        uid = Utils::createUid();
        values.insert(DataPack::PackDescription::Md5, uid);
        desc.setData(DataPack::PackDescription::Md5, uid);
        uid = Utils::createUid();
        values.insert(DataPack::PackDescription::Sha1, uid);
        desc.setData(DataPack::PackDescription::Sha1, uid);
        uid = Utils::createUid();
        values.insert(DataPack::PackDescription::DataType, uid);
        desc.setData(DataPack::PackDescription::DataType, uid);
        uid = Utils::createUid();
        values.insert(DataPack::PackDescription::InstalledFiles, uid);
        desc.setData(DataPack::PackDescription::InstalledFiles, uid);

        QString xml = desc.toXml();
        DataPack::PackDescription desc2;
        // desc2.fromXml(xml);

        // QCOMPARE(desc.toXml(), desc2.toXml());
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_DataPack_Xml)
//#include "tst_datapack_xml.moc"


