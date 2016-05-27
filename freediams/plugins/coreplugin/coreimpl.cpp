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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "coreimpl.h"

#include <coreplugin/settings_p.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/theme.h>
#include <coreplugin/translators.h>
#include <coreplugin/actionmanager/actionmanager_p.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/contextmanager/contextmanager_p.h>
#include <coreplugin/filemanager.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <coreplugin/commandlineparser.h>
#include <coreplugin/patient.h>
#include <coreplugin/user.h>
#include <coreplugin/ipadtools.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <medintuxutils/configuration.h>

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QTime>
#include <QSplashScreen>
#include <QFont>
#include <QWidget>
#include <QSplashScreen>
#include <QLibraryInfo>
#include <QTranslator>

namespace Core {
namespace Internal {
    static CoreImpl *m_instance = 0;
} // namespace Internal
} // namespace Core


using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

ICore* ICore::instance()
{
    return m_instance;
}

// instance is created by Core::CorePlugin()
CoreImpl::CoreImpl(QObject *parent) :
        ICore(parent),
        m_Splash(0),
        m_MainWindow(0),
        m_ActionManager(0),
        m_ContextManager(0),
        m_MedinTux(0),
        m_Patient(new Patient(this)),
        m_User(new Internal::User(this)),
        m_PadTools(0)

{
    QTime chrono;
    m_Settings = new SettingsPrivate(this);
    m_Settings->setPath(ISettings::UpdateUrl, Utils::Constants::FREEDIAMS_UPDATE_URL);

    m_Theme = new ThemePrivate(this);
    m_Theme->setThemeRootPath(m_Settings->path(ISettings::ThemeRootPath));

    m_CommandLine = new CommandLine();
    m_CommandLine->feedPatientDatas(m_Patient);

    bool logChrono = m_CommandLine->value(CommandLine::CL_Chrono).toBool();
    if (logChrono)
        chrono.start();

    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "Command line parsing");

    m_Theme->createSplashScreen(Constants::FREEDIAMS_SPLASHSCREEN);

    // add translators
    m_Theme->messageSplashScreen(tkTr(Trans::Constants::INITIALIZING_TRANSLATIONS));
    m_Translators = new Translators(this);
    m_Translators->setPathToTranslations(m_Settings->path(ISettings::TranslationsPath));
    // Qt
    m_Translators->addNewTranslator("qt");
    // Core Needed Libs
    m_Translators->addNewTranslator(Trans::Constants::CONSTANTS_TRANSLATOR_NAME);
    m_Translators->addNewTranslator("lib_utils");
    m_Translators->addNewTranslator("lib_medical");
    m_Translators->addNewTranslator("lib_medintux");
    m_Translators->addNewTranslator("plugin_fdcore");

    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "translators");

    m_Theme->messageSplashScreen(tkTr(Trans::Constants::STARTING_APPLICATION_AT_1).arg(QDateTime::currentDateTime().toString()));

    m_FileManager = new FileManager(this);
    m_UpdateChecker = new Utils::UpdateChecker(this);

    LOG_FOR("Core", tkTr(Trans::Constants::STARTING_APPLICATION_AT_1).arg(QDateTime::currentDateTime().toString()));

    // initialize the settings
    m_Theme->messageSplashScreen(tkTr(Trans::Constants::LOADING_SETTINGS));

    // WINE compatibility (only for testing under ubuntu when crosscompiling)
#ifdef Q_OS_WIN
    // For WINE testings
    if (m_CommandLine->value(Core::ICommandLine::RunningUnderWine).toBool()) {
        LOG_FOR( "Core", "Running under Wine environnement." );
        QFont::insertSubstitution("MS Shell Dlg", "Tahoma" );
        QFont::insertSubstitution("MS Shell Dlg 2", "Tahoma" );
    }
#endif

    foreach(const QString &l, QCoreApplication::libraryPaths()) {
        LOG_FOR("Core", tkTr(Trans::Constants::USING_LIBRARY_1).arg(l));
    }

//    m_FormManager = new FormManager(this);

    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "managers");

    if (Utils::isRunningOnMac())
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    // ready
    m_Theme->messageSplashScreen(QCoreApplication::translate("Core", "Core intialization finished..."));

    LOG_FOR("Core" , QCoreApplication::translate("Core", "Core intialization finished..."));
    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "end of core intialization");

    m_instance = this;
}

CoreImpl::~CoreImpl()
{
    delete m_CommandLine;
    delete m_MedinTux;
//    delete m_Patient;
}

ActionManager *CoreImpl::actionManager() const { return m_ActionManager; }
ContextManager *CoreImpl::contextManager() const { return m_ContextManager; }
ITheme *CoreImpl::theme() const { return m_Theme; }
Translators *CoreImpl::translators() const { return m_Translators; }
ISettings *CoreImpl::settings() const { return m_Settings; }
IMainWindow *CoreImpl::mainWindow() const { return m_MainWindow; }

void CoreImpl::setMainWindow(IMainWindow *win)
{
    // can be called only once
    Q_ASSERT(m_MainWindow==0);
    Q_ASSERT(m_ActionManager==0);
    Q_ASSERT(m_ContextManager==0);
    m_MainWindow = win;
    m_ActionManager = new ActionManagerPrivate(m_MainWindow);
    m_ContextManager = new ContextManagerPrivate(m_MainWindow);
}

//FormManager *CoreImpl::formManager() const { return m_FormManager; }
FileManager *CoreImpl::fileManager() const { return m_FileManager; }

ICommandLine *CoreImpl::commandLine() const { return m_CommandLine; }
Utils::UpdateChecker *CoreImpl::updateChecker() const { return m_UpdateChecker; }

MedinTux::Configuration *CoreImpl::medintuxConfiguration() const
{
    if (!m_MedinTux)
        m_MedinTux = MedinTux::Configuration::instance();  // need to be deleted
    return m_MedinTux;
}

IPatient *CoreImpl::patient() const {return m_Patient;}

bool CoreImpl::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    // first time runnning ?
    QString msg;
    if (m_Settings->firstTimeRunning()) {
        msg = QCoreApplication::translate("Core", "You are running FreeDiams for the first time. You need to approve the licence terms.");
    } else if (m_Settings->licenseApprovedApplicationNumber() != qApp->applicationVersion()) {
        msg = QCoreApplication::translate("Core", "You are running a new version of FreeDiams, you need to renew the licence agreement.");
    }

    if (!msg.isEmpty()) {
#ifndef LINUX_INTEGRATED
        if (!Utils::defaultLicenseAgreementDialog(msg, Utils::LicenseTerms::GPLv3))
            return false;
#endif
        // update some preferences ?
        bool yes = Utils::yesNoMessageBox(
                QCoreApplication::translate("Core", "You are updating FreeDiams, do you want to update your personnal preferences ?"),
                QCoreApplication::translate("Core", "With the new version some preferences should be outdated. Answering yes will allow FreeDiams to update your personnal preferences."));
        if (yes) {
            m_UpdatePreferences = true;
        }
        m_Settings->noMoreFirstTimeRunning();
        m_Settings->setLicenseApprovedApplicationNumber(qApp->applicationVersion());
    }
    return true;
}

void CoreImpl::extensionsInitialized()
{
    Q_EMIT coreOpened();
}

