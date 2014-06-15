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
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "account2plugin.h"
#include "accountcore.h"

#ifdef FREEMEDFORMS
#    include "accountmode.h"
#endif

#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <QtPlugin>
#include <QDebug>

using namespace Account2;
using namespace Internal;

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

Account2Plugin::Account2Plugin() :
    ExtensionSystem::IPlugin(),
    _core(0)
{
    setObjectName("Account2Plugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating Account2Plugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_accountbase");

    // Create the database instance
    _core = new AccountCore(this);
}

Account2Plugin::~Account2Plugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Account2Plugin::~Account2Plugin()";
}

bool Account2Plugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Account2Plugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void Account2Plugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Account2Plugin::extensionsInitialized";

    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    messageSplash(tr("Initializing accountancy plugin..."));

    // FreeMedForms specific code
#ifdef FREEMEDFORMS
    addAutoReleasedObject(new Account2::Internal::AccountMode(this));
#endif

    // Initialize Account Core
    if (!_core->initialize())
        LOG_ERROR("Unable to initialize account core");

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

ExtensionSystem::IPlugin::ShutdownFlag Account2Plugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(Account2Plugin)
