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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "passwordandlogin.h"

#include <utils/log.h>

#include <QCryptographicHash>
#include <QCoreApplication>

#include <QDebug>

enum {
    // Set to false if you don't want console log in the checkPassword() method
    DebugCheckPassword = false
};

/**
 * \class Utils::PasswordCrypter
 * Password encryption.
 * Note: unsalted passwords are insecure.
 * TODO: introduce PBKDF2 key derivation function. See RFC2898 http://tools.ietf.org/html/rfc2898#section-5.2
 * Standard library for PBKDF2 used in highly secure and reviewed projects such as Bitcoin is OpenSSL (C language).
 * See http://git.openssl.org/gitweb/?p=openssl.git;a=blob;f=crypto/evp/p5_crpt2.c
 */

namespace Utils {

/** Encrypt a clear login. */
QString loginForSQL(const QString &log)
{
    return log.toUtf8().toBase64();
}

/** Decrypt an encrypted login. */
QString loginFromSQL(const QVariant &sql)
{
    return QByteArray::fromBase64(sql.toByteArray());
}

/** Decrypt an encrypted login. */
QString loginFromSQL(const QString &sql)
{
    return QByteArray::fromBase64(sql.toUtf8());
}

/** Ctor */
PasswordCrypter::PasswordCrypter()
{}

/** Dtor */
PasswordCrypter::~PasswordCrypter()
{}

/**
 * Returns an encrypted password using a defined algorithm. \n
 * This method uses destructive encryption. \n
 * The returned value is base64 encrypted and contains the used algorithm
 * followed by the hash eg \e {algorithm}:{base64(hash)}. \n
 * Returns a null string in case of error.
 */
QString PasswordCrypter::cryptPassword(const QString &toCrypt, PasswordCrypter::Algorithm algo)
{
    if (algo == ERROR)
        return QString::null;
    QCryptographicHash::Algorithm qch_algo = QCryptographicHash::Sha1;
    QString prefix;
    switch (algo) {
    case SHA1:
        break;
    case SHA256:
        qch_algo = QCryptographicHash::Sha256;
        prefix = "sha256";
        break;
    case SHA512:
        qch_algo = QCryptographicHash::Sha512;
        prefix = "sha512";
        break;
    case SHA3_256:
        qch_algo = QCryptographicHash::Sha3_256;
        prefix = "sha3-256";
        break;
    case SHA3_512:
        qch_algo = QCryptographicHash::Sha3_512;
        prefix = "sha3-512";
        break;
    default: return QString::null;
    }
    QByteArray crypt = QCryptographicHash::hash(toCrypt.toUtf8(), qch_algo);
    if (!prefix.isEmpty()) {
        QString r = QString("%1:%2")
            .arg(prefix)
            .arg(QString(crypt.toBase64()));
        return r;
    }
    QString r = QString(crypt.toBase64());
    return r;
}

/**
 * Extracts the algorithm used to create the hashed password.
 */
PasswordCrypter::Algorithm PasswordCrypter::extractHashAlgorithm(const QString &cryptedBase64)
{
    // No prefix -> SHA1
    if (!cryptedBase64.contains(":"))
        return SHA1;
    // Compute prefix (in clear string at the begin of the password)
    QString prefix = cryptedBase64.left(cryptedBase64.indexOf(":"));
    if (prefix == "sha1")
        return SHA1;

    else if (prefix == "sha256")
        return SHA256;
    else if (prefix == "sha512")
        return SHA512;
    else if (prefix == "sha3-256")
        return SHA3_256;
    else if (prefix == "sha3-512")
        return SHA3_512;
    return ERROR;
}

/**
 * Checks the prefix of the encrypted password using \e algo.
 */
bool PasswordCrypter::checkPrefix(const QString &cryptedBase64, Algorithm algo)
{
    if (algo == ERROR)
        return false;
    return (extractHashAlgorithm(cryptedBase64) == algo);
}

/** Checks equality between a clear password \e clear and
 * a encrypted password (in base64 encoding) \e cryptedBase64.
 * The encrypted password must have been created using the
 * cryptPassword().
 */
bool PasswordCrypter::checkPassword(const QString &clear, const QString &cryptedBase64)
{
    if (DebugCheckPassword)
        WARN_FUNC;
    // Get the prefixed algorithm
    Algorithm algo = SHA1;
    if (cryptedBase64.contains(":")) {
        algo = extractHashAlgorithm(cryptedBase64);
    }
    QString crypted = cryptPassword(clear, algo);
    if (DebugCheckPassword) {
        qDebug() << QString("clear: %1; crypted: %2\nhash: %3; comparison: %4")
                    .arg(clear).arg(cryptedBase64).arg(algo).arg(crypted);
    }
    return (crypted.compare(cryptedBase64) == 0);
}

/**
 * String hashing using SHA1 algorithm.
 * The output is base64 encoded.
 * TODO: rename cryptPassword() to hashPassphrase()
 */
QString cryptPassword(const QString &toCrypt)
{
    // FIXME: How to improve the password security (currently using SHA1)
    // NOTE: Hash methods > SHA1 were introduced in Qt5
    // SEE: Issue #1 https://github.com/FreeHealth/freehealth/issues/1
    QCryptographicHash crypter(QCryptographicHash::Sha1);
    crypter.addData(toCrypt.toUtf8());
    return crypter.result().toBase64();
}

/**
 * String "encryption".
 * \sa decrypt()
*/
QByteArray nonDestructiveEncryption(const QString &text, const QString &key)
{
    QByteArray texteEnBytes = text.toUtf8();
    QString k = key;
    if (key.isEmpty()) {
        qDebug() << "Key is empty."
                   << "Using sha1 of: qApp->applicationName().left(qApp->applicationName().indexOf(\"_d\")).toUtf8() "
                   << qApp->applicationName().left(qApp->applicationName().indexOf("_d")).toUtf8();

        //k = QCryptographicHash::hash(qApp->applicationName().left(qApp->applicationName().indexOf("_d")).toUtf8(), QCryptographicHash::Sha1);
        // On FreeMedForms RDBMS parameters were "encrypted" with a key
        // equal to the application name (minus _d or _debug).
        // To ensure compatibility we will hardcode the key to "freemedforms"
        // before revamping encryption, hashing and security for FreeHealth
        k = QCryptographicHash::hash("freemedforms", QCryptographicHash::Sha1);
        qDebug() << "Key used: " << k;
    }
    QByteArray cle = k.toUtf8().toBase64();
    QByteArray codeFinal;
    int tailleCle = cle.length();
    for (int i = 0; i < texteEnBytes.length(); ++i) {
        codeFinal += char(texteEnBytes[i] ^ cle[i % tailleCle]);
    }
    return codeFinal.toHex().toBase64();
}

/**
 * Decrypt a string encrypted with the Utils::crypt() method
*/
QString decrypt(const QByteArray &texte, const QString &key)
{
    QByteArray texteEnBytes = QByteArray::fromHex(QByteArray::fromBase64(texte));
    QString k = key;
    if (key.isEmpty()) {
        //k = QCryptographicHash::hash(qApp->applicationName().left(qApp->applicationName().indexOf("_d")).toUtf8(), QCryptographicHash::Sha1);
        // On FreeMedForms RDBMS parameters were "encrypted" with a key
        // equal to the application name (minus _d or _debug).
        // To ensure compatibility we will hardcode the key to "freemedforms"
        // before revamping encryption, hashing and security for FreeHealth
        k = QCryptographicHash::hash("freemedforms", QCryptographicHash::Sha1);
    }
    QByteArray cle = k.toUtf8().toBase64();
    QByteArray codeFinal;
    int tailleCle = cle.length();
    for (int i = 0; i < texteEnBytes.length(); ++i) {
        codeFinal += char(texteEnBytes[i] ^ cle[i % tailleCle]);
    }
    return codeFinal;
}

} // namespace Utils
