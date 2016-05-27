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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "zipcodesplugin.h"
#include "zipcore.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtPlugin>
#include <QDebug>

using namespace ZipCodes;
using namespace Internal;

ZipCodesPlugin::ZipCodesPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating ZipCodesPlugin";

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool
    _core = new ZipCore(this);
}

ZipCodesPlugin::~ZipCodesPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool ZipCodesPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "ZipCodesPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // No user connected here

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_zipcodes");

    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool
    _core->initialize();

    return true;
}

void ZipCodesPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "ZipCodesPlugin::extensionsInitialized";

    // At this point, user is connected

    // All preferences pages must be created in this part (after user connection)

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

ExtensionSystem::IPlugin::ShutdownFlag ZipCodesPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(ZipCodesPlugin)
