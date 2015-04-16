/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2015 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
#include "mfRecovererThread.h"

// include freemedforms headers
#include <mfObject.h>
#include <mfCore.h>
#include <mfMainWindow.h>

// include toolkit headers
#include <tkTheme.h>

// include Qt headers
#include <QLabel>

Q_TK_USING_CONSTANTS

void mfRecovererThread::saveRecoveringFile( mfObject * mfo )
{
     if ( !mfo ) return;

     // Only one recovery at time
     if ( isRunning() ) return;
     stopped = false;
     m_Object = mfo;

     // put an icon into statusbar
     if ( !iconSetted )
     {
          iconSetted = true;
          QLabel * lbl = new QLabel ( mfCore::mainWindow() );
          lbl->setObjectName( "crashicon" );
          lbl->setPixmap( tkTheme::icon(ICONCRASHRECOVER).pixmap(16,16) );
          lbl->setToolTip( QCoreApplication::translate( "mfRecovererThread",
                           "Crash recovering is in action.\n"
                           "Recover your datas after a system or software crash." ) );
          mfCore::mainWindow()->statusBar()->addPermanentWidget( lbl, 0 );
     }

     // Let's go with low priority
     start( LowPriority );
}

void mfRecovererThread::stop()
{
     stopped = true;
}


void mfRecovererThread::run()
{
     saveRecoveryFile( m_Object );
}

QString mfRecovererThread::addObjectRecoverer( const QString & fullParents, mfObject * mfo )
{
     QString objName = mfo->name();
// qWarning() << objName << fullParents;
     QString toReturn = "";
     // Add mfo value

     // If it is the root form object add underscore before its name
     if ( fullParents.isEmpty() )
          objName.prepend( "_" );
     else
          objName.prepend( fullParents );

     if ( !mfo->value().toString().isEmpty() && ( mfo->type() != mfObject::Type_Form ) )
     {
          toReturn = objName;
          if ( mfo->isMultiSelection() )
          {
               toReturn.append( ".ids= new Array(" );
               foreach( int i, mfo->ids() )
               toReturn.append( QString::number( i ) + ", " );
               toReturn.chop( 2 );
               toReturn.append( ");\n" );
               qWarning() << mfo->ids() << toReturn;
          }
          else
          {
               // Single selection objects
               switch ( mfo->type() )
               {
                    case mfObject::Type_ShortText:
                    case mfObject::Type_LongText:
                         toReturn.append( ".value=\"" + mfo->value().toString().replace( "\n", "\\n" ) + "\";\n" );
                         break;
                    case mfObject::Type_Date:
                    {
                         QDateTime dt = mfo->value().toDateTime();
                         toReturn.append( QString( ".value=new Date(%1);\n" )
                                          .arg( dt.toString( "yyyy,MM,dd,HH,mm,ss" ) ) );
                         break;
                    }
                    case mfObject::Type_Button:
                         break;
                    default:
                         toReturn.append( ".id=" + QString::number( mfo->id() ) + ";\n" );
                         break;
               }
          }
     }
     objName.append( "." );
     foreach( mfObject * chd, mfo->mfChildren() )
     {  toReturn.append( addObjectRecoverer( objName , chd ) ); }
     return toReturn;
}

bool mfRecovererThread::saveRecoveryFile( mfObject * mfo )
{
     // Structure of the file :
     // First line = comment
     // Second line = absoluteFileName of the opened form
     // All next lines are script to retrieve values, addFiles...
     if ( mfCore::temporaryFile()->open() )
     {
          // Previous version using QTextStream does not work always got an empty string
          mfCore::temporaryFile()->write( "This temporary recovery file is part of FreeMedForms software.\n" );
          // Add the root form file
          mfCore::temporaryFile()->write( "file:" + mfo->absoluteFileName().toAscii() + "\n" );
          // Add the mfObjects to TextStream
          mfCore::temporaryFile()->write( addObjectRecoverer( QString::null , mfo ).toAscii() );
          mfCore::temporaryFile()->close();
          return true;
     }
     return false;
}
