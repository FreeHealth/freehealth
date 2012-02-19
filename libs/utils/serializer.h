/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef UTILS_SERIALIZER_H
#define UTILS_SERIALIZER_H

#include <utils/global_exporter.h>

#include <QString>
#include <QStringList>
QT_BEGIN_NAMESPACE
class QNetworkProxy;
QT_END_NAMESPACE

/**
  \namespace Utils::Serializer
  \brief Serializer/deserializer for various Qt objects.
*/

namespace Utils {
namespace Serializer {

UTILS_EXPORT const QString separator();
UTILS_EXPORT const QString openParenthese();
UTILS_EXPORT const QString closeParenthese();

// Serialize QStringList
UTILS_EXPORT const QString toString( const QStringList & list, bool base64Protection = false );
UTILS_EXPORT const QStringList toStringList( const QString & serialized, bool base64Protection = false );

// Serialize simples QHash
UTILS_EXPORT const QString toString( const QHash<int,QString> &hash, bool base64Protection = false );
UTILS_EXPORT const QString toString( const QHash<int,QVariant> &hash, bool base64Protection = false );
UTILS_EXPORT const QHash<int,QString> toHash( const QString &serialized, bool base64Protection = false );
UTILS_EXPORT const QHash<int,QVariant> toVariantHash( const QString &serialized, bool base64Protection = false );

// Serialize particular QHash
UTILS_EXPORT const QString threeCharKeyHashToString( const QHash<QString,QString> & hash, bool base64Protection = false );
UTILS_EXPORT const QHash<QString,QString> threeCharKeyHashToHash( const QString & serialized, bool base64Protection = false );

UTILS_EXPORT QString serializeProxy(const QNetworkProxy &proxy);
UTILS_EXPORT bool deserializeProxy(const QString &serializedString, QNetworkProxy &proxy);

}  // End Serializer
}  // End Utils

#endif // UTILS_SERIALIZER_H
