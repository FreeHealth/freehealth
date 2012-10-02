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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "rapidcomptaportingplugin.h"
#include "rapidtofreeviewer.h"
#include "rapidtofreeio.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>

#include <QtPlugin>
#include <QDebug>

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

RapidPortPlugin::RapidPortPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating RapidPortPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("RapidPortPlugin");

    // Create the database instance
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

RapidPortPlugin::~RapidPortPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "RapidPortPlugin::~RapidPortPlugin()";
}

bool RapidPortPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "RapidPortPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void RapidPortPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "RapidPortPlugin::extensionsInitialized";

//    if (!user())
//        return;
//    if (user()->uuid().isEmpty())
//        return;

//    messageSplash(tr("Initializing accountancy plugin..."));

    // Initialize Account Database
//    AccountBase::instance()->initialize();
//    DatapackBase::instance()->initialize();

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

ExtensionSystem::IPlugin::ShutdownFlag RapidPortPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void RapidPortPlugin::postCoreInitialization()
{
    RapidToFreeIO * rIO = new RapidToFreeIO;
    if (!rIO->initialiseBases())
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect to databases" ;
    }
    RapidToFreeViewer *viewer = new RapidToFreeViewer;
    viewer->show();
}

Q_EXPORT_PLUGIN(RapidPortPlugin)
