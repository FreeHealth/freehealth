/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
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
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/formmanager.h>
#include <coreplugin/filemanager.h>

#include <fdcoreplugin/commandlineparser.h>
#include <fdcoreplugin/patient.h>

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

namespace Core {
namespace Internal {
    static CoreImpl *m_instance = 0;
} // namespace Internal
} // namespace Core


using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

ICore* ICore::instance()
{
    return m_instance;
}

inline static void showMessage( QSplashScreen* s, const QString& m )
{
    s->showMessage( m, Qt::AlignRight | Qt::AlignBottom, Qt::black );
}

// instance is created by Core::CorePlugin()
CoreImpl::CoreImpl(QObject *parent) :
        ICore(parent),
        m_MainWindow(0),
        m_ActionManager(0),
        m_ContextManager(0),
        m_MedinTux(0)
{
    m_FileManager = new FileManager(this);
    m_UpdateChecker = new Utils::UpdateChecker(this);
    m_Patient = new Patient();
    m_UID = new UniqueIDManager();
    m_Settings = new SettingsPrivate(this);
    m_Theme = new ThemePrivate(this);
    m_Theme->setThemeRootPath(m_Settings->path(ISettings::ThemeRootPath));

    Utils::Log::addMessage( "Core" , tkTr(Trans::Constants::STARTING_APPLICATION_AT_1).arg( QDateTime::currentDateTime().toString() ) );
    QSplashScreen splash( m_Theme->splashScreen(Constants::FREEDIAMS_SPLASHSCREEN) );
    splash.show();
    QFont ft( splash.font() );
    ft.setPointSize( ft.pointSize() - 2 );
    ft.setBold( true );
    splash.setFont( ft );
    splash.show();

    // initialize the settings
    showMessage( &splash, tkTr(Trans::Constants::GETTING_SETTINGS_FILE));

    QTime chrono;
    chrono.start();

    m_CommandLine = new CommandLine();
    bool logChrono = m_CommandLine->value(CommandLine::CL_Chrono).toBool();
    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "command line parsing");

    // WINE compatibility (only for testing under ubuntu when crosscompiling)
#ifdef Q_OS_WIN
    // For WINE testings
    if (m_CommandLine->value(CommandLine::CL_Wine).toBool()) {
        Utils::Log::addMessage( "Core", "Running under Wine environnement." );
        QFont::insertSubstitution("MS Shell Dlg", "Tahoma" );
        QFont::insertSubstitution("MS Shell Dlg 2", "Tahoma" );
    }
#endif

    // Feed patient datas
    m_Patient->setValue(Patient::DateOfBirth, m_CommandLine->value(CommandLine::CL_DateOfBirth));
    m_Patient->setValue(Patient::CreatinClearance, m_CommandLine->value(CommandLine::CL_ClCr));
    m_Patient->setValue(Patient::Size, m_CommandLine->value(CommandLine::CL_Size));
    m_Patient->setValue(Patient::Weight, m_CommandLine->value(CommandLine::CL_Weight));
    m_Patient->setValue(Patient::Creatinin, m_CommandLine->value(CommandLine::CL_Creatinin));
    m_Patient->setValue(Patient::FullName, m_CommandLine->value(CommandLine::CL_PatientName));
//    m_Patient->setValue(Patient::DrugsAllergies, m_CommandLine->value(CommandLine::));
//    m_Patient->setValue(Patient::ICD10Deceases, m_CommandLine->value(CommandLine::CL_DateOfBirth));

    // Set application libraries
    if (!Utils::isDebugCompilation()) {
        QApplication::setLibraryPaths( QStringList() << settings()->path(ISettings::QtPlugInsPath) );
    }
    foreach(const QString &l, QCoreApplication::libraryPaths() ) {
        Utils::Log::addMessage("Core" , tkTr(Trans::Constants::USING_LIBRARY_1).arg(l));
    }

    m_FormManager = new FormManager(this);

    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "managers");

    // add translators
    showMessage( &splash, tkTr(Trans::Constants::INITIALIZING_TRANSLATIONS) );
    m_Translators = new Translators(this);
    m_Translators->setPathToTranslations(m_Settings->path(ISettings::TranslationsPath));
    // Qt
    m_Translators->addNewTranslator("qt");
    // Core Needed Libs
    m_Translators->addNewTranslator(Trans::Constants::CONSTANTS_TRANSLATOR_NAME);

    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "translators");

    // Manage exchange file
    showMessage( &splash, QCoreApplication::translate( "Core", "Checking command line parameters..." ) );
//    if (!medintuxConfiguration()->isMedinTuxPlugIns(&splash)) {
//        if (!m_CommandLine->value(CommandLine::CL_Chrono).toString().isEmpty()) {
//            showMessage( &splash, QCoreApplication::translate( "Core", "Reading exchange file..." ) );
//            QString extras;
//            DrugsIO::loadPrescription(m_CommandLine->value(CommandLine::CL_Chrono).toString(), extras);
//            /** \too patient feeding */
////            patient()->fromXml(extras);
//        }
//    }

    if (Utils::isRunningOnMac())
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    // start update checking
    showMessage( &splash, tkTr(Trans::Constants::CHECKING_UPDATES) );
//    QObject::connect(updateChecker(), SIGNAL(updateFound()), mainWindow(), SLOT(updateFound()));
//    updateChecker()->check(FREEDIAMS_UPDATE_URL);
//    if (diCorePrivate::m_Chrono)
//        tkLog::logTimeElapsed(chrono, "diCore", tkTr(CHECKING_UPDATES));

//    if (medintuxConfiguration()->isMedinTuxPlugIns( &splash )) {
//        showMessage( &splash, tkTr(Trans::Constants::RAISING_APPLICATION) );
//        tkLog::addMessage( "Core" , tkTr(Trans::Constants::RAISING_APPLICATION) );
//        qApp->setActiveWindow( mainWindow() );
//        mainWindow()->activateWindow();
//        mainWindow()->raise();
//        if (logChrono)
//            Utils::Log::logTimeElapsed(chrono, "Core", "medintux plugins preparation");
//    }

    // ready
    showMessage(&splash, QCoreApplication::translate("Core", "%1 v%2 Ready !").arg(qApp->applicationName(), qApp->applicationVersion()));
    // finish splashscreen
    splash.finish(mainWindow());

    Utils::Log::addMessage("Core" , QCoreApplication::translate("Core", "Core intialization finished..."));
    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "end of core intialization");

    m_instance = this;
}

CoreImpl::~CoreImpl()
{
    delete m_UID;
    delete m_CommandLine;
    delete m_MedinTux;
    delete m_Patient;
}

ActionManager *CoreImpl::actionManager() const
{
    return m_ActionManager;
}

ContextManager *CoreImpl::contextManager() const
{
    return m_ContextManager;
}

UniqueIDManager *CoreImpl::uniqueIDManager() const
{
    return m_UID;
}

ITheme *CoreImpl::theme() const
{
    return m_Theme;
}

Translators *CoreImpl::translators() const { return m_Translators; }

ISettings *CoreImpl::settings() const { return m_Settings; }


IMainWindow *CoreImpl::mainWindow() const { return m_MainWindow; }

void CoreImpl::setMainWindow(IMainWindow *win)
{
    Q_ASSERT(m_MainWindow==0);
    Q_ASSERT(m_ActionManager==0);
    Q_ASSERT(m_ContextManager==0);
    m_MainWindow = win;
    m_ActionManager = new ActionManagerPrivate(m_MainWindow);
    m_ContextManager = new ContextManagerPrivate(m_MainWindow);
}

FormManager *CoreImpl::formManager() const { return m_FormManager; }
FileManager *CoreImpl::fileManager() const { return m_FileManager; }

CommandLine *CoreImpl::commandLine() const { return m_CommandLine; }
Utils::UpdateChecker *CoreImpl::updateChecker() const { return m_UpdateChecker; }

MedinTux::Configuration *CoreImpl::medintuxConfiguration() const
{
    if (!m_MedinTux)
        m_MedinTux = MedinTux::Configuration::instance();  // need to be deleted
    return m_MedinTux;
}

Patient *CoreImpl::patient() const { return m_Patient; }

bool CoreImpl::initialize(const QStringList &arguments, QString *errorString)
{
    // first time runnning ?
    if (m_Settings->firstTimeRunning()) {
        // show the license agreement dialog
        if (!Utils::defaultLicenceAgreementDialog("", Utils::LicenseTerms::BSD ))
            return false;
        m_Settings->noMoreFirstTimeRunning();
        m_Settings->setLicenseApprovedApplicationNumber( qApp->applicationVersion());
//        DrugsPreferences::writeDefaultSettings(settings());
    } else if (m_Settings->licenseApprovedApplicationNumber() != qApp->applicationVersion()) {
        // show the license agreement dialog
        if (!Utils::defaultLicenceAgreementDialog(
                QCoreApplication::translate("Core", "You are running a new version of FreeDiams, you need to renew the licence agreement."),
                Utils::LicenseTerms::BSD ))
            return false;
        m_Settings->setLicenseApprovedApplicationNumber( qApp->applicationVersion());
    }

    return true;
}

void CoreImpl::extensionsInitialized()
{
}

