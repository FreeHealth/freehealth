/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2014 by Eric MAEKER, MD                                     **
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
#include <QMessageBox>
#include <QDebug>
#include <QScriptEngine>
#include <QObject>

#include <mfScriptor.h>
#include <tkLog.h>
#include <mfObject.h>


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
mfScriptor::mfScriptor( QObject * parent )
          : QObject( parent ), m_Object( 0 ), m_RootObject( 0 ), m_ErrorMsg( "" ),
          m_Engine( 0 )
{
    setObjectName( "mfScriptor" );
     m_Engine = new QScriptEngine( this );
     if ( !m_Engine )
     {
          m_ErrorMsg = tr( "Script Engine : Unable to build the script engine." );
          tkLog::addError( this, tr( "Unable to build the script engine." ) );
          return;
     }
     else
         tkLog::addMessage( this, tr( "Initialized." ) );
}


mfScriptor::~mfScriptor()
{
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Initialisators of scriptor -------------------------------------
//--------------------------------------------------------------------------------------------------------
bool mfScriptor::setObject( mfObject * obj )
{
     if ( !obj )
     {
          m_ErrorMsg = tr( "Script Engine : The object passed to the script engine does not exists." );
          tkLog::addError( this, tr( "The object passed to the script engine does not exists." ) );
          return false;
     }

     m_Object = obj;
     // Make rootObject accessible inside the script engine
     QScriptValue object = m_Engine->newQObject( m_Object );

     m_Engine->globalObject().setProperty( "_" + m_Object->name(), object );

     // Retrieve root object --> needs to be setted each time because if reload a form: mfObject is deleted.
     m_RootObject = m_Object->root();
     QScriptValue thisForm = m_Engine->newQObject( m_RootObject );
     m_Engine->globalObject().setProperty( "thisForm", thisForm );

     return true;
}


QString mfScriptor::run( int param )
{
     Q_ASSERT_X( ( param == mfObject::Param_Script ) || ( param == mfObject::Param_OnLoadScript ),
                 "Passing wrong param to scriptor::run",
                 "You can only ask a Param_Script or a Param_OnLoadScript" );

     if ( ! ( ( param == mfObject::Param_Script ) || ( param == mfObject::Param_OnLoadScript ) ) )
          return QString::null;

// qWarning() << m_Object->param( param ).toString() << param;

     QString str = m_Object->param( param ).toString();
     QScriptValue test = m_Engine->evaluate( str );

     if ( m_Engine->hasUncaughtException() )
     {
          QStringList lines = str.split( "\n" );
          QString msg = tr( "Script error %1 at line %2 : %3" )
                 .arg( test.toString() )
                 .arg( m_Engine->uncaughtExceptionLineNumber() )
                 .arg( lines[ m_Engine->uncaughtExceptionLineNumber() - 1 ].trimmed() ) ;
          tkLog::addMessage( this, msg );
          return msg;
     }

     return test.toString();
}

