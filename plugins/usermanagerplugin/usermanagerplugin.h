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
#ifndef USERPLUGIN_INTERNAL_USERMANAGERPLUGIN_H
#define USERPLUGIN_INTERNAL_USERMANAGERPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>
#include <QPointer>
class QAction;

/**
 * \file usermanagerplugin.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 27 Aug 2013
*/

namespace UserPlugin {
class UserManagerDialog;
class FirstRun_UserConnection;
class FirstRun_UserCreation;
class UserManagerMainWindow;
namespace Internal {
class UserManagerMode;

class UserManagerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.UserManagerPlugin" FILE "UserManager.json")

public:
    UserManagerPlugin();
    ~UserManagerPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:
    bool identifyUser();

private Q_SLOTS:
    void postCoreInitialization();
    void createUser();
    void changeCurrentUser();
    void updateActions();
    void showUserManager();
    void showDatabaseInformation();

#ifdef WITH_TESTS
    void initTestCase();
    void test_userdynamicdata_basics();
    void test_userdata_basics();
    void test_usercore_initialization();
    void test_userbase_test_with_virtualuser();
    void test_userbase_basics();
    void test_usermodel_basics();
    void cleanupTestCase();
#endif

private:
    QAction *aCreateUser;
    QAction *aChangeUser;
    QAction *aUserManager;
    QAction *aAboutDatabase;

    FirstRun_UserCreation *m_FirstCreation;
    Internal::UserManagerMode *m_Mode;
    UserManagerMainWindow *m_UserManagerMainWin;
};

} // namespace Internal
} // namespace UserPlugin

#endif  // USERPLUGIN_INTERNAL_USERMANAGERPLUGIN_H
