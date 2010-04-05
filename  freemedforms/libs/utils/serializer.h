/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#ifndef TKSERIALIZER_H
#define TKSERIALIZER_H

#include <utils/global_exporter.h>

#include <QString>
#include <QStringList>

namespace Utils {
namespace Constants {
const char * const  HASH_PREFIX           = "@HASH@@";          /*!< Constant prefix for hash serialization
                                                                     \ingroup constants_serializer
                                                                     \sa tkSerializer */
const char * const  STRINGLIST_PREFIX     = "@STRINGLIST@@";    /*!< Constant prefix for StringList serialization
                                                                     \ingroup constants_serializer
                                                                     \sa tkSerializer */
const char * const  SERIALIZER_SEPARATOR  = "@||@";             /*!< Constant separator used by tkSerializer
                                                                     \ingroup constants_serializer
                                                                     \sa tkSerializer */

}  // End Constants
}  // End Utils


/**
  \namespace tkSerializer
  \brief Serializer/deseroalizer for various Qt objects.
  \ingroup toolkit
  \ingroup object_toolkit
  \sa constants_serializer
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

}  // End Serializer
}  // End Utils

#endif // TKSERIALIZER_H
