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
#include <utils/httpdownloader.h>
#include <utils/httpmultidownloader.h>
#include <utils/global.h>
#include <utils/waitforsignal.h>

#include "../../autotest.h"

#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <QDir>
#include <QTest>

static const char * MAC_BUNDLE = "/../../../";

/**
 * Run test on:
 * - Utils::HttpDownloader
*/
class tst_HttpDownloader : public QObject
{
    Q_OBJECT
public:
    Utils::HttpDownloader *_downloader;
    Utils::HttpMultiDownloader *_multiDownloader;

private slots:
    void initTestCase()
    {
        _downloader = new Utils::HttpDownloader(this);
        _multiDownloader = new Utils::HttpMultiDownloader(this);
    }

    void testDownloaderConfig()
    {
        // TODO: test: download freemedforms.pro but file already exists in the outputPath

        QUrl url("http://freemedforms.googlecode.com/git/freemedforms.pro");
        QString outputPath = qApp->applicationDirPath() + "/http_test/";
        _downloader->setUrl(url);
        QVERIFY(url == _downloader->url());
        QVERIFY(_downloader->outputFileName() == "freemedforms.pro");

        // Test outputpath
        //    path does not exists
        if (QDir(outputPath).exists()) {
            QString error;
            QVERIFY(Utils::removeDirRecursively(outputPath, &error));
        }
        _downloader->setOutputPath(outputPath);
        QVERIFY2(_downloader->outputAbsoluteFileName() == QString("freemedforms.pro"), "Output path defined in HttpDownloader does not exist");

        //    path exists
        QVERIFY(QDir().mkpath(outputPath));
        _downloader->setOutputPath(outputPath);
        QVERIFY2(_downloader->outputAbsoluteFileName() == QString(outputPath + "freemedforms.pro"), "Output path defined in HttpDownloader does exist");

        // Start download
        _downloader->startDownload();
        QVERIFY(Utils::waitForSignal(_downloader, SIGNAL(downloadFinished()), 100000));

        // Verify downloaded content
        QVERIFY2(Utils::fileMd5(QString(outputPath + "freemedforms.pro")) == Utils::fileMd5(QString(qApp->applicationDirPath() + MAC_BUNDLE + "/../../freemedforms.pro")),
                 "Downloaded file MD5 == Local file MD5 (retry after a git pull)"
                 );

        qWarning() << "Utils::HttpDownloader successfully tested";
    }

    void testMultiDownloaderConfig()
    {
        // TODO: test: download freemedforms.pro but file already exists in the outputPath

        QUrl url1("http://freemedforms.googlecode.com/git/freemedforms.pro");
        QUrl url2("http://freemedforms.googlecode.com/git/freediams.pro");
        QList<QUrl> urls;
        urls << url1 << url2;
        _multiDownloader->setUrls(urls);

        QString outputPath = qApp->applicationDirPath() + "/http_test/";
        QString error;
        QVERIFY(Utils::removeDirRecursively(outputPath, &error));
        QVERIFY(QDir().mkpath(outputPath));
        _multiDownloader->setOutputPath(outputPath);

        // Start download
        _multiDownloader->startDownload();
        QVERIFY(Utils::waitForSignal(_multiDownloader, SIGNAL(allDownloadFinished()), 100000));

        // Verify downloaded content
        QVERIFY(QFile(QString(outputPath + "freemedforms.pro")).exists());
        QVERIFY(QFile(QString(outputPath + "freediams.pro")).exists());

        QVERIFY2(Utils::fileMd5(QString(outputPath + "freemedforms.pro")) == Utils::fileMd5(QString(qApp->applicationDirPath() + MAC_BUNDLE + "/../../freemedforms.pro")),
                 "Downloaded file MD5 == Local file MD5 (retry after a git pull)"
                 );
        QVERIFY2(Utils::fileMd5(QString(outputPath + "freediams.pro")) == Utils::fileMd5(QString(qApp->applicationDirPath() + MAC_BUNDLE + "/../../freediams.pro")),
                 "Downloaded file MD5 == Local file MD5 (retry after a git pull)"
                 );

        // Verify downloaded filenames
        QVERIFY(_multiDownloader->outputAbsoluteFileName(url1) == QString(outputPath + "freemedforms.pro"));
        QVERIFY(_multiDownloader->outputAbsoluteFileName(url2) == QString(outputPath + "freediams.pro"));

        qWarning() << "Utils::HttpMultiDownloader successfully tested";
    }

    void testMultiDownloaderWithXmlFile()
    {
        delete _multiDownloader;
        _multiDownloader = new Utils::HttpMultiDownloader(this);
        QUrl url1("http://freemedforms.googlecode.com/git/freemedforms.pro");
        QUrl url2("http://freemedforms.googlecode.com/git/freediams.pro");
        QList<QUrl> urls;
        urls << url1 << url2;
        _multiDownloader->setUrls(urls);
        _multiDownloader->setUseUidAsFileNames(true);

        QString outputPath = qApp->applicationDirPath() + "/http_test/";
        QString error;
        QVERIFY(Utils::removeDirRecursively(outputPath, &error));
        QVERIFY(QDir().mkpath(outputPath));
        _multiDownloader->setOutputPath(outputPath);

        // Start download
        _multiDownloader->startDownload();
        QVERIFY(Utils::waitForSignal(_multiDownloader, SIGNAL(allDownloadFinished()), 100000));

        // Save XML path description
        _multiDownloader->saveXmlUrlFileLinks();

        // Create another downloader
        Utils::HttpMultiDownloader *d2 = new Utils::HttpMultiDownloader(this);
        d2->setOutputPath(_multiDownloader->outputPath());
        // Read the xml file
        d2->readXmlUrlFileLinks();

        // Now both downloader should return the same values
        QVERIFY(_multiDownloader->urls() == d2->urls());
        foreach(const QUrl &url, d2->urls()) {
            QVERIFY(_multiDownloader->outputAbsoluteFileName(url) == d2->outputAbsoluteFileName(url));
            QVERIFY(_multiDownloader->lastErrorString(url) == d2->lastErrorString(url));
            QVERIFY(_multiDownloader->networkError(url) == d2->networkError(url));
        }
    }

    void cleanupTestCase()
    {
        delete _downloader;
        delete _multiDownloader;
    }
};

DECLARE_TEST(tst_HttpDownloader)
#include "tst_httpdownloader.moc"


