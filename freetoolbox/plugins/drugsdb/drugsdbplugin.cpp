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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugsdbplugin.h"
#include "atcpage.h"
#include "canadiandrugsdatabase.h"
#include "frenchdrugsdatabasecreator.h"
#include "fdadrugsdatabasecreator.h"
#include "southafricandrugsdatabase.h"
#include "belgishdrugsdatabase.h"
#include "portuguesedrugsdatabase.h"
#include "moleculelinkerwidget.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/globaltools.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>

#include <QDebug>

// TEST
#include <drugsbaseplugin/drugbaseessentials.h>
// END

using namespace DrugsDbCreator::Internal;

DrugsDbPlugin::DrugsDbPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Creating DrugsDbPlugin";
}

DrugsDbPlugin::~DrugsDbPlugin()
{
    qWarning() << "DrugsDbPlugin::~DrugsDbPlugin()";
}

bool DrugsDbPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsDbPlugin::initialize";

    //    Core::ICore::instance()->translators()->addNewTranslator("freeicd-drugsdbplugin");

    addAutoReleasedObject(new CanadianDrugsDatabasePage(this));
    addAutoReleasedObject(new FdaDrugsDatabasePage(this));
    addAutoReleasedObject(new FrenchDrugsDatabasePage(this));
    addAutoReleasedObject(new SouthAfricanDrugsDatabasePage(this));
//    addAutoReleasedObject(new BeDrugsDatabasePage(this));
//    addAutoReleasedObject(new PtDrugsDatabasePage(this));

//    addAutoReleasedObject(new MoleculeLinkerPage(this));

//    addAutoReleasedObject(new AtcPage(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    return true;
}

void DrugsDbPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsDbPlugin::extensionsInitialized";

//    DrugsDB::Internal::drugbaseessentials c;
//    c.initialize(Core::Tools::drugsDatabaseAbsFileName(), true);
}


Q_EXPORT_PLUGIN(DrugsDbPlugin)
