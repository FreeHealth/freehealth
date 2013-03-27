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
 *   Main Developer: %Author% <%AuthorEmail%>                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "%PluginName:l%plugin.%CppHeaderSuffix%"
#include "%PluginName:l%constants.%CppHeaderSuffix%"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>
//#include <coreplugin/contextmanager/icontext.h>
//#include <coreplugin/actionmanager/actionmanager.h>
//#include <coreplugin/actionmanager/command.h>
//#include <coreplugin/actionmanager/actioncontainer.h>
//#include <coreplugin/coreconstants.h>

#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>

#include <QtPlugin>
#include <QDebug>

using namespace %PluginName%::Internal;

static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

%PluginName%Plugin::%PluginName%Plugin()
{
    setObjectName("%PluginName%Plugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating %PluginName%";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("%PluginName:l%");

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool

    // All preferences pages must be created in this part (before user connection)
    // And included in the QObject pool

//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

%PluginName%Plugin::~%PluginName%Plugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool %PluginName%Plugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    if (Utils::Log::warnPluginsCreation()) {
        qWarning() << "creating %PluginName%";
    }

    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // connect to other plugins' signals
    // "In the initialize method, a plugin can be sure that the plugins it
    //  depends on have initialized their members."

    // FreeMedForms:
    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool

    // No user is logged in until here


//    Core::ActionManager *am = Core::ICore::instance()->actionManager();
//
//    QAction *action = new QAction(tr("%PluginName% action"), this);
//    Core::Command *cmd = am->registerAction(action, Constants::ACTION_ID,
//                         Core::Context(Core::Constants::C_GLOBAL));
//    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Shift+Meta+A")));
//    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));
//
//    Core::ActionContainer *menu = am->createMenu(Constants::CUSTOM_MENU_ID);
//    menu->menu()->setTitle(tr("%PluginName%"));
//    menu->addAction(cmd);
//    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void %PluginName%Plugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation()) {
        qWarning() << "%PluginName%::extensionsInitialized";
    }

    // Add Translator to the Application
    //Core::ICore::instance()->translators()->addNewTranslator("mf_XXX_Widget");

    // Retrieve other objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."

    // If you want to stop the plugin initialization if there are no identified user
    // Just uncomment the following code
//    // no user -> stop here
//    if (!user())
//        return;
//    if (user()->uuid().isEmpty())
//        return;

    messageSplash(tr("Initializing %PluginName%..."));

    // At this point, user is connected

    // Add here e.g. the DataPackPlugin::IDataPackListener objects to the pluginmanager object pool

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void %PluginName%Plugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Core is fully intialized as well as all plugins
    // DataPacks are checked
}

ExtensionSystem::IPlugin::ShutdownFlag %PluginName%Plugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Here you still have a full access to
    //   Core::ICore::instance()
    // And all its objects (user(), patient(), settings(), theme()...).

    return SynchronousShutdown;
}

// void %PluginName%Plugin::triggerAction()
// {
//     QMessageBox::information(Core::ICore::instance()->mainWindow(),
//                              tr("Action triggered"),
//                              tr("This is an action from %PluginName%."));
// }

void %PluginName%Plugin::coreAboutToClose()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Core is about to close
    // ICore::user() is still available
}

Q_EXPORT_PLUGIN(%PluginName%Plugin)
