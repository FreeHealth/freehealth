/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mfSettings.h"

#include <mfGlobal.h>
#include <mfCore.h>
#include <mfMainWindow.h>
#include <mfConstants.h>

#include <tkLog.h>

#include <QApplication>
#include <QMainWindow>

mfSettings::mfSettings( QObject * parent )
    : tkSettings( parent )
{
    setObjectName( "mfSettings" );
//    tkSettings::setPath( tkSettings::WebSiteUrl, "http://code.google.com/p/freemedforms" );

    // init all paths
    QString tempFileTemplate = "";
    QString debugPath = "";
    // every resources should be in the same path as the ini file
    m_TempFileTemplate = path( ApplicationTempPath ) + "fmf_recovery_XXXXXX.tmp";

    // some resources should be saved into config.ini
    m_UserFormPath     = value( "MainApplication/FormsPath", path( SampleFormsPath ) ).toString();
    if ( m_UserFormPath != path( SampleFormsPath ) )
        if ( ! QFileInfo( m_UserFormPath ).exists() )
            m_UserFormPath = path( SampleFormsPath );
    setValue( "MainApplication/FormsPath", m_UserFormPath );
}


bool mfSettings::needUserIdentification()
{
    if (!value(mfConstants::SETTING_LASTLOGIN).toString().isEmpty()) // &&
        //         ( ! value( "Login/userPassword" ).toString().isEmpty() ) )
        return false;
    return true;
}

bool mfSettings::readSettings()
{
    // retreive all values
    // read QMainWindow state
    tkSettings::restoreState( mfCore::mainWindow() );
    return true;
}

bool mfSettings::writeSettings()
{
    // save all dynamic properties of qApp (used by drugswidget)
    QList<QByteArray> prop = qApp->dynamicPropertyNames();
    qSort( prop );
    foreach( QByteArray s, prop )
        setValue( s, qApp->property( s ) );
    // save some specific values
    setValue( "MainApplication/FormsPath", m_UserFormPath );
    // save QMainWindow state
    tkSettings::saveState( mfCore::mainWindow() );
    return true;
}

void mfSettings::setDefaultSettings()
{
}
