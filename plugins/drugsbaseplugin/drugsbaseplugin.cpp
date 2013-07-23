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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \namespace DrugsDB
  \brief Namespace reserved for the DrugsDB plugin.
  The DrugsDB namespace includes:
  - All database accesses
  - All constants related to drugs
  - Interaction engines
  - Save/Load abilities (XML prescription files)
  - Print abilities
*/

#include "drugsbaseplugin.h"
#include "drugstemplateprinter.h"
#include "drugbasecore.h"
#include "drugsbase.h"
#include "drugsdatabaseselector.h"

#include <utils/log.h>
#include <utils/global.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>

using namespace DrugsDB::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

DrugsBasePlugin::DrugsBasePlugin() :
    IPlugin()
{
    if (Utils::Log::warnPluginsCreation()) {
#ifdef FREEDIAMS
        qWarning() << "creating FREEDIAMS::DrugsBasePlugin";
#else
        qWarning() << "creating FREEMEDFORMS::DrugsBasePlugin";
#endif
    }

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_drugsbase");

    // Create the drugs core instance. Core auto-init itself after the coreOpened()
    new DrugsDB::DrugBaseCore(this);
}

DrugsBasePlugin::~DrugsBasePlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool DrugsBasePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // This code is executed AFTER the UserManagerPlugin::initialize()
    messageSplash(tr("Initializing drugs database plugin..."));
    DrugsDB::DrugBaseCore::instance().initialize();

    return true;
}

void DrugsBasePlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::extensionsInitialized";

    messageSplash(tr("Initializing drugs database plugin..."));

    // Check drugs database version
    if (!drugsBase().checkDatabaseVersion()) {
        Utils::warningMessageBox(tr("Wrong drugs database installed"),
                                 tr("A wrong drugs datbase is installed on your computer. This can "
                                    "be the result of an application updating. You have to open to the "
                                    "datapack manager and update or remove the installed drugs database. \n"
                                    "Please find more documentation on the website %1.")
                                 .arg(settings()->path(Core::ISettings::WebSiteUrl)));
    }

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    addAutoReleasedObject(new DrugsDB::Internal::DrugsTemplatePrinter(this));
}

ExtensionSystem::IPlugin::ShutdownFlag DrugsBasePlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(DrugsBasePlugin)
