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
#include <utils/randomizer.h>
#include "../../autotest.h"

#include <QCryptographicHash>
#include <QTest>
#include <QDebug>
#include <QTest>

/**
 * Run test on:
 * - Utils::cryptPassword
 */

class tst_UtilsPasswordHashing : public QObject
{
    Q_OBJECT

    enum Algorithm {
        SHA1 = 0,
#if (QT_VERSION >= 0x050000)
        SHA256,
        SHA512,
#endif
#if (QT_VERSION >= 0x050100)
        SHA3_256,
        SHA3_512,
#endif
        ERROR
    };

    // Returns a crypted password using a defined algorithm. \n
    // This method uses destructive encryption. \n
    // The returned value is base64 encrypted and contains the used alogrithm
    // followed by the hash eg \e {algorithm}:{hash}. \n
    // Returns a null string in case of error.
    QString cryptPassword(const QString &toCrypt, Algorithm algo = SHA1)
    {
        if (algo == ERROR)
            return QString::null;
        QCryptographicHash::Algorithm qch_algo = QCryptographicHash::Sha1;
        QString prefix;
        switch (algo) {
        case SHA1:
            break;
#if (QT_VERSION >= 0x050000)
        case SHA256:
            qch_algo = QCryptographicHash::Sha256;
            prefix = "sha256";
            break;
        case SHA512:
            qch_algo = QCryptographicHash::Sha512;
            prefix = "sha512";
            break;
#endif
#if (QT_VERSION >= 0x050100)
        case SHA3_256:
            qch_algo = QCryptographicHash::Sha3_256;
            prefix = "sha3-256";
            break;
        case SHA3_512:
            qch_algo = QCryptographicHash::Sha3_512;
            prefix = "sha3-512";
            break;
#endif
        default: return QString::null;
        }
        QByteArray crypt = QCryptographicHash::hash(toCrypt.toUtf8(), qch_algo);
        if (!prefix.isEmpty())
            return QString("%1:%2")
                .arg(prefix)
                .arg(QString(crypt.toBase64()));
        return QString(crypt.toBase64());
    }

    // Extract the algorithm used to create the hashed password
    Algorithm extractHashAlgorithm(const QString &cryptedBase64)
    {
        QByteArray crypted = QByteArray::fromBase64(cryptedBase64.toAscii());
        // No prefix -> SHA1
        if (!crypted.contains(":"))
            return SHA1;
        // Compute prefix
        QString prefix = crypted.left(crypted.indexOf(":"));
        if (prefix == "sha1")
            return SHA1;
#if (QT_VERSION >= 0x050000)
        else if (prefix == "sha256")
            return SHA256;
        else if (prefix == "sha512")
            return SHA512;
#endif
#if (QT_VERSION >= 0x050100)
        else if (prefix == "sha3-256")
            return SHA3_256;
        else if (prefix == "sha3-512")
            return SHA3_512;
#endif
        return ERROR;
    }

    // Check the prefix of the crypted password using \e algo.
    bool checkPrefix(const QString &cryptedBase64, Algorithm algo = SHA1)
    {
        if (algo == ERROR)
            return false;
        return (extractHashAlgorithm(cryptedBase64) == algo);
    }

    // Checks equality between a clear password \e clear and
    // a crypted password (in base64 encoding) \e cryptedBase64.
    // The crypted password must have been created using the
    // cryptPassword().
    bool checkPassword(const QString &clear, const QString &cryptedBase64)
    {
        // Get from base64
        QByteArray crypted = QByteArray::fromBase64(cryptedBase64.toAscii());
        // Get the prefixed algorithm
        if (!crypted.contains(":")) {
            // SHA1
            QByteArray cryptClear = QCryptographicHash::hash(clear.toUtf8(), QCryptographicHash::Sha1);
            return (crypted == cryptClear);
        }
        Algorithm algo = extractHashAlgorithm(cryptedBase64);
        return (cryptPassword(clear, algo).compare(cryptedBase64) == 0);
    }

    const QString clearPass = "CryptMe_CryptMe_CryptMe_CryptMe_CryptMe";

private slots:
    void initTestCase()
    {
    }

    void test_passwordHashing_Qt48x()
    {
        // Qt4.8
        QString cryptedPass = cryptPassword(clearPass);
        QVERIFY(checkPrefix(cryptedPass, SHA1) == true);
        QVERIFY(checkPassword("Clear", cryptedPass) == false);
        QVERIFY(checkPassword("CryptMe_CryptMe", cryptedPass) == false);
        QVERIFY(checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
        QVERIFY(checkPassword(clearPass, cryptedPass) == true);
    }

#if (QT_VERSION >= 0x050000)
        void test_passwordHashing_Qt50x()
        {
            // Qt5.0
            QString cryptedPass = cryptPassword(clearPass, SHA256);
            // Check prefix
            QVERIFY(checkPrefix(cryptedPass, SHA1) == false);
            QVERIFY(checkPrefix(cryptedPass, SHA256) == true);
            QVERIFY(checkPrefix(cryptedPass, SHA512) == false);

            // Check password equality
            QVERIFY(checkPassword("Clear", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword(clearPass, cryptedPass) == true);

            cryptedPass = cryptPassword(clearPass, SHA512);
            // Check prefix
            QVERIFY(checkPrefix(cryptedPass, SHA1) == false);
            QVERIFY(checkPrefix(cryptedPass, SHA256) == false);
            QVERIFY(checkPrefix(cryptedPass, SHA512) == true);

            // Check password equality
            QVERIFY(checkPassword("Clear", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword(clearPass, cryptedPass) == true);
        }
#endif

#if (QT_VERSION >= 0x050100)
        void test_passwordHashing_Qt51x()
        {
            // Qt5.1
            QString cryptedPass = cryptPassword(clearPass, SHA3_256);
            QVERIFY(checkPassword("Clear", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword(clearPass, cryptedPass) == true);

            cryptedPass = cryptPassword(clearPass, SHA3_512);
            QVERIFY(checkPassword("Clear", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(checkPassword(clearPass, cryptedPass) == true);
        }
#endif


    void test_passwordHashLength()
    {
        // UserPlugin::Internal::UserBase uses a 200 chars length field
        // Crypted Password must never exceed this length
        // We can say that a user password is always < 100 chars (that
        // a really high level password)
        // FIXME: add a max length in the user password editor
        QString clearPass;
        Utils::Randomizer random;
        for(int i = 0; i < 100; ++i) {
            clearPass += random.randomString(1);

            // Qt4.8
            QVERIFY(cryptPassword(clearPass).length() < 200);

            // Qt5.0
#if (QT_VERSION >= 0x050000)
            QVERIFY(cryptPassword(clearPass, SHA256).length() < 200);
            QVERIFY(cryptPassword(clearPass, SHA512).length() < 200);
#endif

            // Qt5.1
#if (QT_VERSION >= 0x050100)
            QVERIFY(cryptPassword(clearPass, SHA3_256).length() < 200);
            QVERIFY(cryptPassword(clearPass, SHA3_512).length() < 200);
#endif
        }

        // Console output max length
        qDebug() << "SHA1     length" << cryptPassword(clearPass).length();
#if (QT_VERSION >= 0x050000)
        qDebug() << "SHA256   length" << cryptPassword(clearPass, SHA256).length();
        qDebug() << "SHA512   length" << cryptPassword(clearPass, SHA512).length();
#endif
#if (QT_VERSION >= 0x050100)
        qDebug() << "SHA3_256 length" << cryptPassword(clearPass, SHA3_256).length();
        qDebug() << "SHA3_512 length" << cryptPassword(clearPass, SHA3_512).length();
#endif
    }

    void cleanupTestCase()
    {
    }
};

DECLARE_TEST(tst_UtilsPasswordHashing)
#include "tst_passwordhashing.moc"


