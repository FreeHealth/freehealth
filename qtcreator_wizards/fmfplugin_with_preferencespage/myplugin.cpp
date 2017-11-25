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
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating %PluginName%";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_%PluginName:l%");

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool

    // All preferences pages must be created in this part (before user connection)
    // And included in the QObject pool
    m_prefPage = new %PluginName:c%PreferencesPage(this);
    addObject(m_prefPage);

    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

%PluginName%Plugin::~%PluginName%Plugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool %PluginName%Plugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    if (Utils::Log::debugPluginsCreation()) {
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
    if (Utils::Log::debugPluginsCreation()) {
        qWarning() << "%PluginName%::extensionsInitialized";
    }

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
    // Core is fully intialized as well as all plugins
    // DataPacks are checked
}

// aboutToShutdown does not exist in the old QtCreator code.
// we have to wait until FMF is updatet to a newer QtCreator source
ExtensionSystem::IPlugin::ShutdownFlag %PluginName%Plugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
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
    // Core is about to close
    // ICore::user() is still available
}

//void %PluginName%Plugin::test_your_plugin_unit_test()
//{
    // Write your plugin unit test here.

    // Available configuration:
    // With this unit testing, you can manage all plugin dependencies (Core and others).
    // All plugins will be fully loaded and initialized *before* tests are executed.

    // File/code organization:
    // You can include tests in this file, or create a specific path for 'tests'
    // for your plugin and store there only the cpp file including your tests.
    // All tests must be private Q_SLOTS of this ExtensionSystem::IPlugin object and
    // must be named test_*().

    // Test behavior:
    // If your one of your test fail, the testing process must be stopped.
//}

Q_EXPORT_PLUGIN(%PluginName%Plugin)
