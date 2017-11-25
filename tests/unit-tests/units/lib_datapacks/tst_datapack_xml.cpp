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

#include <utils/global.h>
#include <datapackutils/pack.h>
#include <datapackutils/packdescription.h>
#include <datapackutils/server.h>
#include <datapackutils/servercreation/packcreationqueue.h>
#include <datapackutils/constants.h>
#include <datapackutils/servercontent.h>

#include <QDebug>
#include <QTest>
#include <QDir>
#include <QFileInfo>

/**
 * Unit-tests for XML serialization of:
 * - DataPack::Pack
 * - DataPack::Server
*/

namespace {

// Populate PackDescription with virtual values and store a copy of them is the hash
static void populatePackDescription(DataPack::PackDescription &desc, QHash<int, QString> &values)
{
    // Fill Utils values
    QString uid;
    for(int i = 0; i < DataPack::PackDescription::MaxParam; ++i) {
        uid = Utils::createUid();
        values.insert(i, uid);
        desc.setData(i, uid);
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
}

// Populate a queue with real packdescription files from the sources
//     from global_resources/datapack_description
static void populateQueueWithRealDescriptionFiles(DataPack::PackCreationQueue &queue)
{
    // Use real packdescription.xml files from global_resources (grPath)
    QString grPath = QString("%1/global_resources/datapack_description").arg(SOURCES_ROOT_PATH);
    QFileInfoList list = Utils::getFiles(QDir(grPath), "packdescription.xml");
    foreach(const QFileInfo &info, list) {
        DataPack::RequestedPackCreation request;
        request.serverUid = DataPack::Constants::SERVER_COMMUNITY_FREE;
        request.descriptionFilePath = info.absoluteFilePath();
        // content -> key= type; value= content
        request.content.insert(DataPack::RequestedPackCreation::UnzippedFile, info.absoluteFilePath()) ;
        QVERIFY(queue.addToQueue(request) == true);  // files exists!
    }
}

}

class tst_DataPack_Xml : public QObject
{
    Q_OBJECT
public:
    tst_DataPack_Xml() {}

private Q_SLOTS:
    void initTestCase()
    {
    }

    void testPackDescriptionFromToXml()
    {
        using namespace DataPack;

        // Create a PackDescription (based on Utils::GenericDescription)
        DataPack::PackDescription desc;
        QHash<int, QString> values;
        populatePackDescription(desc, values);

        // Test XML transformation
        QString xml = desc.toXml();
        DataPack::PackDescription desc2;
        desc2.fromXmlContent(xml);
        QCOMPARE(desc.toXml(), desc2.toXml());
        foreach(const int key, values.uniqueKeys()) {
            QCOMPARE(desc2.data(key).toString(), values.value(key));
        }

        // Test operator==()
        QVERIFY(desc == desc2);
    }

    void testPackDependenciesFromToXml()
    {
        // TODO: Write this code
    }

    void testPackFromToXml()
    {
        // Create a PackDescription (based on Utils::GenericDescription)
        DataPack::PackDescription desc;
        QHash<int, QString> values;
        populatePackDescription(desc, values);

        // Create a Pack
        DataPack::Pack pack;
        pack.setPackDescription(desc);
        QVERIFY(pack.isValid() == true);

        // Test from/to XML
        QString xml = pack.toXml();
        QString fileName = QDir::tempPath() + "/ut_"+Utils::createUid();
        QVERIFY(Utils::saveStringToFile(xml, fileName) == true);
        DataPack::Pack pack2;
        pack2.fromXmlFile(fileName);
        QVERIFY(pack == pack2);
        QVERIFY(QFile(fileName).remove() == true);
    }

    void testPackCreationQueueFromToXml()
    {
        // Create a PackCreationQueue
        DataPack::PackCreationQueue queue;

        DataPack::RequestedPackCreation request;
        request.serverUid = DataPack::Constants::SERVER_COMMUNITY_FREE;
        request.descriptionFilePath = QString("%1/%2").arg(qApp->applicationDirPath()).arg(DataPack::Constants::PACKDESCRIPTION_FILENAME);
        // content -> key= type; value= content
        request.content.insert(DataPack::RequestedPackCreation::DirContent, qApp->applicationDirPath()) ;
        request.content.insert(DataPack::RequestedPackCreation::ZippedFile, qApp->applicationDirPath() + "/zipped.zip") ;
        request.content.insert(DataPack::RequestedPackCreation::UnzippedFile, qApp->applicationDirPath() + "/test.xml") ;
        QVERIFY(queue.addToQueue(request) == false);  // files does not exist!
        request.content.clear();

        // Use real packdescription.xml files from global_resources
        populateQueueWithRealDescriptionFiles(queue);

        // Test from/to XML
        QString fileName = QDir::tempPath() + "/ut_"+Utils::createUid();
        QVERIFY(queue.sourceAbsolutePathFile() != fileName);
        QVERIFY(queue.sourceAbsolutePathFile().isEmpty());
        QVERIFY(queue.saveToXmlFile(fileName, false) == true);
        QVERIFY(queue.sourceAbsolutePathFile() == fileName);

        DataPack::PackCreationQueue queue2;
        queue2.fromXmlFile(fileName);
        QVERIFY(queue == queue2);
        QVERIFY(QFile(fileName).remove() == true);
    }

    void testServerContentXml()
    {
        DataPack::ServerContent content;
        QVERIFY(content.packDescriptionFileNames().isEmpty() == true);
        QVERIFY(content.addPackRelativeFileName("Content1") == true);
        QVERIFY(content.addPackRelativeFileName("Content2") == true);
        QVERIFY(content.addPackRelativeFileName("Content3") == true);
        QVERIFY(content.addPackRelativeFileName("Content3") == false);
        QStringList test;
        test << "Content1" << "Content2" << "Content3";
        QVERIFY(content.packDescriptionFileNames().count() == 3);
        QVERIFY(content.packDescriptionFileNames() == test);

        // From/ToXML
        DataPack::ServerContent content2;
        content2.fromXml(content.toXml());
        QVERIFY(content2.packDescriptionFileNames().count() == 3);
        QVERIFY(content2.packDescriptionFileNames() == test);

        content.clear();
        QVERIFY(content.packDescriptionFileNames().isEmpty() == true);
        content2.clear();
        QVERIFY(content2.packDescriptionFileNames().isEmpty() == true);
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_DataPack_Xml)
#include "tst_datapack_xml.moc"


