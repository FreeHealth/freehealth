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
#include "ddiplugin.h"
#include "ddicore.h"
#include <ddiplugin/atc/atcmode.h>
#include <ddiplugin/interactors/interactormode.h>
#include <ddiplugin/ddi/drugdruginteractionmode.h>
#include <ddiplugin/server/servermanagermode.h>
#include <ddiplugin/components/componentatcmode.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtPlugin>
#include <QAction>
#include <QDebug>

using namespace DDI;
using namespace Internal;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

DDIPlugin::DDIPlugin() :
    _ddiCore(0),
    _atcMode(0),
    _interactorMode(0),
    _ddiMode(0),
    _serverMode(0),
    _componentAtcMode(0),
    aChangeLocalDb(0)
{
    if (Utils::Log::debugPluginsCreation())
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
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DDIPlugin::initialize";

    // create the core
    _ddiCore = new DDICore(this);
    _ddiCore->initialize();

    // create modes
    addObject(_ddiMode = new DDIMode(this));
    addObject(_interactorMode = new InteractorMode(this));
    addObject(_atcMode = new AtcMode(this));
    addObject(_componentAtcMode = new ComponentAtcMode(this));
    // TODO: The server mode is not yet implemented
    // addObject(_serverMode = new ServerManagerMode(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    return true;
}

void DDIPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DDIPlugin::extensionsInitialized";
    createActions();
}

ExtensionSystem::IPlugin::ShutdownFlag DDIPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
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
    if (_serverMode)
        removeObject(_serverMode);
    removeObject(_componentAtcMode);

    return SynchronousShutdown;
}

void DDIPlugin::createActions()
{
    // TODO: add a filerecentmanager here
    const char * const menuId = Core::Constants::M_FILE;
    const char * const groupNew =  Core::Constants::G_FILE_OPEN;

    Core::ActionContainer *menu = actionManager()->actionContainer(menuId);
    Q_ASSERT(menu);
    if (!menu)
        return;

    Core::Context ctx(Core::Constants::C_GLOBAL);
    QAction *a = 0;
    Core::Command *cmd = 0;

    // Add action: Change local ddi database
    a = aChangeLocalDb = new QAction(this);
    a->setObjectName("aChangeLocalDb");
     a->setIcon(QIcon(Core::Constants::ICONDATABASE));
    cmd = actionManager()->registerAction(aChangeLocalDb, Core::Id("DDIPlugin::aChangeLocalDb"), ctx);
    Q_ASSERT(cmd);
    cmd->setTranslations(tr("Change local database"));
    menu->addAction(cmd, Core::Id(groupNew));
    cmd->retranslate();
    connect(aChangeLocalDb, SIGNAL(triggered()), this, SLOT(changeLocalDatabase()));
}

bool DDIPlugin::changeLocalDatabase()
{
    return _ddiCore->changeLocalDatabaseTo();
}

Q_EXPORT_PLUGIN(DDIPlugin)
