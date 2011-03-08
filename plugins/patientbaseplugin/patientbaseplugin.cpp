/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \namespace Patients
  \brief Namespace reserved for the Patients plugin.
  The Patients namespace includes:
  - All database accesses to the Patients database
  - A Patient creator wizard
  - A patient selector
  - The patient mode and bar
*/

#include "patientbaseplugin.h"
#include "patientbase.h"
#include "patientwidgetmanager.h"
#include "patientsearchmode.h"
#include "patientbasepreferencespage.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/modemanager/modemanager.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>


using namespace Patients;
using namespace Internal;
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }


PatientBasePlugin::PatientBasePlugin() :
        m_Mode(0), prefpage(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PatientBasePlugin";
}

PatientBasePlugin::~PatientBasePlugin()
{
    qWarning() << "PatientBasePlugin::~PatientBasePlugin()";
    if (m_Mode) {
        removeObject(m_Mode);
        delete m_Mode;
        m_Mode = 0;
    }
    if (prefpage) {
        removeObject(prefpage);
        delete prefpage;
        prefpage = 0;
    }
}

bool PatientBasePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PatientBasePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    messageSplash(tr("Initializing patients database plugin..."));

    // Add Translators
    Core::ICore::instance()->translators()->addNewTranslator("patientbaseplugin");

    // add preference page
    prefpage = new PatientBasePreferencesPage(this);
    addObject(prefpage);
    prefpage->checkSettingsValidity();

    // Initialize patient base
    patientBase();
    if (!patientBase()->isInitialized())
        return false;

    // create patient widget manager instance
    PatientWidgetManager::instance();

    // add mode patient search
    m_Mode = new PatientSearchMode(this);
    addObject(m_Mode);

    return true;
}

void PatientBasePlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PatientBasePlugin::extensionsInitialized";

}


Q_EXPORT_PLUGIN(PatientBasePlugin)
