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
#include "preventionplugin.h"
#include "preventCore.h"
#include "connexion.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/appaboutpage.h>
#include <coreplugin/dialogs/commonaboutpages.h>
#include <coreplugin/dialogs/commondebugpages.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/applicationgeneralpreferences.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Prevention::Internal;

PreventionPlugin::PreventionPlugin() :
    prefPage(0)
{
    qWarning() << "PreventionPlugin::PreventionPlugin()";
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("lib_utils");
    Core::ICore::instance()->translators()->addNewTranslator("lib_translations");
//    Core::ICore::instance()->translators()->addNewTranslator("plugin_prevention");

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreOpened()));
}

PreventionPlugin::~PreventionPlugin()
{
    qWarning() << "PreventionPlugin::~CorePlugin()";
//    if (prefPage) {
//        removeObject(prefPage);
//        delete prefPage; prefPage=0;
//    }
}

bool PreventionPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "PreventionPlugin::initialize";
    return true;
}

void PreventionPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "PreventionPlugin::extensionsInitialized";


    // add about pages
//    addAutoReleasedObject(new AppAboutPage(this));
//    addAutoReleasedObject(new TeamAboutPage(this));
//    addAutoReleasedObject(new LicenseAboutPage(this));
//    addAutoReleasedObject(new BuildAboutPage(this));
    // add debugging pages
//    addAutoReleasedObject(new LogErrorDebugPage(this));
//    addAutoReleasedObject(new LogMessageDebugPage(this));
//    addAutoReleasedObject(new SettingDebugPage(this));
    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // add preferences page
//    prefPage = new ApplicationGeneralPreferencesPage(this);
//    prefPage->checkSettingsValidity();
//    addObject(prefPage);
}

void PreventionPlugin::postCoreOpened()
{
    Common::Connexion * c = new Common::Connexion;
    if (!c->connect())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "cannot connect to database" ;
        }
    PreventCore *p = new PreventCore;
    p->run();
}

Q_EXPORT_PLUGIN(PreventionPlugin)
