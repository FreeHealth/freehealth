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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \namespace Patients
  \brief Namespace reserved for the Patients plugin.
  The Patients namespace includes:
  - All database accesses to the Patients database
  - A Patient creator wizard
  - A patient selector
  - The patient mode and bar

  The patient plugin is not dependent of UserManagerPlugin suring build time, but it is dependent of it
  at application start up. So dependence is notified in the pluginspec only.
*/

#include "patientbaseplugin.h"
#include "patientbase.h"
#include "patientwidgetmanager.h"
#include "patientsearchmode.h"
#include "patientbasepreferencespage.h"
#include "patientmodel.h"
#include "patientmodelwrapper.h"
#include "filephotoprovider.h"
#include "urlphotoprovider.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/icommandline.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDir>
#include <QProgressDialog>

#include <QDebug>


using namespace Patients;
using namespace Internal;
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ICommandLine *commandLine() {return Core::ICore::instance()->commandLine();}

static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }

static inline Patients::PatientModel *patientModel() {return Patients::PatientModel::activeModel();}


PatientBasePlugin::PatientBasePlugin() :
    m_Mode(0), prefpage(0),
    m_PatientModelWrapper(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PatientBasePlugin";

    // Add Translators
    Core::ICore::instance()->translators()->addNewTranslator("patientbaseplugin");

    // add preference page
    prefpage = new PatientBasePreferencesPage(this);
    addObject(prefpage);

    // create the base
    new Internal::PatientBase(this);

    // Create IPatient
    m_PatientModelWrapper = new Internal::PatientModelWrapper(this);
    Core::ICore::instance()->setPatient(m_PatientModelWrapper);
}

PatientBasePlugin::~PatientBasePlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool PatientBasePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PatientBasePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // This part of the code is executed AFTER the UserManagerPlugin::initialize()

    messageSplash(tr("Initializing patients database plugin..."));

    if (!user())
        return false;
    if (user()->uuid().isEmpty())
        return false;

    // Initialize patient base
    QProgressDialog dlg(tr("Initializing patient database..."), tr("Please wait"), 0, 0);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setMinimumDuration(1000);
    dlg.show();
    dlg.setFocus();
    dlg.setValue(0);

    if (!patientBase()->initialize())
        return false;

    // create singleton model
    PatientModel *model = new PatientModel(this);
    PatientModel::setActiveModel(model);
    m_PatientModelWrapper->initialize(model);

    if (commandLine()->value(Core::ICommandLine::CreateVirtuals).toBool()) {
        // Populate with some virtual patients
        QString path = settings()->path(Core::ISettings::BigPixmapPath) + QDir::separator();
        int userLkId = 1; //userModel()->practionnerLkIds(userModel()->currentUserData(Core::IUser::Uuid).toString()).at(0);

        QString uid = "b04936fafccb4174a7a6af25dd2bb71c";
        patientBase()->createVirtualPatient("KIRK", "", "James Tiberius", "M", 6, QDate(1968, 04, 20), "US", "USS Enterprise",
                      "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainkirk.jpg");

        uid = "2c49299b9b554300b46a6e3ef6d40a65";
        patientBase()->createVirtualPatient("PICARD", "", "Jean-Luc", "M", 6, QDate(1948, 04, 20), "US", "USS Enterprise-D",
                      "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainpicard.png");

        uid = "ef97f37361824b6f826d5c9246f9dc49";
        patientBase()->createVirtualPatient("ARCHER", "", "Jonathan", "M", 6, QDate(1928, 04, 20), "US", "Enterprise (NX-01) commanding officer",
                      "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainarcher.jpg");

        uid = "493aa06a1b8745b2ae6c79c531ef12a0";
        patientBase()->createVirtualPatient("JANEWAY", "", "Kathryn", "F", 6, QDate(1938, 04, 20), "US", "USS Voyager",
                      "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainjaneway.jpg");
    }

    // create patient widget manager instance
    PatientWidgetManager::instance();

    FilePhotoProvider *filePhotoProvider = new FilePhotoProvider(this);
    addAutoReleasedObject(filePhotoProvider);

    UrlPhotoProvider *urlPhotoProvider = new UrlPhotoProvider(this);
    addAutoReleasedObject(urlPhotoProvider);
    return true;
}

void PatientBasePlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PatientBasePlugin::extensionsInitialized";

    messageSplash(tr("Initializing patients database plugin..."));

    prefpage->checkSettingsValidity();
    settings()->sync();

    // add mode patient search
    m_Mode = new PatientSearchMode(this);
    m_Mode->postCoreInitialization();
    addObject(m_Mode);

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void PatientBasePlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;

    PatientModel::activeModel()->refreshModel();
    PatientWidgetManager::instance()->postCoreInitialization();
}

ExtensionSystem::IPlugin::ShutdownFlag PatientBasePlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
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
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(PatientBasePlugin)
