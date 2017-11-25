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
#include <QCryptographicHash>

/**
 * Run test on:
 * Utils::saveStringToFile( const QString &toSave, const QString &toFile, IOMode mode = Overwrite, const Warn warnUser = WarnUser, QWidget *parent=0 );
 * Utils::saveStringToEncodedFile( const QString &toSave, const QString &toFile, const QString &forceEncoding, IOMode mode = Overwrite, const Warn warnUser = WarnUser, QWidget *parent=0 );
 * Utils::saveStringToFile( const QString &toSave,  const QString &dirPath, const QString &filters, QWidget *parent = 0 );
 * Utils::readTextFile( const QString &toRead, const QString &encoder, const Warn warnUser = WarnUser);
 * Utils::readTextFile( const QString &toRead, const Warn warnUser = WarnUser);
 * Utils::isDirExists(const QString &absPath);
 * Utils::isFileExists(const QString &absPath);
 * Utils::fileMd5(const QString &fileName);
 * Utils::fileSha1(const QString &fileName);
 * Utils::fileSha256(const QString &fileName);
*/
class tst_FileAccess : public QObject
{
    Q_OBJECT
public:
    QString fileName;
    QString content;

private slots:
    void initTestCase()
    {
        fileName = qApp->applicationDirPath() + "/test.txt";
        content = "abcdefghijklmnopqrstuvwxyz\nABCDEFGHIJHKLMNOPQRSTUVWXYZ\n";
        content += "<ACCENTS>\n";
        content += "  ÀàÄäâÂãÃ\n";
        content += "  éÉèÈëËêÊ\n";
        content += "  ïÏîÎìÌ\n";
        content += "  òÒöÖôÔ\n";
        content += "  ùÙüÜûÛ\n";
        content += "  ÿŸ\n";
        content += "  ñÑ\n";
        content += "</ACCENTS>\n";
        // Add more chars
    }

    void testBasicFileAccess()
    {
        // Remove existing file
        QFile(fileName).remove();

        // Save to a tmp file
        QStringList enc;
        enc << "UTF-8"
            << "UTF-16"
               // FIXME: all ISO tests will fail...
//            << "ISO 8859-1"
//            << "ISO 8859-2"
//            << "ISO 8859-3"
//            << "ISO 8859-4"
//            << "ISO 8859-5"
//            << "ISO 8859-6"
//            << "ISO 8859-7"
//            << "ISO 8859-8"
//            << "ISO 8859-9"
//            << "ISO 8859-10"
//            << "ISO 8859-13"
//            << "ISO 8859-14"
//            << "ISO 8859-15"
//            << "ISO 8859-16"
               ;
        foreach(const QString &e, enc) {
            QVERIFY(Utils::saveStringToEncodedFile(content, fileName, e, Utils::Overwrite, Utils::DontWarnUser) == true);
            QVERIFY(QFile(fileName).exists() == true);
            QCOMPARE(Utils::readTextFile(fileName, e, Utils::DontWarnUser), content);
            QFile(fileName).remove();
        }
        QVERIFY(Utils::saveStringToFile(content, fileName, Utils::Overwrite, Utils::DontWarnUser) == true);
        QVERIFY(QFile(fileName).exists() == true);
        QCOMPARE(Utils::readTextFile(fileName, Utils::DontWarnUser), content);

        // Test file
        QCOMPARE(Utils::isFileExists(fileName), fileName);
        QCOMPARE(Utils::isDirExists(QFileInfo(fileName).absolutePath()), QFileInfo(fileName).absolutePath());
        fileName += "_";
        QCOMPARE(Utils::isFileExists(fileName), QString());
        QCOMPARE(Utils::isDirExists(QFileInfo(fileName).absolutePath() + "_"), QString());
        fileName.chop(1);

        // Test Md5 & Sha1
        QVERIFY(QFile(fileName).exists() == true);
        QByteArray s = QCryptographicHash::hash(content.toUtf8(), QCryptographicHash::Md5);
        QCOMPARE(Utils::fileMd5(fileName), s.toHex());
        s = QCryptographicHash::hash(content.toUtf8(), QCryptographicHash::Sha1);
        QCOMPARE(Utils::fileSha1(fileName), s.toHex());
#if QT_VERSION >= 0x050000
        s = QCryptographicHash::hash(content.toUtf8(), QCryptographicHash::Sha256);
        QCOMPARE(Utils::fileSha256(fileName), s.toHex());
#endif

        // Remove existing file
        QFile(fileName).remove();
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_FileAccess)
#include "tst_fileaccess.moc"
