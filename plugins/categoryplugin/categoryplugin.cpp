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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \namespace Category
  \brief Contains category manager for all applications of the FreeMedForms project.
  The Category plugin owns:
  - interface classes for implementation of category in any plugin
  - its database
  - some models for your views
  - dialogs for edition/creation of categories
*/

#include "categoryplugin.h"
#include "categorycore.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Category;

CategoryPlugin::CategoryPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating CategoryPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("categoryplugin");

    // Create the core instance
    CategoryCore::instance(this);
}

CategoryPlugin::~CategoryPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool CategoryPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CategoryPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void CategoryPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CategoryPlugin::extensionsInitialized";
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    if (!CategoryCore::instance()->initialize())
        LOG_ERROR("Category core not initialized");
}

ExtensionSystem::IPlugin::ShutdownFlag CategoryPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(CategoryPlugin)
