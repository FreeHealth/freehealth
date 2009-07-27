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
#include <QtPlugin>
#include <QApplication>

#include <mfMainWindow.h>
#include <mfCore.h>
#include <mfPluginsManager.h>

#include <tkLog.h>

int main( int argc, char *argv[] )
{
     Q_INIT_RESOURCE( application );
     QT_REQUIRE_VERSION( argc, argv, "4.5.0" );

     QApplication app( argc, argv );

     QTextCodec::setCodecForTr( QTextCodec::codecForName( "UTF-8" ) );
     QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );

#ifdef DEBUG
    app.setApplicationName( QString( "%1 - %2 debug" ).arg( PACKAGE_NAME, PACKAGE_VERSION ) );
#else
    app.setApplicationName( QString( "%1 - %2" ).arg( PACKAGE_NAME, PACKAGE_VERSION ) );
#endif

     app.setOrganizationName( PACKAGE_NAME );
     app.setOrganizationDomain( PACKAGE_DOMAIN );
     app.setApplicationVersion( PACKAGE_VERSION );

     QObject::connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

     // init core
     if (!mfCore::init()) {
         tkLog::saveLog();
         return 1;
     }

     // execute application
     int result = app.exec();

     // some cleanup
     mfCore::endOfApplication();

     // return app exit code
     tkLog::saveLog();
     return result;
}

