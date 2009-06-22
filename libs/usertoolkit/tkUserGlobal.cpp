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
#include "tkUserGlobal.h"

// include toolkit headers
#include <tkTranslators.h>

#include <tkUserBase.h>
#include <tkUserModel.h>
#include <tkSettings.h>
#include <tkLog.h>

// include Qt headers
#include <QCryptographicHash>
#include <QByteArray>

/**
  \brief Some global functions for user toolkit librairy.
  \ingroup usertoolkit object_usertoolkit
  \ingroup usermanager
*/
Q_TKUSER_BEGIN_GLOBAL

const void initLib()
{
    tkLog::addMessage( "tkUserGlobal", "Initializing tkUser lib" );
    // load translator
    if ( tkTranslators::instance() )
        tkTranslators::instance()->addNewTranslator( "usertoolkit" );
    // initialize database
    Q_ASSERT_X( tkSettings::instance() , "tkSettings", "Before calling tkUserGlobal::init() YOU MUST define an instance of tkSettings" );
//    tkUserBase::instance()->initialize( tkSettings::instance() );
    // initialize model
    tkUserModel::instance();
    tkLog::addMessage( "tkUserGlobal", "tkUser lib is initialized" );
}

const QString getLibraryInformations()
{
    return QString( "user toolkit library, version : %1 %2 , compiled on : %3 %4" )
            .arg( PACKAGE_VERSION )
#ifdef DEBUG
            .arg( "debug" )
#else
            .arg( "release" )
#endif
            .arg( __DATE__ )
            .arg( __TIME__ );
}


/** \brief First crypt string using SHA1 logarythm then transform crypted result to base64 (so it can be
           added into database without problem - no special characters).
*/
const QString crypt( const QString & toCrypt )
{
    QCryptographicHash crypter( QCryptographicHash::Sha1 );
    crypter.addData( toCrypt.toAscii() );
    return crypter.result().toBase64();
}

const QString loginForSQL ( const QString & log )
{ return log.toAscii().toBase64(); }

const QString loginFromSQL( const QVariant & sql )
{ return QByteArray::fromBase64( sql.toByteArray() ); }

const QString loginFromSQL( const QString & sql )
{ return QByteArray::fromBase64( sql.toAscii() ); }

} // end NAMESPACE TKGLOBAL
