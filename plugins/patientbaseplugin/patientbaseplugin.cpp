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
#include "patientcore.h"
#include "patientbase.h"
#include "patientwidgetmanager.h"
#include "patientsearchmode.h"
#include "patientbasepreferencespage.h"
#include "patientmodel.h"
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

#include <QtPlugin>
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

static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }

PatientBasePlugin::PatientBasePlugin() :
    m_Mode(0),
    prefpage(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PatientBasePlugin";

    // Add Translators
    Core::ICore::instance()->translators()->addNewTranslator("plugin_patientbase");

    // add preference page
    prefpage = new PatientBasePreferencesPage(this);
    addObject(prefpage);

    // create the core
    new PatientCore(this);

//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
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

    // Initialize Core
    if (!patientCore()->initialize())
        return false;

    // Create default virtual patients?
    if (commandLine()->value(Core::ICommandLine::CreateVirtuals).toBool()) {
        if (!patientCore()->createDefaultVirtualPatients())
            LOG_ERROR("Unable to create default virtual patients");
    }

    // Add the Photo providers (file & url)
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

    // Check settings
    prefpage->checkSettingsValidity();
    settings()->sync();

    // add mode patient search
    m_Mode = new PatientSearchMode(this);
    m_Mode->postCoreInitialization();
    addObject(m_Mode);

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

#if QT_VERSION >= 0x050000
Q_DECLARE_INTERFACE(Patients::Internal::PatientBasePlugin, "org.freemedforms.FreeMedForms.PatientBasePlugin")
#else
Q_EXPORT_PLUGIN(PatientBasePlugin)
#endif
