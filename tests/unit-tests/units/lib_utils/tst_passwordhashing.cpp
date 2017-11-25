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
#include <utils/passwordandlogin.h>
#include <utils/randomizer.h>
#include "../../autotest.h"

#include <QCryptographicHash>
#include <QTest>
#include <QDebug>
#include <QTest>

/**
 * Run test on:
 * - Utils::PasswordCrypter
 */

class tst_UtilsPasswordHashing : public QObject
{
    Q_OBJECT

    const QString clearPass = "CryptMe_CryptMe_CryptMe_CryptMe_CryptMe";
    Utils::PasswordCrypter crypter;

private slots:
    void initTestCase()
    {
    }

    void test_passwordHashing_defaultValue()
    {
#if (QT_VERSION < 0x050000)
        QVERIFY(Utils::PasswordCrypter::Default == Utils::PasswordCrypter::SHA1);
#elif (QT_VERSION >= 0x050000 && QT_VERSION < 0x050100)
        QVERIFY(Utils::PasswordCrypter::Default == Utils::PasswordCrypter::SHA512);
#else
        QVERIFY(Utils::PasswordCrypter::Default == Utils::PasswordCrypter::SHA3_512);
#endif
    }

    void test_passwordHashing_Qt48x()
    {
        // Qt4.8
        QString cryptedPass = crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA1);

        // Check prefix
        QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA1) == true);

        // Check password equality
        QVERIFY(crypter.checkPassword("Clear", cryptedPass) == false);
        QVERIFY(crypter.checkPassword("CryptMe_CryptMe", cryptedPass) == false);
        QVERIFY(crypter.checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
        QVERIFY(crypter.checkPassword(clearPass, cryptedPass) == true);
    }

#if (QT_VERSION >= 0x050000)
        void test_passwordHashing_Qt50x()
        {
            // Qt5.0
            QString cryptedPass = crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA256);

            // Check prefix
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA1) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA256) == true);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA512) == false);

            // Check password equality
            QVERIFY(crypter.checkPassword("Clear", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword(clearPass, cryptedPass) == true);

            cryptedPass = crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA512);
            // Check prefix
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA1) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA256) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA512) == true);

            // Check password equality
            QVERIFY(crypter.checkPassword("Clear", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword(clearPass, cryptedPass) == true);
        }
#endif

#if (QT_VERSION >= 0x050100)
        void test_passwordHashing_Qt51x()
        {
            // Qt5.1
            QString cryptedPass = crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA3_256);

            // Check prefix
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA1) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA256) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA512) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA3_256) == true);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA3_512) == false);

            // Check password equality
            QVERIFY(crypter.checkPassword("Clear", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword(clearPass, cryptedPass) == true);

            cryptedPass = crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA3_512);
            // Check prefix
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA1) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA256) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA512) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA3_256) == false);
            QVERIFY(crypter.checkPrefix(cryptedPass, Utils::PasswordCrypter::SHA3_512) == true);

            // Check password equality
            QVERIFY(crypter.checkPassword("Clear", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword("CryptMe_CryptMe_CryptMe", cryptedPass) == false);
            QVERIFY(crypter.checkPassword(clearPass, cryptedPass) == true);
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
            QVERIFY(crypter.cryptPassword(clearPass).length() < 200);

            // Qt5.0
#if (QT_VERSION >= 0x050000)
            QVERIFY(crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA256).length() < 200);
            QVERIFY(crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA512).length() < 200);
#endif

            // Qt5.1
#if (QT_VERSION >= 0x050100)
            QVERIFY(crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA3_256).length() < 200);
            QVERIFY(crypter.cryptPassword(clearPass, Utils::PasswordCrypter::SHA3_512).length() < 200);
#endif
        }

        // Utils::PasswordCrypter::SHA1     length 28
        // Utils::PasswordCrypter::SHA256   length 51
        // Utils::PasswordCrypter::SHA512   length 95
        // Utils::PasswordCrypter::SHA3_256 length 53
        // Utils::PasswordCrypter::SHA3_512 length 97
    }

    void test_passwordUpdate()
    {
        // Here we will test if we can update the code transparently
        // from the old user password to the new crypter

        // FIXME: write unit-tests for QString DatabaseConnector::cryptedPass() const (from old to new)
        // FIXME: test bool UserModel::setCurrentUser(const QString &clearLog, const QString &clearPassword, bool refreshCache, bool checkPrefValidity)

        QString clearPass;
        Utils::Randomizer random;
        for(int i = 0; i < 100; ++i) {
            clearPass += random.randomString(1);
            QString oldPass = Utils::cryptPassword(clearPass);

            // Qt4.8
            QVERIFY(crypter.checkPassword(clearPass, oldPass));

            // Qt5.0
#if (QT_VERSION >= 0x050000)
            QVERIFY(crypter.checkPassword(clearPass, oldPass));
#endif

            // Qt5.1
#if (QT_VERSION >= 0x050100)
            QVERIFY(crypter.checkPassword(clearPass, oldPass));
#endif
        }
    }

    void cleanupTestCase()
    {
    }
};

DECLARE_TEST(tst_UtilsPasswordHashing)
#include "tst_passwordhashing.moc"


