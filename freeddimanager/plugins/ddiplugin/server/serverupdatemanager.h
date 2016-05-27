/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : EricMaeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_SERVERUPDATEMANAGER_H
#define DDIMANAGER_DDIPLUGIN_SERVERUPDATEMANAGER_H

#include <ddiplugin/server/serverupdate.h>
#include <QObject>

/**
 * \file serverupdatemanager.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 01 Nov 2013
*/

namespace DDI {
namespace Internal {
class ServerUpdateManagerPrivate;

class ServerUpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerUpdateManager(QObject *parent = 0);
    ~ServerUpdateManager();
    bool initialize();

    void prepareUpdateToSend(ServerUpdate::UpdateType type, ServerUpdate::UpdateSubType subType);
    
    int numberDatabaseOfUpdates(ServerUpdate::UpdateType type, ServerUpdate::UpdateSubType subType) const;

Q_SIGNALS:
    void updateSent(ServerUpdate::UpdateType type, ServerUpdate::UpdateSubType sub, bool error);
    void allUpdateSent();

public Q_SLOTS:
    void sendUpdates();

private:
    Internal::ServerUpdateManagerPrivate *d;
};

} // namespace Internal
} // namespace DDI

#endif  // DDIMANAGER_DDIPLUGIN_SERVERUPDATEMANAGER_H

