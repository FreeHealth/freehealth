/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mainwindowplugin.h"
#include "mainwindow.h"
#include "mainwindowpreferences.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>

#include <QDebug>

using namespace MainWin;

MainWinPlugin::MainWinPlugin() :
        m_MainWindow(0), prefPage(0), virtualBasePage(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating FREEMEDFORMS::MainWinPlugin";

    // Declare MainWindow to the Core
    m_MainWindow = new MainWindow;
    Core::ICore::instance()->setMainWindow(m_MainWindow);
    m_MainWindow->init();
}

MainWinPlugin::~MainWinPlugin()
{
    qWarning() << "MainWinPlugin::~MainWinPlugin()";
    if (virtualBasePage) {
        removeObject(virtualBasePage);
        delete virtualBasePage; virtualBasePage=0;
    }
    // m_MainWindow is deleted by Core
}

bool MainWinPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "FREEMEDFORMS::MainWinPlugin::initialize";

    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("fmfmainwindowplugin");

    // Add preferences pages
    virtualBasePage = new Internal::VirtualBasePage();
    virtualBasePage->checkSettingsValidity();
    addObject(virtualBasePage);

    m_MainWindow->initialize(arguments, errorString);
    return true;
}

void MainWinPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "FREEMEDFORMS::MainWinPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    m_MainWindow->extensionsInitialized();
}

Q_EXPORT_PLUGIN(MainWinPlugin)
