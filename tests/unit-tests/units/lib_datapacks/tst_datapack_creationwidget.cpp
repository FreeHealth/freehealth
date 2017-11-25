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

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <datapackutils/pack.h>
#include <datapackutils/packdescription.h>
#include <datapackutils/server.h>
#include <datapackutils/constants.h>
#include <datapackutils/servercreation/servercreationwidget.h>
#include <datapackutils/servercreation/packcreationqueue.h>


#include <quazip/JlCompress.h>

#include <QDebug>
#include <QTest>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QApplication>

/**
 * Unit-tests for server creation widget
 * - DataPack::ServerCreationWidget
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

QString createFakeContent(int size)
{
    QString content;
    while (content.size() < size)
        content += Utils::createUid();
    return content.left(size);
}

} // Anonymous namespace

class tst_DataPack_ServerCreationWidget : public QObject
{
    Q_OBJECT

    DataPack::ServerCreationWidget *_widget;
    const int loop = 10;
    QString _tempPath;
    Utils::Randomizer random;
    QHash<QString, QString> _tempPathToDescriptionPath; // Key= tmppath for the request equivalent path for pack description file ; Value = pack description file path
    QList<DataPack::PackCreationQueue *> queues;

    void createFakePackDescriptionAndContent(QString path, DataPack::RequestedPackCreation &request, DataPack::RequestedPackCreation::ContentType type = DataPack::RequestedPackCreation::UnzippedFile, uint numberOfContents = 1)
    {
        using namespace DataPack;
        path = QString("%1/%2").arg(path).arg(Utils::createUid());
        QVERIFY(QDir().mkpath(path) == true);

        // Create a Fake pack description file and content
        QString descrFile = QString("%1/%2").arg(path).arg(Constants::PACKDESCRIPTION_FILENAME);
        PackDescription descr;
        QHash<int, QString> values;
        populatePackDescription(descr, values);
        QVERIFY(Utils::saveStringToFile(descr.toXml(), descrFile) == true);

        // Create the request
        request.serverUid = Constants::SERVER_COMMUNITY_FREE;
        request.descriptionFilePath = descrFile;
        _tempPathToDescriptionPath.insert(path, request.descriptionFilePath);

        // Create a fake pack content
        for(int i = 0; i < numberOfContents; ++i) {
            if (type == RequestedPackCreation::UnzippedFile) {
                QString content;
                if (random.randomBool()) {
                    // create a subdir
                    content = QString("%1/%2/context_%3.txt").arg(path).arg(Utils::createUid()).arg(i);
                    QVERIFY(QDir().mkpath(QFileInfo(content).absolutePath()) == true);
                } else {
                    content = QString("%1/context_%2.txt").arg(path).arg(i);
                }
                QVERIFY(Utils::saveStringToFile(createFakeContent(1024 * (i+1)), content) == true);
                request.content.insert(RequestedPackCreation::UnzippedFile, content);
            } else if (type == RequestedPackCreation::ZippedFile) {
                QString content;
                if (random.randomBool()) {
                    // create a subdir
                    content = QString("%1/%2/context_%3.txt").arg(path).arg(Utils::createUid()).arg(i);
                    QVERIFY(QDir().mkpath(QFileInfo(content).absolutePath()) == true);
                } else {
                    content = QString("%1/context_%2.txt").arg(path).arg(i);
                }
                QString zipFileName = QString("%1/zipped_context_%2.zip").arg(path).arg(i);
                QVERIFY(Utils::saveStringToFile(createFakeContent(1024 * (i+1)), content) == true);
                QVERIFY(JlCompress::compressFiles(zipFileName, QStringList() << content) == true);
                QVERIFY(QFile(content).remove() == true);
                request.content.insert(RequestedPackCreation::ZippedFile, zipFileName);
            } else if (type == RequestedPackCreation::DirContent) {
                QString dir;
                if (random.randomBool()) {
                    // create a subdir
                    dir = QString("%1/%2/%3").arg(path).arg(Utils::createUid()).arg(Utils::createUid());
                    QVERIFY(QDir().mkpath(QFileInfo(dir).absolutePath()) == true);
                } else {
                    dir = QString("%1/%2").arg(path).arg(Utils::createUid());
                }
                QVERIFY(QDir().mkpath(dir));
                QString content = QString("%1/%2").arg(dir).arg("dir_content.txt");
                QVERIFY(Utils::saveStringToFile(createFakeContent(1024 * (i+1)), content) == true);
                request.content.insert(RequestedPackCreation::DirContent, dir);
            }
        }
    }

    // Queue file is created in the specified path while Pack are created in subdirs
    void createFakeQueue(const QString &path, DataPack::PackCreationQueue &queue)
    {
        using namespace DataPack;

        // Create Pack in the path
        for(int i=0; i < loop; ++i) {
            RequestedPackCreation request;
            createFakePackDescriptionAndContent(path, request);
            QVERIFY(queue.addToQueue(request) == true);
        }

        // Create the queue XML file
        QString queueFileName = QString("%1/%2")
                .arg(path)
                .arg(Constants::PACKCREATIONQUEUE_DEFAULT_FILENAME);
        QVERIFY(queue.saveToXmlFile(queueFileName) == true);
    }

private Q_SLOTS:
    void initTestCase()
    {
        Utils::Log::setDebugFileInOutProcess(false);
//        if (Utils::isRunningOnMac())
//            random.setPathToFiles(qApp->applicationDirPath() + "/../../../../../global_resources/textfiles/");
//        else
//            random.setPathToFiles(qApp->applicationDirPath() + "/../../global_resources/textfiles/");

        // Create temp path
        _tempPath = QString("%1/packcreationmodel_%2/").arg(QDir::tempPath()).arg(Utils::createUid());
        QVERIFY(QDir().mkpath(_tempPath) == true);
        qDebug() << "Queue test path" << _tempPath;

        using namespace DataPack;

        // Create widget
        qDebug() << "creating widget";
        _widget = new ServerCreationWidget;
        _widget->show();

        // Create multiple subpath with multiple Queue/Pack files
        for(int i=0; i < loop; ++i) {
            PackCreationQueue *queue = new PackCreationQueue;
            QString path = QString("%1/Q_%2").arg(_tempPath).arg(Utils::createUid());
            createFakeQueue(path, *queue);
            queues.append(queue);
        }
    }

    void test_Widget()
    {
        QVERIFY(_widget->addScreeningPath(_tempPath) == true);
    }

    void cleanupTestCase()
    {
        qDebug() << "deleting widget";
        // Causes segfault: delete _widget; _widget = 0;
        qDeleteAll(queues);
        // Clear the temp dir
        QVERIFY(Utils::removeDirRecursively(_tempPath, 0) == true);
    }
};

DECLARE_TEST(tst_DataPack_ServerCreationWidget)
#include "tst_datapack_creationwidget.moc"


