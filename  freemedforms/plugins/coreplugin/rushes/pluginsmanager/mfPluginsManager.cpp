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
/**
   \class mfPluginsManager
   \brief Plugins manager for FreeMedForms.
*/

#include "mfPluginsManager.h"

#include <mfCore.h>
#include <mfSettings.h>

#include <tkLog.h>
#include <tkGlobal.h>

#include <QPluginLoader>
#include <QDir>
#include <QFileInfo>

/** \brief Constructor */
mfPluginsManager::mfPluginsManager( QObject* p )
          : QObject( p ), m_IOPlugin( 0 ), m_BaseWidget( 0 )
{
    setObjectName( "mfPluginsManager" );
}

/**
  \brief Load plugins from The plugins path specified by tkSettings::path() tkSettings::FMFPlugInsPath.
  tkSettings must be instanciated BEFORE you call plugins loader.\n
  This member automatically detects the default IO plugins and the BaseWidget plugins.
*/
void mfPluginsManager::loadPlugins()
{
     tkLog::addMessage( this, tr( "Load plugins. Path : %1" ).arg( mfCore::settings()->pluginPath() ) );
     // loads static plugins
     foreach ( QObject* o, QPluginLoader::staticInstances() )
     if ( !addPlugin( o ) )
          tkLog::addError( this, tr( "Failed to load static plugin." ) );

     // load dynamic plugins
     QDir d( mfCore::settings()->pluginPath() );

     // load all plugins
     foreach ( QFileInfo f, tkGlobal::getFiles( d ) ) {
          // don't proced no library file
          if ( !QLibrary::isLibrary( f.absoluteFilePath() ) )
               continue;
          // load plugin
          QPluginLoader l( f.absoluteFilePath() );
          // try unload it and reload it in case of old one in memory
          if ( !l.instance() ) {
               l.unload();
               l.load();
          }
          // continue on no plugin
          if ( !l.instance() ) {
               tkLog::addError( this, tr( "Failed to load plugin ( %1 ): Error: %2" )
                                .arg( l.fileName(), l.errorString() ) );
               continue;
          }
          // try to add plugin to plugins list, else unload it
          else if ( !addPlugin( l.instance() ) )
               l.unload();
     }

     // installs user requested plugins
//      enableUserPlugins();
}

/** \brief Add a functionnal plugins to the plugins manager \internal*/
bool mfPluginsManager::addPlugin( QObject* o )
{
     // try to cast instance to BasePlugin
     mfBaseInterface* bp = qobject_cast<mfBaseInterface*>( o );

     // if not return
     if ( !bp )
          return false;

     // check duplicates
     foreach ( mfBaseInterface* p, m_Plugins ) {
          if ( p->infos().Name == bp->infos().Name ) {
               tkLog::addError( this, tr( "Skipping duplicate plugin: %1, type: %2" )
                                .arg( p->infos().Name )
                                .arg( p->typeToString( p->infos().Type ) ) );
               return false;
          }
     }

     // show plugin infos
     tkLog::addMessage( this, tr( "Adding %1, type: %2" )
                        .arg( bp->infos().Name )
                        .arg( bp->typeToString( bp->infos().Type ) ) );

     // check basewidget
     if ( !m_BaseWidget )
          if ( bp->infos().Name.contains( "baseformwidget", Qt::CaseInsensitive ) ) {
               tkLog::addMessage( this, tr( "Setting Base Widget: %1" )
                                  .arg( bp->infos().Name ) );
               m_BaseWidget = qobject_cast<mfFormWidgetInterface*>( bp );
          }

     // set current io plugins
     if ( bp->infos().Type == mfBaseInterface::iIO )
          setCurrentIO( qobject_cast<mfIOInterface*>( bp ) );

     // add it to plugins list
     m_Plugins << bp;

     // return
     return true;
}

/*
void mfPluginsManager::enableUserPlugins()
{

     foreach ( mfBaseInterface* bp, m_Plugins )
     {
          // check in settings if we must install this plugin
          if ( !MonkeyCore::settings()->value( QString( "Plugins/%1" ).arg( bp->infos().Name ), true ).toBool() )
               qWarning( qPrintable( tr( "User wantn't to intall plugin: %1" ).arg( bp->infos().Name ) ) );
          // if not enabled, enable it
          else if ( !bp->isEnabled() )
          {
               if ( bp->setEnabled( true ) )
                    qWarning( qPrintable( tr( "Successfully enabled plugin: %1" ).arg( bp->infos().Name ) ) );
               else
                    qWarning( qPrintable( tr( "Unsuccessfully enabled plugin: %1" ).arg( bp->infos().Name ) ) );
          }
          else
               qWarning( qPrintable( tr( "Already enabled plugin: %1" ).arg( bp->infos().Name ) ) );
     }
}
*/

/** \brief Defines the current IO plugins for forms IO */
void mfPluginsManager::setCurrentIO( mfIOInterface * b )
{
     // if same cancel
     if ( m_IOPlugin == b )
          return;

     // disabled all plugins of the same family
     foreach ( mfIOInterface * bp, plugins<mfIOInterface*>( mfPluginsManager::stAll ) )
     bp->setEnabled( false );

     // enabled the one we choose
     m_IOPlugin = b;
     if ( m_IOPlugin )
          m_IOPlugin->setEnabled( true );

     tkLog::addMessage( this, tr( "Setting default IO: %1" ).arg( b->infos().Name ) );
}


// void mfPluginsManager::manageRequested()
// { ( new UIPluginsSettings() )->show(); }

/** \brief Cleans everything */
void mfPluginsManager::clearPlugins()
{
     foreach ( mfBaseInterface* bp, m_Plugins ) {
          tkLog::addMessage( this, tr( "Clearing %1..." ).arg( bp->infos().Name ) );
          bp->setEnabled( false );
     }
     qDeleteAll( m_Plugins );
     m_Plugins.clear();
}
