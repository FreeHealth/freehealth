/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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
        m_DistancePage(new DistanceRulesPage(this))

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
}


Q_EXPORT_PLUGIN(AccountPlugin)
