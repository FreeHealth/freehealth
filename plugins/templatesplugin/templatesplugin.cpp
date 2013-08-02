/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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

/**
  \namespace Templates
  \brief Namespace reserved for the Templates plugin.
*/

#include "templatesplugin.h"
#include "templatescore.h"
#include "templatesmodel.h"
#include "templatesview.h"
#include "templatesview_p.h"
#include "templatespreferencespages.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Templates;
using namespace Internal;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

TemplatesPlugin::TemplatesPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating TemplatesPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_templates");

    // Create the instance of Core
    new Templates::TemplatesCore(this);

    prefPage = new Internal::TemplatesPreferencesPage(this);
    addObject(prefPage);
}

TemplatesPlugin::~TemplatesPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool TemplatesPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "TemplatesPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void TemplatesPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "TemplatesPlugin::extensionsInitialized";

    // Initialize templateCore
    Templates::TemplatesCore::instance().init();

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    prefPage->checkSettingsValidity();
}

ExtensionSystem::IPlugin::ShutdownFlag TemplatesPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (prefPage) {
        removeObject(prefPage);
        delete prefPage; prefPage=0;
    }
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(TemplatesPlugin)
