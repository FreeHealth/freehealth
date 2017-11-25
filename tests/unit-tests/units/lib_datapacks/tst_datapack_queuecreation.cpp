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
#include <datapackutils/servercreation/packcreationqueue.h>
#include <datapackutils/constants.h>

#include <quazip/JlCompress.h>

#include <QDebug>
#include <QTest>
#include <QDir>
#include <QFileInfo>
#include <QString>

/**
 * Unit-tests for datapack creation
 * - DataPack::PackCreationQueue
*/

// TODO: add tests for PackServerCreator

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

class tst_DataPack_QueueCreation : public QObject
{
    Q_OBJECT

    const int loop = 10;
    QString _tempPath;
    Utils::Randomizer random;
    QHash<QString, QString> _tempPathToDescriptionPath; // Key= tmppath for the request equivalent path for pack description file ; Value = pack description file path

    // Returns abspath to pack description file
    void createFakePackDescriptionAndContent(DataPack::RequestedPackCreation &request, DataPack::RequestedPackCreation::ContentType type = DataPack::RequestedPackCreation::UnzippedFile, uint numberOfContents = 1)
    {
        using namespace DataPack;
        // Create tmp path
        QString path = QString("%1/%2").arg(_tempPath).arg(Utils::createUid());
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

private Q_SLOTS:

    void initTestCase()
    {
        Utils::Log::setDebugFileInOutProcess(false);
//        if (Utils::isRunningOnMac())
//            random.setPathToFiles(qApp->applicationDirPath() + "/../../../../../global_resources/textfiles/");
//        else
//            random.setPathToFiles(qApp->applicationDirPath() + "/../../global_resources/textfiles/");

        // Create temp path
        _tempPath = QString("%1/packcreationqueue_%2/").arg(QDir::tempPath()).arg(Utils::createUid());
        QVERIFY(QDir().mkpath(_tempPath));
        qDebug() << "Queue test path" << _tempPath;

        // Create a queue with some real pack descriptions with real contents
        // DataPack::Constants::SERVER_COMMUNITY_FREE;

    }

    // PackCreationQueue
    // TEST: pack content creation / update with zipped files and mixed content

    void test_queue_isRelativePathFromDescriptionPathValid()
    {
        using namespace DataPack;
        RequestedPackCreation request;
        createFakePackDescriptionAndContent(request, RequestedPackCreation::UnzippedFile, 5);
        QString rootPath = QFileInfo(request.descriptionFilePath).absolutePath();
        QString testPath, relativePath;

        // Correct paths
        relativePath = "relativePath/file.txt";
        testPath = QString("%1/%2").arg(rootPath).arg(relativePath);
        QVERIFY(request.isRelativePathFromDescriptionPathValid(testPath) == true);
        QVERIFY(request.relativePathFromDescriptionPath(testPath) == relativePath);

        relativePath = "relativePath/To/Another/file.bin";
        testPath = QString("%1/%2").arg(rootPath).arg(relativePath);
        QVERIFY(request.isRelativePathFromDescriptionPathValid(testPath) == true);
        QVERIFY(request.relativePathFromDescriptionPath(testPath) == relativePath);

        relativePath = "file.bin";
        testPath = QString("%1/%2").arg(rootPath).arg(relativePath);
        QVERIFY(request.isRelativePathFromDescriptionPathValid(testPath) == true);
        QVERIFY(request.relativePathFromDescriptionPath(testPath) == relativePath);

        // Uncorrects path
        relativePath = "../relativePath/file.txt";
        testPath = QString("%1/%2").arg(rootPath).arg(relativePath);
        QVERIFY(request.isRelativePathFromDescriptionPathValid(testPath) == false);

        relativePath = "../file.txt";
        testPath = QString("%1/%2").arg(rootPath).arg(relativePath);
        QVERIFY(request.isRelativePathFromDescriptionPathValid(testPath) == false);
    }

    void test_queue_createZippedContent_UnzippedFiles()
    {
        using namespace DataPack;
        QHash<QString, QString> contentToMd5;

        // Create fake queue with UnzippedFile only
        PackCreationQueue queue;
        for(int i = 0; i < loop; ++i) {
            // Create a request with Unzipped files
            RequestedPackCreation request;
            createFakePackDescriptionAndContent(request, RequestedPackCreation::UnzippedFile, 5);
            QVERIFY(queue.addToQueue(request) == true);

            // Get all MD5 checksum of content files
            foreach(const QString &file, request.content.values(RequestedPackCreation::UnzippedFile)) {
                contentToMd5.insert(file, Utils::fileMd5(file));
            }

            // Create request zipped content
            QString contentPath = QFileInfo(request.descriptionFilePath).absolutePath();
            QString zip = QString("%1/content.zip").arg(contentPath);
            QVERIFY(queue.createZippedContent(request, zip));
            QVERIFY(QFile(zip).exists() == true);

            // Remove all original content files
            foreach(const QString &file, request.content.values(RequestedPackCreation::UnzippedFile)) {
                QVERIFY(QFile(file).remove() == true);
            }

            // Unzip content file
            QStringList files;
            files = JlCompress::extractFiles(zip, JlCompress::getFileList(zip), contentPath);
            QVERIFY(files.isEmpty() == false);
            // Check all MD5
            foreach(const QString &file, files) {
                QString md5 = Utils::fileMd5(file);
                QString source = contentToMd5.key(md5);
                // source filename exists
                QVERIFY(source.isEmpty() == false);
                // filename of the extracted file is the same as the source file
                QVERIFY(QFileInfo(source).fileName() == QFileInfo(file).fileName());
                // relative path (from the pack description file) of the extracted file is the same as the source file
                QVERIFY(request.relativePathFromDescriptionPath(source) == QDir(_tempPathToDescriptionPath.key(request.descriptionFilePath)).relativeFilePath(file));
            }
        }
    }

    void test_queue_createZippedContent_DirContents()
    {
        using namespace DataPack;
        QHash<QString, QString> contentToMd5;

        // Create fake queue with DirContent only
        PackCreationQueue queue;
        for(int i = 0; i < loop; ++i) {
            // Create a request with DirContent
            RequestedPackCreation request;
            createFakePackDescriptionAndContent(request, RequestedPackCreation::DirContent, 1);
            QVERIFY(queue.addToQueue(request) == true);

            // Get all MD5 checksum of content files
            foreach(const QString &file, request.content.values(RequestedPackCreation::DirContent)) {
                QFileInfoList fileContents = Utils::getFiles(QDir(file));
                foreach(const QFileInfo &content, fileContents)
                    contentToMd5.insert(content.absoluteFilePath(), Utils::fileMd5(content.absoluteFilePath()));
            }

            // Create request zipped content
            QString contentPath = QFileInfo(request.descriptionFilePath).absolutePath();
            QString zip = QString("%1/content.zip").arg(contentPath);
            QVERIFY(queue.createZippedContent(request, zip));
            QVERIFY(QFile(zip).exists() == true);

            // Remove all original content files
            foreach(const QString &file, request.content.values(RequestedPackCreation::UnzippedFile)) {
                QVERIFY(QFile(file).remove() == true);
            }

            // Unzip content file
            QStringList files;
            files = JlCompress::extractFiles(zip, JlCompress::getFileList(zip), contentPath);
            QVERIFY(files.isEmpty() == false);
//            QVERIFY(QuaZipTools::unzipFile(zip, contentPath) == true);

            // Check all MD5
            foreach(const QFileInfo &file, Utils::getFiles(contentPath, "*.txt")) {
                QString md5 = Utils::fileMd5(file.absoluteFilePath());
                QString source = contentToMd5.key(md5);
                // source filename exists
                QVERIFY(source.isEmpty() == false);
                // filename of the extracted file is the same as the source file
                QVERIFY(QFileInfo(source).fileName() == file.fileName());
                // relative path (from the pack description file) of the extracted file is the same as the source file
                QVERIFY(request.relativePathFromDescriptionPath(source) == QDir(_tempPathToDescriptionPath.key(request.descriptionFilePath)).relativeFilePath(file.absoluteFilePath()));
            }
        }
    }

    void test_queue_containsPackDescriptionFile()
    {
        using namespace DataPack;

        // Create fake queue
        PackCreationQueue queue;
        for(int i = 0; i < 10; ++i) {
            RequestedPackCreation request;
            createFakePackDescriptionAndContent(request);
            QVERIFY(queue.addToQueue(request) == true);
        }

        // Test: containsPackDescriptionFile()
        foreach(const RequestedPackCreation &request, queue.queue()) {
            QVERIFY(queue.containsPackDescriptionFile(request.descriptionFilePath) == true);
            QVERIFY(queue.containsPackDescriptionFile(request.content.values().at(0)) == false);
        }
    }

    // TODO: write test for PackCreationQueue::queueToServer()

    void cleanupTestCase()
    {
        // Clear the temp dir
        QVERIFY(Utils::removeDirRecursively(_tempPath, 0) == true);
    }
};

DECLARE_TEST(tst_DataPack_QueueCreation)
#include "tst_datapack_queuecreation.moc"


