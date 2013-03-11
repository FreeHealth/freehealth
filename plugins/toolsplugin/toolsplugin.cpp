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
 *   Main Developer: Eric Maeker <eric.maeker@gmail.com>                  *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "toolsplugin.h"
#include "toolsconstants.h"
#include "pdftkwrapper.h"
#include "chequeprinterdialog.h"
#include "chequeprinter_preferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>

#include <QtPlugin>
#include <QAction>
#include <QDebug>

using namespace Tools;
using namespace Internal;

static inline Core::IScriptManager *scriptManager()  { return Core::ICore::instance()->scriptManager(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager()  { return Core::ICore::instance()->actionManager(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

namespace {
const char* const PRINT_CHEQUE = QT_TRANSLATE_NOOP("Tools", "Print a cheque");
}

ToolsPlugin::ToolsPlugin() :
    ExtensionSystem::IPlugin(),
    m_prefPage(0),
    pdf(0)
{
    setObjectName("ToolsPlugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating Tools";
    
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_tools");
    
    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool
    
    // All preferences pages must be created in this part (before user connection)
    // And included in the QObject pool
//    m_prefPage = new ToolsPreferencesPage(this);
//    addObject(m_prefPage);
    addAutoReleasedObject(new ChequePrinterPreferencesPage(this));

//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
//    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

ToolsPlugin::~ToolsPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool ToolsPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    if (Utils::Log::warnPluginsCreation()) {
        qWarning() << "Tools::initialize";
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
    
    return true;
}

void ToolsPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation()) {
        qWarning() << "Tools::extensionsInitialized";
    }
    
    // Retrieve other objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
    
    // If you want to stop the plugin initialization if there are no identified user
    // Just uncomment the following code
    //    // no user -> stop here
    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;
    
    messageSplash(tr("Initializing Tools..."));
    
    // At this point, user is connected

    // Create some menu actions
    QAction *action = new QAction(this);
    Core::Command *cmd = actionManager()->registerAction(action, "aTools.PrintCheque", Core::Context(Core::Constants::C_GLOBAL));
    cmd->setTranslations(::PRINT_CHEQUE, ::PRINT_CHEQUE, "Tools");
    //: Translation for the 'Print Cheque' action
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+C")));
    connect(action, SIGNAL(triggered()), this, SLOT(printCheque()));

    Core::ActionContainer *menu = actionManager()->createMenu(Core::Constants::M_GENERAL);
    menu->addAction(cmd, Core::Id(Core::Constants::G_GENERAL_PRINT));
    // TODO: add action to the mode manager ?

    // Add here e.g. the DataPackPlugin::IDataPackListener objects to the pluginmanager object pool
    
    // Add Tools to ScriptManager
    pdf = new PdfTkWrapper(this);
    pdf->initialize();
    QScriptValue pdfValue = scriptManager()->addScriptObject(pdf);
    scriptManager()->evaluate("namespace.com.freemedforms").setProperty("pdf", pdfValue);

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void ToolsPlugin::postCoreInitialization()
{
    // Core is fully intialized as well as all plugins
    // DataPacks are checked
}

ExtensionSystem::IPlugin::ShutdownFlag ToolsPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void ToolsPlugin::printCheque()
{
    Q_ASSERT(pdf);
    ChequePrinterDialog printDialog;
    printDialog.setPlace(settings()->value(S_PLACE).toString());
    printDialog.setDate(QDate::currentDate());
    printDialog.setOrder(settings()->value(S_ORDER).toString());
    printDialog.setDefaultAmounts(settings()->value(S_VALUES).toStringList());
    printDialog.exec();
}

Q_EXPORT_PLUGIN(ToolsPlugin)

