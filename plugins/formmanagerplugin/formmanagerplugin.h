/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef FORM_MANAGER_IPLUGIN_H
#define FORM_MANAGER_IPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file formmanagerplugin.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 01 Aug 2014
*/

namespace Form {
class FormCore;
namespace Internal {
class FormManagerMode;
class FirstRunFormManagerConfigPage;
class FormPreferencesPage;
class FormPreferencesFileSelectorPage;

class FormManagerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.FormManagerPlugin" FILE "FormManager.json")

public:
    FormManagerPlugin();
    ~FormManagerPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();

#ifdef WITH_TESTS
    void initTestCase();
    void test_formcore_initialization();
    void test_FormManager_initialization();
    void test_FormIOQuery();
    void test_FormIO_queryFromDatabase();
    void test_FormIO_queryFromLocal();
    void test_FormIO_screenshots();
    void test_FormIO_userForms();
    void cleanupTestCase();
#endif

private:
    FormCore *_core;
    FormManagerMode *_mode;
    Internal::FirstRunFormManagerConfigPage *m_FirstRun;
    Internal::FormPreferencesFileSelectorPage *m_PrefPageSelector;
    Internal::FormPreferencesPage *m_PrefPage;
};

}
}

#endif  // End FORM_MANAGER_IPLUGIN_H
