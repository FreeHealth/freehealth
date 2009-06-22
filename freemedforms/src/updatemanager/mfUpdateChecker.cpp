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
#include <mfUpdateChecker.h>

// include freemedforms headers
#include <mfCore.h>
#include <tkLog.h>
#include <mfSettings.h>
#include <mfMainWindow.h>

// include toolkit headers
#include <tkTheme.h>

// include Qt headers
#include <QtCore>
#include <QMessageBox>
#include <QLabel>

Q_TK_USING_CONSTANTS

const QUrl mfUpdateChecker::url = QUrl( QUrl( "http://www.ericmaeker.fr/FreeMedForms/dld/version.txt" ) );

mfUpdateChecker::mfUpdateChecker( QObject *parent )
          : QObject( parent )
{
    setObjectName( "mfUpdateChecker" );
     connect( &http, SIGNAL( done( bool ) ), this, SLOT( httpDone( bool ) ) );
}

mfUpdateChecker::~mfUpdateChecker()
{
}

void mfUpdateChecker::check()
{
     tkLog::addMessage( this, tr( "Check for Updates." ) );
     getFile( url );
}

bool mfUpdateChecker::getFile( const QUrl &url )
{
     if ( !url.isValid() )
          return false;
     if ( url.scheme() != "http" )
          return false;
     if ( url.path().isEmpty() )
          return false;

     http.setHost( url.host(), url.port( 80 ) );
     http.get( url.path(), &buffer );
     http.close();
     return true;
}

void mfUpdateChecker::httpDone( bool error )
{
    if ( error )
    {
        tkLog::addError( this, tr( "Error. httpDone = false." ) );
        return;
    }

    QString update = buffer.data();
    QString forLog = "";
    QString msg = "";

    // Get last version number from update file
    int idLast = 0;
    while ( true )
    {
        idLast = update.indexOf( "version=", idLast );
        if ( idLast == -1 )
            break;

        idLast += 8;
        QString lastVersion = update.mid( idLast, 4 );

        // If necessary keep changelog into update and emit updateFound with the changelog
        if ( qApp->applicationVersion() < lastVersion )
        {
            int begin = update.indexOf( "{", idLast ) + 1;
            int end = update.indexOf( "}", begin );
            msg.append( tr( "* Updates of version : %1" ).arg( lastVersion ) );
            msg.append( update.mid( begin, end - begin ) + "\n" );
            forLog += lastVersion + "; ";
        }
    }

    if ( msg.isEmpty() )
    {
        tkLog::addMessage( this, tr( "Version up to date." ) );
        return;
    }

    forLog.chop( 2 );
    tkLog::addMessage( this, tr( "Update founded : %1" ).arg( forLog ));
    msg = msg.replace( "\t", "  " );
    updateFound( msg );
}

void mfUpdateChecker::cancelDownload()
{
     http.abort();
}

void mfUpdateChecker::updateFound( const QString & up )
{
    QMessageBox msgBox;
    msgBox.setWindowTitle( qApp->applicationName() );
    msgBox.setText( tr( "<b>A new version of this application is available on the Internet.</b><br><br>"
                        "<a href=\"%1\">Please check the web site the application.</a>" )
                    .arg( mfCore::settings()->webSiteUrl() ) );
    msgBox.setDetailedText( up );
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setDefaultButton( QMessageBox::Ok );
    msgBox.exec();

     QLabel * lbl = new QLabel ();
     lbl->setPixmap( tkTheme::icon( ICONSOFTWAREUPDATEAVAILABLE ).pixmap(16,16) );
     lbl->setToolTip( tr( "An update is available.<br />Please, check the web site." ) );
     mfCore::mainWindow()->statusBar()->addPermanentWidget( lbl, 0 );
}

