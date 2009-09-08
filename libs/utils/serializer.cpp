/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#include "serializer.h"
#include <utils/log.h>

#include <QApplication>

using namespace Utils;
using namespace Utils::Constants;

namespace Utils {
namespace Serializer {

/**
  \brief Official separator for the serializer engine.
*/
const QString separator()
{
    return QString( SERIALIZER_SEPARATOR );
}

/**
  \brief Serialize a QStringList into a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QString toString( const QStringList & list, bool base64Protection )
{
    if (!base64Protection)
        return list.join( Serializer::separator() );
    QString tmp;
    foreach( const QString & s, list ) {
        tmp += s.toAscii().toBase64() + Serializer::separator();
    }
    if (!tmp.isEmpty())
        tmp.chop(Serializer::separator().length());
    return tmp;
}


/**
  \brief Deserialize a QStringList from a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QStringList toStringList( const QString & serialized, bool base64Protection )
{
    if (!base64Protection)
        return serialized.split( Serializer::separator() );
    QStringList toReturn;
    foreach( const QString &s, serialized.split(Serializer::separator()) ) {
        toReturn << QByteArray::fromBase64(s.toAscii());
    }
    return toReturn;
}

/**
  \brief Serialize a QHash<int, QString> into a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QString toString( const QHash<int,QString> & hash, bool base64Protection )
{
    QString tmp;
    tmp = HASH_PREFIX;
    if (!base64Protection) {
        // Add key + Separator + value + Separator
        foreach( int i, hash.keys() ) {
            tmp += QString::number(i) + separator() + hash.value(i) + separator();
        }
        //        tmp << hash;
        return tmp;
    }
    QString val;
    foreach( int i, hash.keys() ) {
        val = hash.value(i);
        tmp += QString::number(i) + separator() + val.toAscii().toBase64() + separator();
    }
    return tmp;
}

/**
  \brief Deserialize a QHash<int, QString> from a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QHash<int,QString> toHash( const QString & serialized, bool base64Protection )
{
    QHash<int,QString> toReturn;
    if (!serialized.startsWith(HASH_PREFIX)) {
        Log::addError( "Serializer", QCoreApplication::translate("Serializer", "Can not deserialize to QHash.") );
        return toReturn;
    }
    QString ser = serialized.mid( QString(HASH_PREFIX).length() );
    QStringList list = ser.split( separator() );
    QStringListIterator it(list);
    QString val;
    QString k;
    if (base64Protection) {
        while (it.hasNext()) {
            val = it.next().toAscii();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val.toInt(), QByteArray::fromBase64( k.toAscii() ) );
            }
        }
    } else {
        while (it.hasNext()) {
            val = it.next().toAscii();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val.toInt(), k );
            }
        }
    }
    return toReturn;
}

/**
  \brief obsolete
  \obsolete
*/
const QString threeCharKeyHashToString( const QHash<QString,QString> & hash, bool base64Protection )
{
    QString tmp;
    tmp = HASH_PREFIX;
    if (!base64Protection) {
        // Add key + Separator + value + Separator
        foreach( const QString &i, hash.keys() ) {
            tmp += i + separator() + hash.value(i) + separator();
        }
        //        tmp << hash;
        return tmp;
    } else {
        QString val;
        foreach( const QString &i, hash.keys() ) {
            val = hash.value(i);
            tmp += i + separator() + val.toAscii().toBase64() + separator();
        }
    }
    return tmp;
}

/**
  \brief obsolete
  \obsolete
*/
const QHash<QString,QString> threeCharKeyHashToHash( const QString & serialized, bool base64Protection )
{
    QHash<QString,QString> toReturn;
    if (!serialized.startsWith(HASH_PREFIX)) {
        Log::addError( "Serializer", QCoreApplication::translate("Serializer", "Can not deserialize to QHash.") );
        return toReturn;
    }
    QString ser = serialized.mid( QString(HASH_PREFIX).length() );
    QStringList list = ser.split( separator() );
    QStringListIterator it(list);
    QString val;
    QString k;
    if (base64Protection) {
        while (it.hasNext()) {
            val = it.next();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val, QByteArray::fromBase64( k.toAscii() ) );
            }
        }
    } else {
        while (it.hasNext()) {
            val = it.next().toAscii();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val, k );
            }
        }
    }
    return toReturn;
}


}  // End Serializer
}  // End Utils
