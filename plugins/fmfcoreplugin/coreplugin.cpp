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
#include "coreplugin.h"
#include "coreimpl.h"
#include "sqlitedatabasepathpreferences.h"

#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <extensionsystem/pluginmanager.h>

#include <fmfcoreplugin/appaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/dialogs/commonaboutpages.h>
#include <coreplugin/dialogs/commondebugpages.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/applicationgeneralpreferences.h>
#include <coreplugin/dialogs/networkpreferences.h>

#include <QtPlugin>

using namespace Core;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

CorePlugin::CorePlugin() :
    m_CoreImpl(0),
    prefPage(0),
    proxyPage(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating CorePlugin";
    m_CoreImpl =  new CoreImpl(this);

    prefPage = new ApplicationGeneralPreferencesPage(this);
    addObject(prefPage);
    proxyPage = new ProxyPreferencesPage(this);
    addObject(proxyPage);
}

CorePlugin::~CorePlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "CorePlugin::initialize";

    messageSplash(tr("Initializing core plugin..."));

    return m_CoreImpl->initialize(arguments,errorMessage);
//    Q_UNUSED(arguments);
//    const bool success = m_mainWindow->init(errorMessage);
//    if (success) {
//        m_welcomeMode = new WelcomeMode;
//        addObject(m_welcomeMode);
//
//        EditorManager *editorManager = m_mainWindow->editorManager();
//        m_editMode = new EditMode(editorManager);
//        addObject(m_editMode);
//    }
//    return success;
}

void CorePlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "CorePlugin::extensionsInitialized";

    messageSplash(tr("Initializing core plugin..."));

    // add about pages
    this->addAutoReleasedObject(new AppAboutPage(this));
    this->addAutoReleasedObject(new TeamAboutPage(this));
    this->addAutoReleasedObject(new LicenseAboutPage(this));
    this->addAutoReleasedObject(new BuildAboutPage(this));

    // add debugging pages
    this->addAutoReleasedObject(new LogErrorDebugPage(this));
    this->addAutoReleasedObject(new LogMessageDebugPage(this));
    this->addAutoReleasedObject(new SettingDebugPage(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // add preferences page
    prefPage->checkSettingsValidity();
    proxyPage->checkSettingsValidity();
    m_CoreImpl->settings()->sync();

    m_CoreImpl->extensionsInitialized();

    // Add preference page
    if (settings()->databaseConnector().driver() == Utils::Database::SQLite)
        this->addAutoReleasedObject(new SqliteDatabasePathPage(this));
}

void CorePlugin::remoteArgument(const QString& arg)
{
    Q_UNUSED(arg);
    // An empty argument is sent to trigger activation
    // of the window via QtSingleApplication. It should be
    // the last of a sequence.
//    if (arg.isEmpty()) {
//        m_mainWindow->activateWindow();
//    } else {
//        m_mainWindow->openFiles(QStringList(arg));
//    }
}

ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (prefPage) {
        removeObject(prefPage);
        delete prefPage; prefPage=0;
    }
    if (proxyPage) {
        removeObject(proxyPage);
        delete proxyPage; proxyPage=0;
    }
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(CorePlugin)
