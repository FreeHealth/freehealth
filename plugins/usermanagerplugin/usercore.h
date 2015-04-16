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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERPLUGIN_USERCORE_H
#define USERPLUGIN_USERCORE_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <QObject>

/**
 * \file usercore.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 11 Feb 2013
*/

namespace UserPlugin {
class UserModel;
class UserCreationPage;

namespace Internal {
class UserCorePrivate;
class UserManagerPlugin;
class UserBase;
} // namespace Internal

class USER_EXPORT UserCore : public QObject
{
    Q_OBJECT
    friend class UserPlugin::Internal::UserManagerPlugin;
    friend class UserPlugin::UserCreationPage;

protected:
    explicit UserCore(QObject *parent = 0);
    bool initialize();
    bool forceReInitialization();
    bool postCoreInitialization();

public:
    static UserCore &instance();
    ~UserCore();

    bool isInitialized() const;
    Internal::UserBase *userBase() const;
    UserModel *userModel() const;

Q_SIGNALS:

public Q_SLOTS:

private:
    Internal::UserCorePrivate *d;
    static UserCore *_instance;
};

} // namespace UserPlugin

#endif  // USERPLUGIN_USERCORE_H

