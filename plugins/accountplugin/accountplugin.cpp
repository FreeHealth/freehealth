/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "accountplugin.h"
#include "accountwidgetmanager.h"

#ifdef FREEHEALTH
#    include "accountmode.h"
#endif

#include "preferences/accountpreferences.h"
#include "preferences/bankdetailspage.h"
#include "preferences/availablemovementpage.h"
#include "preferences/medicalprocedurepage.h"
#include "preferences/virtualdatabasecreator.h"
#include "preferences/sitespage.h"
#include "preferences/insurancepage.h"
#include "preferences/percentagespage.h"
#include "preferences/distancerulespage.h"
#include "preferences/assetsratespage.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Account;
using namespace Internal;

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

AccountPlugin::AccountPlugin() :
        ExtensionSystem::IPlugin(),
        m_UserPage(0),
        m_BankPage(new BankDetailsPage(this)),
        m_AvMovPage(new AvailableMovementPage(this)),
        m_MPPage(new MedicalProcedurePage(this)),
        m_VirtPage(new VirtualDatabaseCreatorPage(this)),
        m_SitesPage(new SitesPage(this)),
        m_InsurPage(new InsurancePage(this)),
        m_PercentPage(new PercentagesPage(this)),
        m_DistancePage(new DistanceRulesPage(this)),
        m_AssetsRatesPage(new AssetsRatesPage(this)),
        m_DefaultPage(new AccountDatabaseDefaultsPage(this))

{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating AccountPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_account");

    // Add pages to plugins manager object pool
    addObject(m_BankPage);
    addObject(m_AvMovPage);
    addObject(m_MPPage);
    addObject(m_VirtPage);
    addObject(m_SitesPage);
    addObject(m_InsurPage);
    addObject(m_PercentPage);
    addObject(m_DistancePage);
    addObject(m_AssetsRatesPage);
    addObject(m_DefaultPage);
}

void AccountPlugin::removeAndDelete(QObject *o)
{
    if (o) {
        removeObject(o);
        delete o;
        o = 0;
    }
}

AccountPlugin::~AccountPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "AccountPlugin::~AccountPlugin()";
}

bool AccountPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "AccountPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Create instance of WidgetManager and ActionHandler
    Account::AccountWidgetManager::instance();

    return true;
}

void AccountPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "AccountPlugin::extensionsInitialized";

    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    messageSplash(tr("Initializing accountancy plugin..."));

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // FreeAccount specific code
#ifdef FREEACCOUNT
    m_UserPage = new AccountUserOptionsPage(this);
    m_UserPage->checkSettingsValidity();
    addObject(m_UserPage);
#endif

    // FreeMedForms specific code
#ifdef FREEHEALTH
    addAutoReleasedObject(new Account::Internal::AccountMode(this));
#endif

    // Check settings validity
    m_BankPage->checkSettingsValidity();
    m_AvMovPage->checkSettingsValidity();
    m_MPPage->checkSettingsValidity();
    m_VirtPage->checkSettingsValidity();
    m_SitesPage->checkSettingsValidity();
    m_InsurPage->checkSettingsValidity();
    m_PercentPage->checkSettingsValidity();
    m_DistancePage->checkSettingsValidity();
    m_AssetsRatesPage->checkSettingsValidity();
    m_DefaultPage->checkSettingsValidity();
}

ExtensionSystem::IPlugin::ShutdownFlag AccountPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    removeAndDelete(m_UserPage);
    removeAndDelete(m_BankPage);
    removeAndDelete(m_AvMovPage);
    removeAndDelete(m_MPPage);
    removeAndDelete(m_VirtPage);
    removeAndDelete(m_SitesPage);
    removeAndDelete(m_InsurPage);
    removeAndDelete(m_PercentPage);
    removeAndDelete(m_DistancePage);
    removeAndDelete(m_AssetsRatesPage);
    removeAndDelete(m_DefaultPage);
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(AccountPlugin)
