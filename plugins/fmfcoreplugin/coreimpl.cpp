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
#include <coreplugin/constants_icons.h>

#include <fmfcoreplugin/commandlineparser.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

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

    m_Theme->createSplashScreen(Constants::FREEMEDFORMS_SPLASHSCREEN);
    m_Settings->setPath(ISettings::Splashscreen, Constants::FREEMEDFORMS_SPLASHSCREEN);

    // add translators
    m_Theme->messageSplashScreen(tkTr(Trans::Constants::INITIALIZING_TRANSLATIONS));
    m_Translators = new Translators(this);
    m_Translators->setPathToTranslations(m_Settings->path(ISettings::TranslationsPath));
    // Qt
    m_Translators->addNewTranslator("qt");
    // Core Needed Libs
    m_Translators->addNewTranslator(Trans::Constants::CONSTANTS_TRANSLATOR_NAME);
    m_Translators->addNewTranslator("utils");
    m_Translators->addNewTranslator("medicalutils");
    m_Translators->addNewTranslator("fmfcoreplugin");

    if (logChrono)
        Utils::Log::logTimeElapsed(chrono, "Core", "translators");

    m_Theme->messageSplashScreen(tkTr(Trans::Constants::STARTING_APPLICATION_AT_1).arg(QDateTime::currentDateTime().toString()));
    Utils::Log::addMessage( "Core" , tkTr(Trans::Constants::STARTING_APPLICATION_AT_1).arg( QDateTime::currentDateTime().toString() ) );

    foreach(const QString &l, QCoreApplication::libraryPaths()) {
        Utils::Log::addMessage("Core" , tkTr(Trans::Constants::USING_LIBRARY_1).arg(l));
    }

    // initialize the settings
    m_Theme->messageSplashScreen(tkTr(Trans::Constants::LOADING_SETTINGS));

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
    m_Theme->messageSplashScreen(QCoreApplication::translate("Core", "Core intialization finished..."));

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
        if (!Utils::defaultLicenceAgreementDialog("", Utils::LicenseTerms::GPLv3 ))
            return false;
#endif
        m_Settings->setLicenseApprovedApplicationNumber(qApp->applicationVersion());
    } else if (m_Settings->licenseApprovedApplicationNumber() != qApp->applicationVersion()) {
        // show the license agreement dialog
#ifndef LINUX_INTEGRATED
        if (!Utils::defaultLicenceAgreementDialog(
                QCoreApplication::translate("Core", "You are running a new version of FreeDiams, you need to renew the licence agreement."),
                Utils::LicenseTerms::GPLv3 ))
            return false;
#endif
        m_Settings->setLicenseApprovedApplicationNumber(qApp->applicationVersion());
    }

    return true;
}

void CoreImpl::extensionsInitialized()
{
    Utils::Log::addMessage(this, "Core Opened");
    Q_EMIT coreOpened();
}

