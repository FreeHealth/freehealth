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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "scriptplugin.h"
#include "scriptmanager.h"
#include "scriptwriterdialog.h"

#include <utils/log.h>
#include <utils/global.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <QtPlugin>
#include <QAction>
#include <QDebug>

static inline Core::ModeManager *modeManager()  { return Core::ICore::instance()->modeManager(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

static inline QAction *createAction(QObject *parent, const QString &name, const QString &icon,
                                    const QString &actionName,
                                    const Core::Context &context,
                                    const QString &trans, const QString &transContext,
                                    Core::Command *cmd,
                                    Core::ActionContainer *menu,
                                    const QString &group,
                                    QKeySequence::StandardKey key = QKeySequence::UnknownKey,
                                    bool checkable = false)
{
    Q_UNUSED(menu);
    Q_UNUSED(group);
    QAction *a = new QAction(parent);
    a->setObjectName(name);
    if (!icon.isEmpty())
        a->setIcon(theme()->icon(icon));
    if (checkable) {
        a->setCheckable(true);
        a->setChecked(false);
    }
    cmd = actionManager()->registerAction(a, Core::Id(actionName), context);
    if (!transContext.isEmpty())
        cmd->setTranslations(trans, trans, transContext);
    else
        cmd->setTranslations(trans, trans); // use the Trans::Constants tr context (automatic)
    if (key != QKeySequence::UnknownKey)
        cmd->setDefaultKeySequence(key);
//    if (menu)
//        menu->addAction(cmd, Core::Id(group));
    return a;
}

using namespace Script;
using namespace Internal;

ScriptPlugin::ScriptPlugin() :
    m_Manager(0),
    aScriptDialog(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating ScriptPlugin";

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_script");
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

    // Initialize script manager
    m_Manager = new ScriptManager(this);
    Core::ICore::instance()->setScriptManager(m_Manager);

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

    // In debug mode, add the script dialog
    if (!Utils::isReleaseCompilation()) {
        Core::Context ctx(Core::Constants::C_GLOBAL);
        Core::Command *cmd = 0;
        aScriptDialog = createAction(this, "aScriptDialog", Core::Constants::ICONPROCESS,
                                     "script.aScriptDialog",
                                     ctx,
                                     "", "",
                                     cmd,
                                     0, "",
                                     QKeySequence::UnknownKey, false);
        aScriptDialog->setText("Script Dialog");
        connect(aScriptDialog, SIGNAL(triggered()), this, SLOT(onScriptDialogTriggered()));
        modeManager()->addAction(aScriptDialog, 0);
    }
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

void ScriptPlugin::onScriptDialogTriggered()
{
    ScriptWriterDialog dlg(Core::ICore::instance()->mainWindow());
    dlg.initialize();
    dlg.exec();
}

Q_EXPORT_PLUGIN(ScriptPlugin)
