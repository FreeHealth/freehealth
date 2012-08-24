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
#include "scriptplugin.h"
#include "scriptmanager.h"

#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <QtCore/QtPlugin>
#include <QDebug>

// TEST
#include <coreplugin/constants.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <utils/global.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
// END

using namespace Script;
using namespace Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

ScriptPlugin::ScriptPlugin() :
    m_Manager(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating ScriptPlugin";

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("scriptplugin");
}

ScriptPlugin::~ScriptPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool ScriptPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ScriptPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // No user connected here

    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool

    return true;
}

void ScriptPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ScriptPlugin::extensionsInitialized";

    // At this point, user is connected

    // All preferences pages must be created in this part (after user connection)

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void ScriptPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    // Core is fully intialized as well as all plugins

    // Initialize script manager
    m_Manager = new ScriptManager(this);

    // TEST

//    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_GENERAL);
//    Q_ASSERT(menu);
//    if (!menu)
//        return;
//    QList<int> ctx = QList<int>() << Core::Constants::C_GLOBAL_ID;

//    // renew script test
//    QAction *a = new QAction(this);
//    a->setObjectName("aTestScript");
//    Core::Command *cmd = actionManager()->registerAction(a, "aTestScript", ctx);
//    Q_ASSERT(cmd);
//    cmd->setTranslations("Test Script");
//    menu->addAction(cmd);
//    cmd->retranslate();
//    connect(a, SIGNAL(triggered()), this, SLOT(patientSelected()));

//    connect(Core::ICore::instance()->patient(), SIGNAL(currentPatientChanged()),
//            this, SLOT(patientSelected()));
    // END
}

void ScriptPlugin::patientSelected()
{
//    m_Manager->evaluate(Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath)+"/test.js"));
}

ExtensionSystem::IPlugin::ShutdownFlag ScriptPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(ScriptPlugin)
