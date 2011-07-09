/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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

/**
  \namespace DrugsDB
  \brief Namespace reserved for the DrugsDB plugin.
  The DrugsDB namespace includes:
  - All database accesses
  - All constants related to drugs
  - Interaction engine
  - Save/Load abilities (XML prescription files)
  - Print abilities
*/

#include "drugsbaseplugin.h"
#include "drugstemplateprinter.h"
#include "drugsbase.h"
#include "drugsdatabaseselector.h"
#include "drugdruginteractionengine.h"
#include "pimengine.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>

using namespace DrugsDB;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

DrugsBasePlugin::DrugsBasePlugin() :
        IPlugin(), m_DDIEngine(0)
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
    removeObject(m_DDIEngine);
    if (m_DDIEngine)
        delete m_DDIEngine;
    m_DDIEngine = 0;
    removeObject(m_PimEngine);
    if (m_PimEngine)
        delete m_PimEngine;
    m_PimEngine = 0;
}

bool DrugsBasePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    messageSplash(tr("Initializing drugs database plugin..."));

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("drugsbaseplugin");

    return true;
}

void DrugsBasePlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::extensionsInitialized";

    // initialize DrugsBase
    Internal::DrugsBase::instance();

    // Initialize the drugs engines
    m_DDIEngine = new DrugsDB::Internal::DrugDrugInteractionEngine(this);
    m_DDIEngine->init();
    addObject(m_DDIEngine);
    m_PimEngine = new DrugsDB::Internal::PimEngine(this);
    m_PimEngine->init();
    addObject(m_PimEngine);

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    addAutoReleasedObject(new DrugsDB::Internal::DrugsTemplatePrinter(this));
}


Q_EXPORT_PLUGIN(DrugsBasePlugin)
