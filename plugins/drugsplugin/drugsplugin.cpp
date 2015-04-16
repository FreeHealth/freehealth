/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \namespace DrugsWidget
  \brief Namespace reserved for the Drugs plugin.
  The DrugsWidget namespace includes:
  - All dialogs, widgets and views related to drugs (needs models classes issued from DrugsDB namespace)
  - All preferences pages (constants are issued from DrugsDB namespace)
  - Some private constants (reserved to the Drugs UI)
*/

#include "drugsplugin.h"
#include "constants.h"
#include "drugspreferences/druggeneralpreferences.h"
#include "drugspreferences/drugselectorpreferences.h"
#include "drugspreferences/drugposologicsentencepreferences.h"
#include "drugspreferences/druguserpreferences.h"
#include "drugspreferences/drugextrapreferences.h"
#include "drugspreferences/databaseselectorwidget.h"
#include "drugspreferences/protocolpreferencespage.h"
#include "drugspreferences/drugenginespreferences.h"

#ifdef FREEMEDFORMS
#    include "drugswidgetfactory.h"
#    include "drugsmode.h"
#endif

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace DrugsWidget::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

DrugsPlugin::DrugsPlugin() :
    viewPage(0),
    selectorPage(0),
    posologicPage(0),
    userPage(0),
    extraPage(0),
    databaseSelectorPage(0),
    protocolPage(0),
    enginePage(0)
{
    setObjectName("DrugsPlugin");
    if (Utils::Log::debugPluginsCreation())
#ifdef FREEMEDFORMS
        qWarning() << "creating FREEMEDFORMS::DrugsPlugin";
#endif
#ifdef FREEDIAMS
        qWarning() << "creating FREEDIAMS::DrugsPlugin";
#endif

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_drugs");

    viewPage = new DrugGeneralOptionsPage(this);
    selectorPage = new DrugsSelectorOptionsPage(this);
    posologicPage = new DrugPosologicSentencePage(this);
#ifdef FREEDIAMS
    userPage = new DrugsUserOptionsPage(this);
#endif
    extraPage = new DrugsExtraOptionsPage(this);
    databaseSelectorPage = new DrugsDatabaseSelectorPage(this);
    protocolPage = new ProtocolPreferencesPage(this);
    enginePage = new DrugEnginesPreferencesPage(this);

    addObject(viewPage);
    addObject(selectorPage);
    addObject(posologicPage);
#ifdef FREEDIAMS
    addObject(userPage);
#endif
    addObject(extraPage);
    addObject(databaseSelectorPage);
    addObject(protocolPage);
    addObject(enginePage);
}

DrugsPlugin::~DrugsPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool DrugsPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DrugsPlugin::initialize";

    messageSplash(tr("Initializing drugs plugin..."));

    addAutoReleasedObject(new Core::PluginAboutPage(this->pluginSpec(), this));

#ifdef FREEMEDFORMS
    addAutoReleasedObject(new DrugsWidgetsFactory(this));
#endif

    // Initialize drugs database after the settings where checked
//    drugsBase();

    return true;
}

void DrugsPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DrugsPlugin::extensionsInitialized";

    messageSplash(tr("Initializing drugs plugin..."));

    // check settings
    if (!settings()->value(Constants::S_CONFIGURED, false).toBool()) {
        viewPage->writeDefaultSettings(Core::ICore::instance()->settings());
        selectorPage->writeDefaultSettings(Core::ICore::instance()->settings());
        posologicPage->writeDefaultSettings(Core::ICore::instance()->settings());
#ifdef FREEDIAMS
        userPage->writeDefaultSettings(Core::ICore::instance()->settings());
#endif
        extraPage->writeDefaultSettings(Core::ICore::instance()->settings());
        databaseSelectorPage->writeDefaultSettings(Core::ICore::instance()->settings());
        protocolPage->writeDefaultSettings(Core::ICore::instance()->settings());
        enginePage->writeDefaultSettings(Core::ICore::instance()->settings());
        settings()->setValue(Constants::S_CONFIGURED, true);
        settings()->sync();
    } else {
        viewPage->checkSettingsValidity();
        selectorPage->checkSettingsValidity();
        posologicPage->checkSettingsValidity();
#ifdef FREEDIAMS
        userPage->checkSettingsValidity();
#endif
        extraPage->checkSettingsValidity();
        databaseSelectorPage->checkSettingsValidity();
        protocolPage->checkSettingsValidity();
        enginePage->checkSettingsValidity();
    }

#ifdef FREEMEDFORMS
    // Add drug mode. DrugsMode manages its inclusion in pluginManager itself.
    DrugsMode *mode = new DrugsMode(this);
    Q_UNUSED(mode);
#endif

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreOpened()));
}

void DrugsPlugin::postCoreOpened()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << Q_FUNC_INFO;
}

ExtensionSystem::IPlugin::ShutdownFlag DrugsPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (viewPage) {
        removeObject(viewPage);
        delete viewPage; viewPage=0;
    }
    if (selectorPage) {
        removeObject(selectorPage);
        delete selectorPage; selectorPage=0;
    }
    if (userPage) {
        removeObject(userPage);
        delete userPage; userPage=0;
    }
    if (extraPage) {
        removeObject(extraPage);
        delete extraPage; extraPage=0;
    }
    if (posologicPage) {
        removeObject(posologicPage);
        delete posologicPage; posologicPage=0;
    }
    if (databaseSelectorPage) {
        removeObject(databaseSelectorPage);
        delete databaseSelectorPage; databaseSelectorPage=0;
    }
    if (protocolPage) {
        removeObject(protocolPage);
        delete protocolPage; protocolPage=0;
    }
    if (enginePage) {
        removeObject(enginePage);
        delete enginePage; enginePage=0;
    }
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(DrugsPlugin)
