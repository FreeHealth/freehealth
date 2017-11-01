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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/**
  \namespace SaveRestore
  \brief Namespace reserved for the Save and Restore plugin.
*/

#include "saverestoreplugin.h"
#include "saverestorepage.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace SaveRestore;
using namespace Internal;

SaveRestorePlugin::SaveRestorePlugin() :
    page(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating SaveRestorePlugin";
}

SaveRestorePlugin::~SaveRestorePlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool SaveRestorePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "SaveRestorePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_saverestore");

    return true;
}

void SaveRestorePlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "SaveRestorePlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    page = new SaveRestorePage(this);
    addObject(page);
}

ExtensionSystem::IPlugin::ShutdownFlag SaveRestorePlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool

    if (page) {
        removeObject(page);
        delete page;
        page = 0;
    }

    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(SaveRestorePlugin)
