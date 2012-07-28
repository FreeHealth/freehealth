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
#include "mainwindowplugin.h"
#include "mainwindow.h"

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>

#include <QDebug>

using namespace MainWin;

MainWinPlugin::MainWinPlugin() :
        m_MainWindow(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating FREEDIAMS::MainWinPlugin";
    m_MainWindow = new MainWindow();
    Core::ICore::instance()->setMainWindow(m_MainWindow);
}

MainWinPlugin::~MainWinPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "MainWinPlugin::~MainWinPlugin()";
    if (m_MainWindow) {
        delete m_MainWindow;
        m_MainWindow = 0;
    }
}

bool MainWinPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "MainWinPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("fdmainwindowplugin");

    m_MainWindow->initialize(QStringList(),0);
    return true;
}

void MainWinPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "MainWinPlugin::extensionsInitialized";

    m_MainWindow->extensionsInitialized();
}

ExtensionSystem::IPlugin::ShutdownFlag MainWinPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (m_MainWindow->isVisible())
        m_MainWindow->hide();

    // m_MainWindow is deleted by Core
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(MainWinPlugin)
