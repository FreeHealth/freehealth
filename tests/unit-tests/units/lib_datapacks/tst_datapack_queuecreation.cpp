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
#include <datapackutils/servercreation/packcreationqueue.h>
#include <datapackutils/servercreation/packcreationmodel.h>
#include <datapackutils/constants.h>

#include <QDebug>
#include <QTest>
#include <QDir>
#include <QFileInfo>
#include <QString>

/**
 * Unit-tests for datapack creation
 * - DataPack::PackCreationQueue
 * - DataPack::PackCreationModel
*/

namespace {

// Populate PackDescription with virtual values and store a copy of them is the hash
//static void populatePackDescription(DataPack::PackDescription &desc, QHash<int, QString> &values)
//{
//    // Fill Utils values
//    QString uid;
//    for(int i = 0; i < DataPack::PackDescription::MaxParam; ++i) {
//        uid = Utils::createUid();
//        values.insert(i, uid);
//        desc.setData(i, uid);
//    }

//    // Add PackDescription::ExtraNonTranslated
//    values.insert(DataPack::PackDescription::Size, "2349234");
//    desc.setData(DataPack::PackDescription::Size, "2349234");
//    uid = Utils::createUid();
//    values.insert(DataPack::PackDescription::UnzipToPath, uid);
//    desc.setData(DataPack::PackDescription::UnzipToPath, uid);
//    uid = Utils::createUid();
//    values.insert(DataPack::PackDescription::Md5, uid);
//    desc.setData(DataPack::PackDescription::Md5, uid);
//    uid = Utils::createUid();
//    values.insert(DataPack::PackDescription::Sha1, uid);
//    desc.setData(DataPack::PackDescription::Sha1, uid);
//    uid = Utils::createUid();
//    values.insert(DataPack::PackDescription::DataType, uid);
//    desc.setData(DataPack::PackDescription::DataType, uid);
//    uid = Utils::createUid();
//    values.insert(DataPack::PackDescription::InstalledFiles, uid);
//    desc.setData(DataPack::PackDescription::InstalledFiles, uid);
//}

// Populate a queue with real packdescription files from the sources
//     from global_resources/datapack_description
//static void populateQueueWithRealDescriptionFiles(DataPack::PackCreationQueue &queue)
//{
//    // Use real packdescription.xml files from global_resources (grPath)
//    QString grPath = QString("%1/global_resources/datapack_description").arg(SOURCES_ROOT_PATH);
//    QFileInfoList list = Utils::getFiles(QDir(grPath), "packdescription.xml");
//    foreach(const QFileInfo &info, list) {
//        DataPack::RequestedPackCreation request;
//        request.serverUid = DataPack::Constants::SERVER_COMMUNITY_FREE;
//        request.descriptionFilePath = info.absoluteFilePath();
//        // content -> key= type; value= content
//        request.content.insert(DataPack::RequestedPackCreation::UnzippedFile, info.absoluteFilePath()) ;
//        QVERIFY(queue.addToQueue(request) == true);  // files exists!
//    }
//}

QString createFakeContent(uint size)
{
    QString content;
    while (content.size() < size)
        content += Utils::createUid();
    return content.left(size);
}

} // Anonymous namespace

class tst_DataPack_QueueCreation : public QObject
{
    Q_OBJECT

    QString _tempPath;

private Q_SLOTS:

    void initTestCase()
    {
        // Create temp path
        _tempPath = QString("%1/packcreationqueue_%2/").arg(QDir::tempPath()).arg(Utils::createUid());
        QVERIFY(QDir().mkpath(_tempPath));
        qDebug() << "Queue test path" << _tempPath;


        // Create a queue with some real pack descriptions with real contents
        // DataPack::Constants::SERVER_COMMUNITY_FREE;

    }


    void cleanupTestCase()
    {
        // Clear the temp dir
        qDebug() << "Queue test path: clearing" << _tempPath;
        QVERIFY(Utils::removeDirRecursively(_tempPath, 0) == true);
    }
};

DECLARE_TEST(tst_DataPack_QueueCreation)
#include "tst_datapack_queuecreation.moc"


