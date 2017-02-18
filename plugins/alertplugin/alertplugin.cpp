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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertplugin.h"
#include "alertcore.h"
#include "alertitem.h"
#include "alertpreferences.h"
#include "patientbaralertplaceholder.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/ipatientbar.h>
#include <coreplugin/translators.h>

#include <QtPlugin>
#include <QDebug>

using namespace Alert;
using namespace Internal;

static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

AlertPlugin::AlertPlugin() :
    ExtensionSystem::IPlugin(),
    _prefPage(0),
    _patientPlaceHolder(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating AlertPlugin";
    setObjectName("AlertPlugin");

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_alert");

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool

    // All preferences pages must be created in this part (before user connection)
    _prefPage = new AlertPreferencesPage(this);
    // And included in the QObject pool
    addObject(_prefPage);

    // Create the core instance
    new AlertCore(this);

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

AlertPlugin::~AlertPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool AlertPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "AlertPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // No user connected here

    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool

    return true;
}

void AlertPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "AlertPlugin::extensionsInitialized";

    // If you want to stop the plugin initialization if there are no identified user
    // Just uncomment the following code
    // no user -> end
    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    messageSplash(tr("Initializing AlertPlugin..."));

    // At this point, user is connected
    AlertCore::instance().initialize();

    // Add here the DataPackPlugin::IDataPackListener objects to the pluginmanager object pool

    // Add the patientalertplaceholder
    LOG("Creating patient alert placeholder");
    _patientPlaceHolder = new PatientBarAlertPlaceHolder(this);
    addObject(_patientPlaceHolder);
    if (patient()->patientBar())
        patient()->patientBar()->addBottomWidget(_patientPlaceHolder->createWidget(patient()->patientBar()));

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void AlertPlugin::postCoreInitialization()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    // Core is fully intialized as well as all plugins
    // DataPacks are checked

    AlertCore::instance().postCoreInitialization();
}

void AlertPlugin::coreAboutToClose()
{
    // Core is about to close
    // ICore::user() is still available
}

ExtensionSystem::IPlugin::ShutdownFlag AlertPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (_prefPage)
        removeObject(_prefPage);
    if (_patientPlaceHolder)
        removeObject(_patientPlaceHolder);
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(AlertPlugin)
