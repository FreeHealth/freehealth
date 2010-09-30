/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "drugsbaseplugin.h"
#include "drugstemplateprinter.h"
#include "drugsbase.h"
#include "drugsdatabaseselector.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>

using namespace DrugsDB;

DrugsBasePlugin::DrugsBasePlugin()
{
    if (Utils::Log::warnPluginsCreation()) {
#ifdef FREEDIAMS
        qWarning() << "creating FREEDIAMS::DrugsBasePlugin";
#else
        qWarning() << "creating FREEMEDFORMS::DrugsBasePlugin";
#endif
    }
}

DrugsBasePlugin::~DrugsBasePlugin()
{
}

bool DrugsBasePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

//    DrugsDatabaseSelector::instance();
//    DrugsDatabaseSelector::instance()->getAllDatabaseInformations();

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("drugsbaseplugin");

    // initialize DrugsBase
    Internal::DrugsBase::instance();

    return true;
}

void DrugsBasePlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    addAutoReleasedObject(new DrugsDB::Internal::DrugsTemplatePrinter(this));
}


Q_EXPORT_PLUGIN(DrugsBasePlugin)
