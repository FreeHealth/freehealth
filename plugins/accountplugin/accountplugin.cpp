/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "accountplugin.h"
#include "accountwidgetmanager.h"

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

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Account;

AccountPlugin::AccountPlugin() :
        ExtensionSystem::IPlugin(),
        m_UserPage(new AccountUserOptionsPage(this)),
        m_BankPage(new BankDetailsPage(this)),
        m_AvMovPage(new AvailableMovementPage(this)),
        m_MPPage(new MedicalProcedurePage(this)),
        m_VirtPage(new VirtualDatabaseCreatorPage(this)),
        m_SitesPage(new SitesPage(this)),
        m_InsurPage(new InsurancePage(this)),
        m_PercentPage(new PercentagesPage(this)),
        m_DistancePage(new DistanceRulesPage(this)),
        m_AssetsRatesPage(new AssetsRatesPage(this))

{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating AccountPlugin";
}

AccountPlugin::~AccountPlugin()
{
    qWarning() << "AccountPlugin::~AccountPlugin()";
    // Remove preferences pages to plugins manager object pool
    removeObject(m_UserPage);
    removeObject(m_BankPage);
    removeObject(m_AvMovPage);
    removeObject(m_MPPage);
    removeObject(m_VirtPage);
    removeObject(m_SitesPage);
    removeObject(m_InsurPage);
    removeObject(m_PercentPage);
    removeObject(m_DistancePage);
    removeObject(m_AssetsRatesPage);
}

bool AccountPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AccountPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Create instance of WidgetManager and ActionHandler
    Account::AccountWidgetManager::instance();

    return true;
}

void AccountPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AccountPlugin::extensionsInitialized";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("accountplugin");
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Check settings validity
    m_UserPage->checkSettingsValidity();
    m_BankPage->checkSettingsValidity();
    m_AvMovPage->checkSettingsValidity();
    m_MPPage->checkSettingsValidity();
    m_VirtPage->checkSettingsValidity();
    m_SitesPage->checkSettingsValidity();
    m_InsurPage->checkSettingsValidity();
    m_PercentPage->checkSettingsValidity();
    m_DistancePage->checkSettingsValidity();
    m_AssetsRatesPage->checkSettingsValidity();

    // Add pages to plugins manager object pool
    addObject(m_UserPage);
    addObject(m_BankPage);
    addObject(m_AvMovPage);
    addObject(m_MPPage);
    addObject(m_VirtPage);
    addObject(m_SitesPage);
    addObject(m_InsurPage);
    addObject(m_PercentPage);
    addObject(m_DistancePage);
    addObject(m_AssetsRatesPage);
}


Q_EXPORT_PLUGIN(AccountPlugin)
