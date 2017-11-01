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
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "coreplugin.h"
#include "coreimpl.h"
#include "pathpreferences.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <coreplugin/appaboutpage.h>
#include <coreplugin/dialogs/commonaboutpages.h>
#include <coreplugin/dialogs/commondebugpages.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/applicationgeneralpreferences.h>
#include <coreplugin/pathpreferences.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Core::Internal;

CorePlugin::CorePlugin() :
    m_CoreImpl(new CoreImpl(this)),
    prefPage(0)
{
}

CorePlugin::~CorePlugin()
{
    qWarning() << "CorePlugin::~CorePlugin()";
    if (prefPage) {
        removeObject(prefPage);
        delete prefPage; prefPage=0;
    }
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "CorePlugin::initialize";
    return m_CoreImpl->initialize(arguments,errorMessage);
}

void CorePlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "CorePlugin::extensionsInitialized";

    // add about pages
    addAutoReleasedObject(new AppAboutPage(this));
    addAutoReleasedObject(new TeamAboutPage(this));
    addAutoReleasedObject(new LicenseAboutPage(this));
    addAutoReleasedObject(new BuildAboutPage(this));
    // add debugging pages
    addAutoReleasedObject(new LogErrorDebugPage(this));
    addAutoReleasedObject(new LogMessageDebugPage(this));
    addAutoReleasedObject(new SettingDebugPage(this));
    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // add preferences page
    prefPage = new PathPreferencesPage(this);
    prefPage->checkSettingsValidity();
    addObject(prefPage);

//    prefPage = new ApplicationGeneralPreferencesPage(this);
//    prefPage->checkSettingsValidity();
//    addObject(prefPage);

    // All pages must be included in the plugin manager here
    // So that mainwindow can construct the full tree page
    m_CoreImpl->extensionsInitialized();
}

void CorePlugin::remoteArgument(const QString &arg)
{
    qWarning() << arg;
    Q_UNUSED(arg);
    // An empty argument is sent to trigger activation
    // of the window via QtSingleApplication. It should be
    // the last of a sequence.
//    if (arg.isEmpty()) {
//        m_mainWindow->activateWindow();
//    } else {
//        m_mainWindow->openFiles(QStringList(arg));
//    }
}

ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Here you still have a full access to
    //   Core::ICore::instance()
    // And all its objects (user(), patient(), settings(), theme()...).

    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(CorePlugin)
