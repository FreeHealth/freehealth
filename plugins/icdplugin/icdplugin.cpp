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
/**
  \namespace ICD
  \brief Namespace reserved for the ICD plugin.
  The ICD namespace includes:
  - All database accesses to the ICD10 database
  - A class used to build the ICD10 database from the raw sources
  - All models related to ICD10
  - Some UIs like code search, code selector, code association manager
*/

#include "icdplugin.h"
#include "icdwidgetmanager.h"
#include "icddatabase.h"
#include "icddownloader.h"

#ifdef FREEMEDFORMS
#    include "icdwidgetfactory.h"
#endif

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>

#include <QtPlugin>
#include <QDebug>

using namespace ICD::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

IcdPlugin::IcdPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating IcdPlugin";

    // Create the database instance
    IcdDatabase::instance();
}

IcdPlugin::~IcdPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool IcdPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "IcdPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_icd");
    messageSplash(tr("Initializing ICD10 plugin..."));

    return true;
}

void IcdPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "IcdPlugin::extensionsInitialized";

    messageSplash(tr("Initializing ICD10 plugin..."));

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

#ifdef FREEMEDFORMS
    addAutoReleasedObject(new IcdWidgetFactory(this));
#endif

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void IcdPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;

    IcdDatabase::instance()->initialize();

    // Create the widgetmanager instance (here because we need that MainWindow has done its initialization)
#ifndef FREETOOLBOX
    IcdWidgetManager::instance();
#endif
}

ExtensionSystem::IPlugin::ShutdownFlag IcdPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(IcdPlugin)
