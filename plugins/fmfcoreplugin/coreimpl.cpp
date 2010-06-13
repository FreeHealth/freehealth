/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include <coreplugin/filemanager.h>
#include <coreplugin/modemanager/modemanager.h>

#include <fmfcoreplugin/commandlineparser.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QSplashScreen>

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

// instance is created by Core::CorePlugin()
CoreImpl::CoreImpl(QObject *parent) :
        ICore(parent),
        m_Splash(0),
        m_MainWindow(0),
        m_ActionManager(0),
        m_ContextManager(0),
        m_ModeManager(0),
        m_Patient(0),
        m_User(0)
{
    setObjectName("Core");
    m_Settings = new SettingsPrivate(this);
    m_Settings->setPath(ISettings::UpdateUrl, Utils::Constants::FREEMEDFORMS_UPDATE_URL);

    m_Theme = new ThemePrivate(this);
    m_Theme->setThemeRootPath(m_Settings->path(ISettings::ThemeRootPath));
    m_CommandLine = new CommandLine();

    QTime chrono;
    chrono.start();
    bool logChrono = m_CommandLine->value(CommandLine::CL_Chrono).toBool();
    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "command line parsing");

    createSplashScreen(m_Theme->splashScreen(Constants::FREEMEDFORMS_SPLASHSCREEN));

    // add translators
    messageSplashScreen(tkTr(Trans::Constants::INITIALIZING_TRANSLATIONS));
    m_Translators = new Translators(this);
    m_Translators->setPathToTranslations(m_Settings->path(ISettings::TranslationsPath));
    // Qt
    m_Translators->addNewTranslator("qt");
    // Core Needed Libs
    m_Translators->addNewTranslator(Trans::Constants::CONSTANTS_TRANSLATOR_NAME);
    m_Translators->addNewTranslator("utils");
    m_Translators->addNewTranslator("medicalutils");
    m_Translators->addNewTranslator("fmf    coreplugin");

    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "translators");

    messageSplashScreen(tkTr(Trans::Constants::STARTING_APPLICATION_AT_1).arg(QDateTime::currentDateTime().toString()));
    Utils::Log::addMessage( "Core" , tkTr(Trans::Constants::STARTING_APPLICATION_AT_1).arg( QDateTime::currentDateTime().toString() ) );

    foreach(const QString &l, QCoreApplication::libraryPaths()) {
        Utils::Log::addMessage("Core" , tkTr(Trans::Constants::USING_LIBRARY_1).arg(l));
    }

    // initialize the settings
    messageSplashScreen(tkTr(Trans::Constants::LOADING_SETTINGS));

    // WINE compatibility (only for testing under ubuntu when crosscompiling)
#ifdef Q_OS_WIN
    // For WINE testings
    if (m_CommandLine->value(Core::CommandLine::CL_RunningUnderWine).toBool()) {
        Utils::Log::addMessage( "Core", "Running under Wine environnement." );
        QFont::insertSubstitution("MS Shell Dlg", "Tahoma" );
        QFont::insertSubstitution("MS Shell Dlg 2", "Tahoma" );
    }
#endif

//    m_MainWindow = new MainWindow();
//    m_ActionManager = new ActionManagerPrivate(m_MainWindow);
//    m_ContextManager = new ContextManagerPrivate(m_MainWindow);
    m_FileManager = new FileManager(this);
    m_UpdateChecker = new Utils::UpdateChecker(this);
    m_UID = new UniqueIDManager();
    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "managers");

    if (Utils::isRunningOnMac())
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    // ready
    messageSplashScreen(QCoreApplication::translate("Core", "Core intialization finished..."));

    Utils::Log::addMessage("Core" , QCoreApplication::translate("Core", "Core intialization finished..."));
    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "end of core intialization");

    m_instance = this;
}

CoreImpl::~CoreImpl()
{
    Q_EMIT coreAboutToClose();
    if (m_UID)
        delete m_UID;
    if (m_CommandLine)
        delete m_CommandLine;
    if (m_ModeManager)
        delete m_ModeManager;
    if (m_MainWindow)
        delete m_MainWindow;
}

void CoreImpl::createSplashScreen(const QPixmap &pix)
{
    if (!m_Splash) {
        m_Splash = new QSplashScreen(pix);
        QFont ft(m_Splash->font());
        ft.setPointSize(ft.pointSize() - 2);
        ft.setBold(true);
        m_Splash->setFont(ft);
        m_Splash->show();
    }
}

void CoreImpl::finishSplashScreen(QWidget *w)
{
    Q_ASSERT(m_Splash);
    if (m_Splash) {
        m_Splash->finish(w);
        delete m_Splash;
        m_Splash = 0;
    }
}

void CoreImpl::messageSplashScreen(const QString &msg)
{
    Q_ASSERT(m_Splash);
    if (m_Splash)
        m_Splash->showMessage( msg, Qt::AlignLeft | Qt::AlignBottom, Qt::black );
}

IMainWindow *CoreImpl::mainWindow() const  { return m_MainWindow; }
void CoreImpl::setMainWindow(IMainWindow *win)
{
    // can be called only once
    Q_ASSERT(m_MainWindow==0);
    Q_ASSERT(m_ActionManager==0);
    Q_ASSERT(m_ContextManager==0);
    Q_ASSERT(m_ModeManager==0);
    m_MainWindow = win;
    m_ModeManager = new ModeManager(m_MainWindow);
    m_ContextManager = new ContextManagerPrivate(m_MainWindow);
    m_ActionManager = new ActionManagerPrivate(m_MainWindow);
}

QSplashScreen *CoreImpl::splashScreen()  { return m_Splash;}
ActionManager *CoreImpl::actionManager() const { return m_ActionManager; }
ContextManager *CoreImpl::contextManager() const { return m_ContextManager; }
UniqueIDManager *CoreImpl::uniqueIDManager() const { return m_UID; }
ITheme *CoreImpl::theme() const { return m_Theme; }
Translators *CoreImpl::translators() const { return m_Translators; }
ISettings *CoreImpl::settings() const{ return m_Settings; }
FileManager *CoreImpl::fileManager() const { return m_FileManager; }
Utils::UpdateChecker *CoreImpl::updateChecker() const { return m_UpdateChecker; }
CommandLine *CoreImpl::commandLine() const { return m_CommandLine; }
ModeManager *CoreImpl::modeManager() const {return m_ModeManager;}

bool CoreImpl::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // first time runnning ?
    if (m_Settings->firstTimeRunning()) {
        // show the license agreement dialog
#ifndef LINUX_INTEGRATED
        if (!Utils::defaultLicenceAgreementDialog("", Utils::LicenseTerms::BSD ))
            return false;
#endif
        m_Settings->setLicenseApprovedApplicationNumber(qApp->applicationVersion());
    } else if (m_Settings->licenseApprovedApplicationNumber() != qApp->applicationVersion()) {
        // show the license agreement dialog
#ifndef LINUX_INTEGRATED
        if (!Utils::defaultLicenceAgreementDialog(
                QCoreApplication::translate("Core", "You are running a new version of FreeDiams, you need to renew the licence agreement."),
                Utils::LicenseTerms::BSD ))
            return false;
#endif
        m_Settings->setLicenseApprovedApplicationNumber(qApp->applicationVersion());
    }

    return true;
}

void CoreImpl::extensionsInitialized()
{
    if (m_Splash)
        finishSplashScreen(0);
    Utils::Log::addMessage(this, "Core Opened");
    Q_EMIT coreOpened();
}

