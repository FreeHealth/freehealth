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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef ICORELISTENER_H
#define ICORELISTENER_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
class QSplashScreen;
class QPixmap;
class QWidget;
class QString;

/**
 * \file icorelistener.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.3.0
 * \date 28 Jan 2010
*/

/**
  \class Core::ICoreListener

  \brief Provides a hook for plugins to veto on certain events emitted from
the core plugin.

  You implement this interface if you want to prevent certain events from
  occurring, e.g.  if you want to prevent the closing of the whole application
  or to prevent the closing of an editor window under certain conditions.

  If e.g. the application window requests a close, then first
  ICoreListener::coreAboutToClose() is called (in arbitrary order) on all
  registered objects implementing this interface. If one if these calls returns
  false, the process is aborted and the event is ignored.  If all calls return
  true, the corresponding signal is emitted and the event is accepted/performed.

  Guidelines for implementing:
  \list
  \o Return false from the implemented method if you want to prevent the event.
  \o You need to add your implementing object to the plugin managers objects:
     ExtensionSystem::PluginManager::instance()->addObject(yourImplementingObject);
  \o Don't forget to remove the object again at deconstruction (e.g. in the destructor of
     your plugin).
*/

namespace Core {

class CORE_EXPORT ICoreListener : public QObject
{
    Q_OBJECT
public:
    ICoreListener(QObject *parent) : QObject(parent) {}
    virtual ~ICoreListener() {}

    virtual bool coreAboutToClose() {return true;}
};

} // namespace Core

#endif // ICORELISTENER_H
