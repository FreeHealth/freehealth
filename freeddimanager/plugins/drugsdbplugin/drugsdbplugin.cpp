/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugsdbplugin.h"
#include "imode/drugsdbmode.h"
#include "idrugdatabase.h"
#include <drugsdbplugin/countries/fr/frenchdrugsdatabasecreator.h>
#include <drugsdbplugin/countries/za/southafricandrugsdatabase.h>
#include <drugsdbplugin/countries/ca/canadiandrugsdatabase.h>
#include <drugsdbplugin/countries/us/fdadrugsdatabase.h>

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtPlugin>
#include <QAction>
#include <QDebug>

using namespace DrugsDb;
using namespace Internal;

/**
 * The core IPlugin object manages:
 * - the IMode object
 * - all the IDrugDatabase objects, register them to the IMode and manages their life-cycle
 */
DrugsDbPlugin::DrugsDbPlugin() :
    _mode(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "Creating DrugsDbPlugin";
    Core::ICore::instance()->translators()->addNewTranslator("plugin_drugsdb");
}

DrugsDbPlugin::~DrugsDbPlugin()
{
    qWarning() << "DrugsDbPlugin::~DrugsDbPlugin()";
    delete _mode;
    _mode = 0;
}

bool DrugsDbPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DrugsDbPlugin::initialize";

    // Create the mode
    _mode = new DrugsDbMode(this);
    addObject(_mode);

    // Create the database creator
    IDrugDatabase *base = 0;

    // French drugs databases (Free & NonFree)
    base = new FrDrugDatabase(this);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    base = new FrDrugDatabase(this);
    base->setLicenseType(IDrugDatabase::NonFree);
    base->setServerOwner(IDrugDatabase::FrenchAssociation);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    // South-African drugs databases (Free & NonFree)
    base = new ZaDrugDatabase(this);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    base = new ZaDrugDatabase(this);
    base->setLicenseType(IDrugDatabase::NonFree);
    base->setServerOwner(IDrugDatabase::FrenchAssociation);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    // Canadian drugs databases (Free & NonFree)
    base = new CaDrugDatabase(this);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    base = new CaDrugDatabase(this);
    base->setLicenseType(IDrugDatabase::NonFree);
    base->setServerOwner(IDrugDatabase::FrenchAssociation);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    // USA drugs databases (Free & NonFree)
    base = new FdaDrugDatabase(this);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    base = new FdaDrugDatabase(this);
    base->setLicenseType(IDrugDatabase::NonFree);
    base->setServerOwner(IDrugDatabase::FrenchAssociation);
    _mode->registerDrugDatabase(base);
    _databases.append(base);

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    return true;
}

void DrugsDbPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "DrugsDbPlugin::extensionsInitialized";
}

ExtensionSystem::IPlugin::ShutdownFlag DrugsDbPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Here you still have a full access to
    //   Core::ICore::instance()
    // And all its objects (user(), patient(), settings(), theme()...).
    removeObject(_mode);

    // Delete all drug databases
    qDeleteAll(_databases);

    return SynchronousShutdown;
}


Q_EXPORT_PLUGIN(DrugsDbPlugin)
