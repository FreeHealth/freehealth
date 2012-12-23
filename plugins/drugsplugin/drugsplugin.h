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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSWIDGETPLUGIN_H
#define DRUGSWIDGETPLUGIN_H

#include <extensionsystem/iplugin.h>

/**
 * \file drugsplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace DrugsWidget {
namespace Internal {
class DrugGeneralOptionsPage;
class DrugsSelectorOptionsPage;
class DrugPosologicSentencePage;
class DrugsUserOptionsPage;
class DrugsExtraOptionsPage;
class ProtocolPreferencesPage;
class DrugsDatabaseSelectorPage;
class DrugEnginesPreferencesPage;

class DrugsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.DrugsPlugin" FILE "Drugs.json")

public:
    DrugsPlugin();
    ~DrugsPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreOpened();

private:
    DrugGeneralOptionsPage *viewPage;
    DrugsSelectorOptionsPage *selectorPage;
    DrugPosologicSentencePage *posologicPage;
    DrugsUserOptionsPage *userPage;
    DrugsExtraOptionsPage *extraPage;
    DrugsDatabaseSelectorPage *databaseSelectorPage;
    ProtocolPreferencesPage *protocolPage;
    DrugEnginesPreferencesPage *enginePage;
};

} // namespace Internal
} // namespace DrugsWidget

#endif // DRUGSWIDGETPLUGIN_H
