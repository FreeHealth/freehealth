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
 *  Main Developer: Jerome Pinguet <jerome@jerome.cc>                      *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "patientimportplugin.h"
#include "patientimportconstants.h"
#include "patientimportdialog.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>
//#include <coreplugin/contextmanager/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
//#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
//#include <coreplugin/coreconstants.h>
#include <coreplugin/constants_menus.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>

#include <QtPlugin>
#include <QDebug>
#include <QAction>

using namespace PatientImport::Internal;

static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

PatientimportPlugin::PatientimportPlugin()
{
    setObjectName("PatientimportPlugin");
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating patientimport";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("patientimport");

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool

    // All preferences pages must be created in this part (before user connection)
    // And included in the QObject pool

    //    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

PatientimportPlugin::~PatientimportPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool PatientimportPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    if (Utils::Log::debugPluginsCreation()) {
        qWarning() << "creating patientimport";
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


    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    //
    QAction *a = new QAction(tr("patientimport action"), this);
    a->setIcon(theme()->icon(Constants::PATIENTIMPORT_ICON));
    Core::Command *cmd = am->registerAction(a, Constants::ACTION_ID,
                                            Core::Context(Core::Constants::C_GLOBAL));
    cmd->setTranslations(Constants::PATIENT_IMPORT);

    //Core::ActionContainer *menu = am->createMenu(Constants::MENU_ID);
    //    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Shift+Meta+A")));
    //    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));
    //    Core::ActionContainer *menu = am->createMenu(Constants::CUSTOM_MENU_ID);
    //    menu->addAction(cmd);
    //menu->addAction(cmd, Core::Id(Core::Constants::G_GENERAL_PLUGINS));
    am->actionContainer(Core::Constants::M_PLUGINS)->addAction(cmd);
    connect(a, SIGNAL(triggered()), this, SLOT(openPatientImportDialog()));


    //    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void PatientimportPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation()) {
        qWarning() << "patientimport::extensionsInitialized";
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

    messageSplash(tr("Initializing patientimport..."));

    // At this point, user is connected

    // Add here e.g. the DataPackPlugin::IDataPackListener objects to the pluginmanager object pool

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void PatientimportPlugin::postCoreInitialization()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Core is fully intialized as well as all plugins
    // DataPacks are checked
}

void PatientimportPlugin::openPatientImportDialog()
{
    qWarning() << "PatientimportPlugin::openPatientImportDialog()";
    PatientImport::PatientImportDialog dlg;
    dlg.exec();
    return;
}

QStringList PatientimportPlugin::softwareList()
{
    QStringList softwareList;
    softwareList << PatientImport::Constants::CIELCOMPTA // Index 0
                 << PatientImport::Constants::GESTCAB;   // Index 1
    return softwareList;
}

ExtensionSystem::IPlugin::ShutdownFlag PatientimportPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Here you still have a full access to
    //   Core::ICore::instance()
    // And all its objects (user(), patient(), settings(), theme()...).

    return SynchronousShutdown;
}

// void PatientimportPlugin::triggerAction()
// {
//     QMessageBox::information(Core::ICore::instance()->mainWindow(),
//                              tr("Action triggered"),
//                              tr("This is an action from patientimport."));
// }

void PatientimportPlugin::coreAboutToClose()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Core is about to close
    // ICore::user() is still available
}

//void PatientimportPlugin::test_your_plugin_unit_test()
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

Q_EXPORT_PLUGIN(patientimportPlugin)
