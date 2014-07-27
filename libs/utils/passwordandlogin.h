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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_PASSWORDANDLOGIN_H
#define UTILS_PASSWORDANDLOGIN_H

#include <utils/global_exporter.h>
#include <QString>
#include <QByteArray>
#include <QVariant>

/**
 * \file passwordandlogin.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 27 Jul 2014
*/

namespace Utils {

UTILS_EXPORT QString loginForSQL(const QString &log);
UTILS_EXPORT QString loginFromSQL(const QVariant &sql);
UTILS_EXPORT QString loginFromSQL(const QString &sql);

// Old password crypting members
UTILS_EXPORT QString cryptPassword(const QString &toCrypt);

// Non destructive encryption
UTILS_EXPORT QByteArray nonDestructiveEncryption(const QString &text, const QString &key = QString::null);
UTILS_EXPORT QString decrypt(const QByteArray &text, const QString &key = QString::null);

class UTILS_EXPORT PasswordCrypter {
public:
    enum Algorithm {
        SHA1 = 0,
#if (QT_VERSION >= 0x050000)
        SHA256 = 1,
        SHA512 = 2,
#endif
#if (QT_VERSION >= 0x050100)
        SHA3_256 = 3,
        SHA3_512 = 4,
#endif
#if (QT_VERSION < 0x050000)
        Default = SHA1,
#elif (QT_VERSION >= 0x050000 && QT_VERSION < 0x050100)
        Default = SHA512,
#else
        Default = SHA3_512,
#endif
        ERROR = -1
    };

    PasswordCrypter();
    ~PasswordCrypter();

    QString cryptPassword(const QString &toCrypt, Algorithm algo = Default);
    Algorithm extractHashAlgorithm(const QString &cryptedBase64);
    bool checkPrefix(const QString &cryptedBase64, Algorithm algo = Default);
    bool checkPassword(const QString &clear, const QString &cryptedBase64);

};

} // namespace Utils

#endif // UTILS_PASSWORDANDLOGIN_H
