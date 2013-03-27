/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT_PLUGIN_H
#define ACCOUNT_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file accountplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace Account {
class AccountUserOptionsPage;
class BankDetailsPage;
class AvailableMovementPage;
class MedicalProcedurePage;
class VirtualDatabaseCreatorPage;
class SitesPage;
class InsurancePage;
class PercentagesPage;
class DistanceRulesPage;
class AssetsRatesPage;
class AccountDatabaseDefaultsPage;

namespace Internal {
class AccountPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.AccountPlugin" FILE "Account.json")

public:
    AccountPlugin();
    ~AccountPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:
    void removeAndDelete(QObject *o);

private:
    AccountUserOptionsPage *m_UserPage;
    BankDetailsPage *m_BankPage;
    AvailableMovementPage *m_AvMovPage;
    MedicalProcedurePage *m_MPPage;
    VirtualDatabaseCreatorPage *m_VirtPage;
    SitesPage *m_SitesPage;
    InsurancePage *m_InsurPage;
    PercentagesPage *m_PercentPage;
    DistanceRulesPage *m_DistancePage;
    AssetsRatesPage *m_AssetsRatesPage;
    AccountDatabaseDefaultsPage *m_DefaultPage;
};

} // namespace Internal
}  // End namespace Account

#endif    // End ACCOUNT_PLUGIN_H
