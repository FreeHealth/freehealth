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
#include "drugsdbcore.h"
#include "atcpage.h"
#include "countries/moleculelinkerwidget.h"

#include "countries/frenchdrugsdatabasecreator.h"
#include "countries/fdadrugsdatabasecreator.h"
#include "countries/canadiandrugsdatabase.h"
#include "countries/belgishdrugsdatabase.h"
#include "countries/southafricandrugsdatabase.h"
//#include "countries/portuguesedrugsdatabase.h"

#include "ddi/drugdruginteractioncore.h"
#include "ddi/afssapsintegrator.h"
#include "ddi/interactioneditorpage.h"
#include "ddi/interactoreditorpage.h"

//#include "ddi/cytochromep450interactionspage.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <drugsdb/tools.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtPlugin>

#include <QDebug>

// TEST
#include <drugsbaseplugin/drugbaseessentials.h>
// END

using namespace DrugsDB;
using namespace Internal;

DrugsDbPlugin::DrugsDbPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Creating DrugsDbPlugin";
//    Core::ICore::instance()->translators()->addNewTranslator("drugsdbplugin");
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

    // create the core
    DrugsDBCore *core = new DrugsDBCore(this);
    core->initialize();

    // add database pages
    addAutoReleasedObject(new FreeFrenchDrugsDatabasePage(this));
    addAutoReleasedObject(new NonFreeFrenchDrugsDatabasePage(this));

    addAutoReleasedObject(new FreeFdaDrugsDatabasePage(this));
    addAutoReleasedObject(new NonFreeFdaDrugsDatabasePage(this));

    addAutoReleasedObject(new FreeCanadianDrugsDatabasePage(this));
    addAutoReleasedObject(new NonFreeCanadianDrugsDatabasePage(this));

    addAutoReleasedObject(new FreeBeDrugsDatabasePage(this));
    addAutoReleasedObject(new NonFreeBeDrugsDatabasePage(this));

    addAutoReleasedObject(new FreeSouthAfricanDrugsDatabasePage(this));
    addAutoReleasedObject(new NonFreeSouthAfricanDrugsDatabasePage(this));

//    addAutoReleasedObject(new PtDrugsDatabasePage(this));

    addAutoReleasedObject(new MoleculeLinkerPage(this));

//    addAutoReleasedObject(new AtcPage(this));

    // Create the core object
    IAMDb::DrugDrugInteractionCore::instance();

//    addAutoReleasedObject(new AfssapsIntegratorPage(this));
//    addAutoReleasedObject(new AfssapsClassTreePage(this));
    addAutoReleasedObject(new IAMDb::InteractionEditorPage(this));
    addAutoReleasedObject(new IAMDb::InteractorEditorPage(this));
//    addAutoReleasedObject(new CytochromeP450InteractionsPage(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    return true;
}

void DrugsDbPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsDbPlugin::extensionsInitialized";
}

Q_EXPORT_PLUGIN(DrugsDbPlugin)
