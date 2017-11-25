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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
#include "mainwindowplugin.h"
#include "mainwindow.h"
#include "patientmodelwrapper.h"
#include "usermodelwrapper.h"
//#include "preferences/userpreferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>

#include <QDebug>

using namespace MainWin;
using namespace Internal;

MainWinPlugin::MainWinPlugin() :
        m_MainWindow(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating FREEPAD::MainWinPlugin";
    m_MainWindow = new MainWindow();
    Core::ICore::instance()->setMainWindow(m_MainWindow);
    m_MainWindow->initialize(QStringList(),0);
    Core::IPatient *p = new Internal::PatientModelWrapper(Core::ICore::instance());
    Core::ICore::instance()->setPatient(p);
    Core::IUser *u = new Internal::UserModelWrapper(Core::ICore::instance());
    Core::ICore::instance()->setUser(u);
}

MainWinPlugin::~MainWinPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "MainWinPlugin::~MainWinPlugin()";
    if (m_MainWindow)
        delete m_MainWindow;
}

bool MainWinPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	qDebug("Mainwindow::initialize");
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "MainWinPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

void MainWinPlugin::extensionsInitialized()
{
	qDebug("Mainwindow::extensionsInitialized");
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "MainWinPlugin::extensionsInitialized";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_freeportmainwindow");

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
//    addAutoReleasedObject(new ICD::IcdUserOptionsPage(this));

    m_MainWindow->extensionsInitialized();
}

Q_EXPORT_PLUGIN(MainWinPlugin)
