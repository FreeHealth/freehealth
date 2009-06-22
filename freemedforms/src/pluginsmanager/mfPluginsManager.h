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
/***************************************************************************
 *   This code is inspired of the Monkey Studio project                    *
 *   http://www.monkeystudio.org/                                          *
 *   Filipe AZEVEDO aka Nox P@sNox <pasnox@gmail.com>                      *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include "mfBaseInterface.h"
#include "mfFormWidgetInterface.h"
#include "mfIOInterface.h"
#include <mfExporter.h>

/**
 * \file mfPluginsManager.h
 * \author Eric MAEKER, MD <eric.maeker@free.fr>
 * \version 0.02
 * \date 14 Janvier 2009
*/

typedef QList<mfFormWidgetInterface*> WidgetPlugins;
typedef QList<mfIOInterface*> IOPlugins;


class Q_FMF_EXPORT mfPluginsManager : public QObject
{
     Q_OBJECT
     friend class mfCore;

public:
     enum StateType { stAll = -1, stDisabled, stEnabled };

     void loadsPlugins();

     QList<mfBaseInterface*> plugins() const { return m_Plugins; }

     template <class T>
     QList<T> plugins( mfPluginsManager::StateType t = stAll,
                       const QString& name = QString::null,
                       const QString& version = QString::null )
     {
          // temporary list
          QList<T> l;
          // for each plugin
          foreach ( mfBaseInterface* bp, m_Plugins )
          // plugin state
          if ( t == stAll || ( !bp->isEnabled() && t == stDisabled ) ||
                    ( bp->isEnabled() && t == stEnabled ) )
               // empty or good name
               if ( name.isEmpty() || bp->infos().Name == name )
                    // no version or good version
                    if ( version.isEmpty() || bp->infos().Version == version )
                         // good cast
                         if ( T p = qobject_cast<T>( bp ) )
                              l << p;
          // return list
          return l;
     }


     template <class T>
     T plugin( mfPluginsManager::StateType type,
               const QString& name,
               const QString& version = QString::null )
     {
          if ( name.isEmpty() )
               return 0;
          return plugins<T>( type, name, version ).value( 0 );
     }

     mfFormWidgetInterface* currentBaseWidget() const
     {
          Q_ASSERT_X( m_BaseWidget, "pluginsManager: no base widget",
                      "there must be a basewidget plugin" );
          if ( !m_BaseWidget )
               qWarning() << qPrintable( tr ( "warning: no base form widget plugin defined" ) );
          return m_BaseWidget;
     }

     void setCurrentIO( mfIOInterface * );
     mfIOInterface* currentIO() const
     {
          Q_ASSERT_X( m_IOPlugin, "pluginsManager: no io plugins",
                      "there must be at least one io plugin" );
          if ( !m_IOPlugin )
               qWarning() << qPrintable( tr ( "warning: no io plugin defined" ) );
          return m_IOPlugin;
     }



protected:
     QList<mfBaseInterface*>  m_Plugins;
     mfIOInterface*           m_IOPlugin;
     mfFormWidgetInterface*   m_BaseWidget;

     mfPluginsManager( QObject* = 0 );
     bool addPlugin( QObject* );
//       void enableUserPlugins();

public slots:
//      void manageRequested();
     void clearPlugins();
};

#endif // PLUGINSMANAGER_H
