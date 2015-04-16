/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IUSERLISTENER_H
#define IUSERLISTENER_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <QObject>

namespace UserPlugin {

/*!
  \class Core::IUserListener

  \brief Provides a hook for plugins to veto on certain events emitted from the
  UserManager plugin.

  You implement this interface if you want to prevent certain events from
  occurring, e.g.  if you want to prevent user disconnection.

  If e.g. the UserModel requests a user disconnection then a new user
  connection, then first IUserListener::userAboutToChange() is called (in
  arbitrary order) on all registered objects implementing this interface.
  If one of these calls returns false, the process is aborted. If all calls
  return true, the corresponding signal is emitted and the event is
  accepted/performed.

  Guidelines for implementing:
  \list
  \o Return false from the implemented method if you want to prevent the event.
  \o You need to add your implementing object to the plugin managers objects:
     ExtensionSystem::PluginManager::instance()->addObject(yourImplementingObject);
  \o Don't forget to remove the object again at deconstruction (e.g. in the
     destructor of your plugin).
*/
class USER_EXPORT IUserListener : public QObject
{
    Q_OBJECT
protected:
    IUserListener(QObject *parent = 0) : QObject(parent) {}
public:
    virtual ~IUserListener() {}

    virtual bool userAboutToChange() { return true; }
    virtual bool currentUserAboutToDisconnect() { return true; }
    virtual void newUserConnected(const QString &uuid) {Q_UNUSED(uuid);}
};

} // end namespace UserPlugin

#endif // IUSERLISTENER_H
