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
#include "icd10dbplugin.h"
#include "icd10databasecreator.h"
#include "icdgrouplinker.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtPlugin>
#include <QDebug>

using namespace Icd10::Internal;

Icd10DbPlugin::Icd10DbPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "Creating Icd10DbPlugin";
}

Icd10DbPlugin::~Icd10DbPlugin()
{
    qWarning() << "Icd10DbPlugin::~Icd10DbPlugin()";
}

bool Icd10DbPlugin::initialize(const QStringList &/*arguments*/, QString */*errorMessage*/)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "Icd10DbPlugin::initialize";

    //    Core::ICore::instance()->translators()->addNewTranslator("freeicd-Icd10DbPlugin");

    addAutoReleasedObject(new Icd10DatabasePage(this));
    addAutoReleasedObject(new IcdGroupLinkerPage(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    return true;
}

void Icd10DbPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "Icd10DbPlugin::extensionsInitialized";
}

ExtensionSystem::IPlugin::ShutdownFlag Icd10DbPlugin::aboutToShutdown()
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

Q_EXPORT_PLUGIN(Icd10DbPlugin)
