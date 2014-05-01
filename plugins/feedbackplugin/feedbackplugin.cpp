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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "feedbackplugin/feedbackplugin.h"
#include "feedbackplugin/feedbackconstants.h"
#include "feedbackplugin/bugreportdialog.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/constants_menus.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>

#include <QtPlugin>
#include <QDebug>
#include <QAction>

using namespace Feedback::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

FeedbackPlugin::FeedbackPlugin()
{
    setObjectName("FeedbackPlugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating Feedback";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_feedback");

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool

    // All preferences pages must be created in this part (before user connection)
    // And included in the QObject pool

    //    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

FeedbackPlugin::~FeedbackPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool FeedbackPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    if (Utils::Log::warnPluginsCreation()) {
        qWarning() << "creating Feedback";
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
    QAction *a = new QAction(this);

    a->setIcon(theme()->icon("face-sad.png"));
    Core::Command *cmd = am->registerAction(a, Core::Constants::A_BUGREPORT, Core::Context(Core::Constants::C_GLOBAL));
    cmd->setTranslations(Trans::Constants::BUG_REPORT);
    Core::ActionContainer *menu = am->actionContainer(Core::Constants::M_HELP);
    menu->addAction(cmd, Core::Id(Core::Constants::G_HELP_DEBUG));

    connect(a, SIGNAL(triggered()), this, SLOT(reportBug()));

    return true;
}

void FeedbackPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation()) {
        qWarning() << "Feedback::extensionsInitialized";
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

    messageSplash(tr("Initializing Feedback..."));

    // At this point, user is connected

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void FeedbackPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Core is fully intialized as well as all plugins
    // DataPacks are checked
}

ExtensionSystem::IPlugin::ShutdownFlag FeedbackPlugin::aboutToShutdown()
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

/** Starts the Utils::BugReportDialog with the correct categories */
void FeedbackPlugin::reportBug()
{
    Feedback::BugReportDialog dlg;
    QStringList categories;
    categories << tr("General comment")
               << tr("Forms management")
               << tr("Patient management")
               << tr("User management")
               << tr("Drugs management")
               << tr("Agenda management")
               << tr("Printings")
               << tr("Installation")
               << tr("Configuration");
    dlg.setBugCategories(categories);
    int result = dlg.exec();
    Q_UNUSED(result);
    return;
}

// void FeedbackPlugin::triggerAction()
// {
//     QMessageBox::information(Core::ICore::instance()->mainWindow(),
//                              tr("Action triggered"),
//                              tr("This is an action from Feedback."));
// }

void FeedbackPlugin::coreAboutToClose()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Core is about to close
    // ICore::user() is still available
}

Q_EXPORT_PLUGIN(FeedbackPlugin)

