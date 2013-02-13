/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
 * \author Eric Maeker
 * \version 0.8.2
 * \date 08 Feb 2013
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
    virtual QString errorMessage() const {return QString::null;}
};

} // namespace Core

#endif // ICORELISTENER_H
