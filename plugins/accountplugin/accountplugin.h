/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef ACCOUNT_PLUGIN_H
#define ACCOUNT_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file accountplugin.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.1.0
 * \date 13 Mar 2011
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

class AccountPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    AccountPlugin();
    ~AccountPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

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
};


}  // End namespace Account

#endif    // End ACCOUNT_PLUGIN_H
