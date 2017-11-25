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
#include <utils/log.h>

#include "../../autotest.h"

#include <QDir>
#include <QString>
#include <QDebug>
#include <QTest>

/**
 * Run test on:
 * Utils::copyDir
*/
class tst_Utils_dirCopy : public QObject
{
    Q_OBJECT
public:
    const int loop = 2;
    QString _tempPath;
    int count;

    QString createFakeContent(int size)
    {
        QString content;
        while (content.size() < size)
            content += Utils::createUid();
        return content.left(size);
    }

    void createSubDirs(const QString &dir, int deep = 1)
    {
        for(int i=0; i < loop; ++i) {
            ++count;
            QString sub = QString("%1/%2").arg(dir).arg(count);
            QDir().mkpath(sub);
            QString file = createFakeContent(1024);
            Utils::saveStringToFile(file, QString("%1/dir_%2.txt").arg(sub).arg(count));
            if (deep > 1)
                createSubDirs(sub, deep-1);
        }
    }

private slots:
    void initTestCase()
    {
        Utils::Log::setDebugFileInOutProcess(false);
        // Create temp path
        _tempPath = QString("%1/unittest_%2/").arg(QDir::tempPath()).arg(Utils::createUid());
        QVERIFY(QDir().mkpath(_tempPath));
        count = 0;
        qDebug() << "DirCopy test path" << _tempPath;
    }

    void test_dirCopy()
    {
        // Create subdirs
        createSubDirs(_tempPath, 3);

        // Copy to another path
        QString to = QString("%1/unittest_copy_%2/").arg(QDir::tempPath()).arg(Utils::createUid());
        QVERIFY(Utils::copyDir(_tempPath, to) == true);
        QFileInfoList sourceFiles = Utils::getFiles(QDir(_tempPath));
        QFileInfoList destFiles = Utils::getFiles(QDir(to));
        foreach(const QFileInfo &src, sourceFiles) {
            bool ok = false;
            QString srcRel = QDir(_tempPath).relativeFilePath(src.absoluteFilePath());
            foreach(const QFileInfo &dest, destFiles) {
                QString destRel = QDir(to).relativeFilePath(dest.absoluteFilePath());
                if(srcRel == destRel) {
                    ok = true;
                    QVERIFY(Utils::fileMd5(src.absoluteFilePath()) == Utils::fileMd5(dest.absoluteFilePath()));
                    QVERIFY(src.size() == dest.size());
                    QVERIFY(src.created() == dest.created());
                    QVERIFY(src.isExecutable() == dest.isExecutable());
                    QVERIFY(src.isWritable() == dest.isWritable());
                    QVERIFY(src.lastModified() == dest.lastModified());
                    QVERIFY(src.lastRead() == dest.lastRead());
                    destFiles.removeAll(dest);
                    break;
                }
            }
            QVERIFY(ok == true);
        }

        // TEST: Copy inside the source tree path
        to = QDir::cleanPath(QString("%1/copy").arg(_tempPath));
        QVERIFY(Utils::copyDir(_tempPath, to) == true);
        sourceFiles = Utils::getFiles(QDir(_tempPath));
        destFiles = Utils::getFiles(QDir(to));
        foreach(const QFileInfo &src, sourceFiles) {
            bool ok = false;
            if (src.absoluteFilePath().startsWith(to))
                continue;
            QString srcRel = QDir(_tempPath).relativeFilePath(src.absoluteFilePath());
            foreach(const QFileInfo &dest, destFiles) {
                QString destRel = QDir(to).relativeFilePath(dest.absoluteFilePath());
                if(srcRel == destRel) {
                    ok = true;
                    QVERIFY(Utils::fileMd5(src.absoluteFilePath()) == Utils::fileMd5(dest.absoluteFilePath()));
                    QVERIFY(src.size() == dest.size());
                    QVERIFY(src.created() == dest.created());
                    QVERIFY(src.isExecutable() == dest.isExecutable());
                    QVERIFY(src.isWritable() == dest.isWritable());
                    QVERIFY(src.lastModified() == dest.lastModified());
                    QVERIFY(src.lastRead() == dest.lastRead());
                    destFiles.removeAll(dest);
                    break;
                }
            }
            QVERIFY(ok == true);
        }
    }

    void cleanupTestCase()
    {
        // Clear the temp dir
        QVERIFY(Utils::removeDirRecursively(_tempPath, 0) == true);
    }
};

DECLARE_TEST(tst_Utils_dirCopy)
#include "tst_dircopy.moc"


