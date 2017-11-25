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
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "datapackplugin.h"
#include "datapackcore.h"
#include "imode/datapackmode.h"

#include <coreplugin/icore.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/translators.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace DataPackPlugin;
using namespace Internal;

DataPackIPlugin::DataPackIPlugin()
    : _datapackMode(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "Creating DataPackIPlugin";
//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    Core::ICore::instance()->translators()->addNewTranslator("plugin_fddi_datapack");
}

DataPackIPlugin::~DataPackIPlugin()
{
    qWarning() << "DataPackIPlugin::~DataPackIPlugin()";
}

bool DataPackIPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DataPackIPlugin::initialize";

    // Create the core
    DataPackCore *core = new DataPackCore(this);
    core->initialize();

    // Add pages
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Create modes
    addObject(_datapackMode = new DataPackMode(this));

    return true;
}

void DataPackIPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DataPackIPlugin::extensionsInitialized";
}

ExtensionSystem::IPlugin::ShutdownFlag DataPackIPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Here you still have a full access to
    //   Core::ICore::instance()
    // And all its objects (user(), patient(), settings(), theme()...).

    removeObject(_datapackMode);
    delete _datapackMode;
    _datapackMode = 0;

    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(DataPackIPlugin)
