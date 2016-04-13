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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
  \namespace PadTools
  \brief Namespace reserved for the PadTools plugin.
  The PadTools namespace includes:
  - Token engine
  - PadDocument creator
  - PadWriter
*/

#include "padtoolsplugin.h"
#include "padtoolscore.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/translators.h>

#include <QtPlugin>
#include <QDebug>

using namespace PadTools;
using namespace Internal;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}

PadToolsPlugin::PadToolsPlugin() :
    ExtensionSystem::IPlugin(),
    _core(0),
    _impl(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating PadToolsPlugin";
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_padtools");

    // Create the Core::IPadTools implementation and register it to the Core::ICore::instance()
    _core = new  PadToolsCore(this);
}

PadToolsPlugin::~PadToolsPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool PadToolsPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "PadToolsPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void PadToolsPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "PadToolsPlugin::extensionsInitialized";

#if defined(FREEMEDFORMS) || defined(FREEACCOUNT)
    if (!user())
        return;
#endif

    _core->initialize();
    Core::ICore::instance()->patient()->registerPatientTokens();
    Core::ICore::instance()->user()->registerUserTokens();

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

ExtensionSystem::IPlugin::ShutdownFlag PadToolsPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool

    Core::ICore::instance()->setPadTools(0);

    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(PadToolsPlugin)
