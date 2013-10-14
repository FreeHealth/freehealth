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
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "ddiplugin.h"
#include "ddicore.h"
#include <ddiplugin/atc/atcmode.h>
#include <ddiplugin/interactors/interactormode.h>
#include <ddiplugin/ddi/drugdruginteractionmode.h>

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtPlugin>
#include <QDebug>

using namespace DDI;
using namespace Internal;

DDIPlugin::DDIPlugin() :
    _atcMode(0),
    _interactorMode(0),
    _ddiMode(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Creating DDIPlugin";
    Core::ICore::instance()->translators()->addNewTranslator("plugin_ddimanager");
}

DDIPlugin::~DDIPlugin()
{
    qWarning() << "DDIPlugin::~DDIPlugin()";
}

bool DDIPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DDIPlugin::initialize";

    // create the core
    DDICore *core = new DDICore(this);
    core->initialize();

    // create modes
    addObject(_ddiMode = new DDIMode(this));
    addObject(_interactorMode = new InteractorMode(this));
    addObject(_atcMode = new AtcMode(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    return true;
}

void DDIPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DDIPlugin::extensionsInitialized";
}

ExtensionSystem::IPlugin::ShutdownFlag DDIPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Here you still have a full access to
    //   Core::ICore::instance()
    // And all its objects (user(), patient(), settings(), theme()...).
    removeObject(_ddiMode);
    removeObject(_interactorMode);
    removeObject(_atcMode);

    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(DDIPlugin)
