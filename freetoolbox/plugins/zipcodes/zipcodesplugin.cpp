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
#include "zipcodesplugin.h"
#include "frenchzipcodespage.h"
#include "genericzipcodespage.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

// TODO: Ca zipcodes: http://www.populardata.com/postalcode_database.html */
// TODO: USA zipcodes: http://www.populardata.com/zipcode_database.html // http://federalgovernmentzipcodes.us/ //  http://www.free-zipcodes.com/download-zip-code-database.php */
// UK: http://www.freemaptools.com/download-uk-postcode-lat-lng.htm
// Australia http://auspost.com.au/apps/postcode.html
// Austria: http://www.post.at/geschaeftlich_adress_management_postlexikon.php -> "PLZ-Verzeichnis %Month% %YEAR%"
// (ALL !!!! : http://download.geonames.org/export/dump/ (no zip codes))
// ALL !!!! : http://download.geonames.org/export/zip/

using namespace ZipCodes;
using namespace Internal;

ZipCodesPlugin::ZipCodesPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "Creating ZipCodesPlugin";
}

ZipCodesPlugin::~ZipCodesPlugin()
{
    qWarning() << "ZipCodesPlugin::~ZipCodesPlugin()";
}

bool ZipCodesPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "ZipCodesPlugin::initialize";

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    addAutoReleasedObject(new FrenchZipCodesPage(this));
    addAutoReleasedObject(new GenericZipCodesPage(this));

    return true;
}

void ZipCodesPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "ZipCodesPlugin::extensionsInitialized";
}

ExtensionSystem::IPlugin::ShutdownFlag ZipCodesPlugin::aboutToShutdown()
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

Q_EXPORT_PLUGIN(ZipCodesPlugin)
