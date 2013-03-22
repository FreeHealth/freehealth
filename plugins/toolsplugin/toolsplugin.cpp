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
#include <coreplugin/constants_icons.h>
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
const char* const PRINT_FSP    = "Imprimer la FSP";
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
    Core::ActionContainer *menu = actionManager()->createMenu(Core::Constants::M_GENERAL);

    QAction *action = new QAction(this);
    action->setIcon(theme()->icon(Core::Constants::ICONCHEQUE));
    Core::Command *cmd = actionManager()->registerAction(action, "aTools.PrintCheque", Core::Context(Core::Constants::C_GLOBAL));
    cmd->setTranslations(::PRINT_CHEQUE, ::PRINT_CHEQUE, "Tools");
    //: Translation for the 'Print Cheque' action
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+C")));
    connect(action, SIGNAL(triggered()), this, SLOT(printCheque()));
    menu->addAction(cmd, Core::Id(Core::Constants::G_GENERAL_PRINT));

    QAction *printFsp = new QAction(this);
    printFsp->setIcon(theme()->icon(Core::Constants::ICONCHEQUE));
    cmd = actionManager()->registerAction(printFsp, "aTools.PrintFsp", Core::Context(Core::Constants::C_GLOBAL));
    printFsp->setText(::PRINT_FSP);
    //: Translation for the 'Print FSP' action
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+F")));
    connect(printFsp, SIGNAL(triggered()), this, SLOT(printFsp()));
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
    ChequePrinterDialog printDialog;
    printDialog.setPlace(settings()->value(S_PLACE).toString());
    printDialog.setDate(QDate::currentDate());
    printDialog.setOrder(settings()->value(S_ORDER).toString());
    printDialog.setDefaultAmounts(settings()->value(S_VALUES).toStringList());
    printDialog.exec();
}

#include "fspprinter.h"
void ToolsPlugin::printFsp()
{
    FspPrinter printer;
    printer.setBillNumber("123456789012345");
    printer.setBillDate(QDate::currentDate());

    Fsp_Patient patient;
    patient.fullName = "NOM PATIENT ET PRENOM";
    patient.dob = QDate(1974, 11, 7);
    patient.nss = "1234567890123";
    patient.nssKey = "45";
    patient.assuranceNumber = "ASSURNB";
    patient.assuredName = "NOM DE L'ASSURÉ";
    patient.assuredNss = "ASSURE7890123";
    patient.assuredNssKey = "89";
    patient.address = "ADRESSE DU PATIENT SDFQSD FQSDF QSD FQSD FQSD FQSDFQSDFQSDF QSD F24352345 2345 21345 SQDFQSDF";
    printer.setPatient(patient);

    Fsp_Conditions conds;
    conds.maladie = true;
    conds.exonerationTM = true;
    conds.ald = true;
    conds.accidentParTiers = true;
    conds.accidentParTiersDate = QDate::currentDate();
    conds.prevention = true;
    conds.autre = true;
    conds.soinL115 = true;
    conds.maternite = true;
    conds.dateGrossesse = QDate::currentDate();
    conds.atMP = true;
    conds.nouveauMT = true;

    conds.atMPNumber = "12345678910182742394";
    conds.atMPDate = QDate(12,2,12);
    conds.envoyeParMedecin = "MEDECIN ENVOYANT";
    conds.accessSpecific = true;
    conds.urgence = true;
    conds.horsResidence = true;
    conds.medTraitantRemplace = true;
    conds.accessHorsCoordination = true;
    conds.dateAccordPrealable = QDate(13,3,12);
    printer.setConditions(conds);

    Fsp_AmountLine line;
    line.date = QDate::currentDate();
    line.act = "123456789012345678";
    line.otherAct1 = "O1";
    line.otherAct2 = "OOO";
    line.activity = 1;
    line.C = "CS";
    line.amount = 23.00;
    line.idMd = "EE";
    line.deplacement_nb = 2;
    line.deplacement_amount = 0.97;
    printer.addAmountLine(line);

    Fsp_AmountLine line2;
    line2.date = QDate::currentDate();
    line2.act = "ACT2342341";
    line2.otherAct1 = "--0";
    line2.otherAct2 = "__1";
    line2.activity = 2;
    line2.C = "CS";
    line2.amount = 976.00;
    line2.idMd = "S";
    line2.deplacement_nb = 12;
    line2.deplacement_amount = 0.97;
    printer.addAmountLine(line2);

    printer.setPayment(true, true);
    printer.print(FspPrinter::S12541_02);
}

Q_EXPORT_PLUGIN(ToolsPlugin)

