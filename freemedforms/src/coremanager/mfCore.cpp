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
#include <mfCore.h>

// including freemedforms headers
#include <mfGlobal.h>
#include <mfSettings.h>
#include <mfMainWindow.h>
#include <mfMainWidget.h>
#include <mfScriptor.h>
#include <mfPluginsManager.h>
#include <mfCrashRecovererDialog.h>
//#include <mfUpdateChecker.h>
#include <mfConstants.h>

// including toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkTheme.h>
#include <tkTranslators.h>
#include <tkActionManager.h>
#include <tkContextManager.h>
#include <tkConstantTranslations.h>
#include <tkConstants.h>
#include <tkUpdateChecker.h>

// include usertoolkit headers
#include <tkUser.h>
#include <tkUserGlobal.h>
#include <tkUserIdentifier.h>
#include <tkUserModel.h>
#include <tkUserViewer.h>

// including contrib headers
#include <QuaZip>

// including Qt headers
#include <QMessageBox>
#include <QTemporaryFile>
#include <QHash>
#include <QObject>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS
Q_FMF_USING_CONSTANTS


class mfCorePrivate
{
public:
    static bool firstTimeRunning(QSplashScreen &splash);
public:
    // instances of object
    static QHash<const QMetaObject*, QObject*> mInstances;
    static QTemporaryFile * m_TempFile;
    static bool m_WineRunning;
};

// intialization of static datas
QHash<const QMetaObject*, QObject*> mfCorePrivate::mInstances;
QTemporaryFile * mfCorePrivate::m_TempFile = 0;
bool mfCorePrivate::m_WineRunning = false;

void showMessage( QSplashScreen* s, const QString& m )
{ s->showMessage( m, Qt::AlignRight | Qt::AlignBottom, Qt::white ); }

mfSettings* mfCore::settings()
{
    if ( !mfCorePrivate::mInstances.contains( &mfSettings::staticMetaObject ) )
        mfCorePrivate::mInstances[&mfSettings::staticMetaObject] = new mfSettings( qApp );
    return qobject_cast<mfSettings*>( mfCorePrivate::mInstances[&mfSettings::staticMetaObject] );
}

mfMainWindow* mfCore::mainWindow()
{
    if ( !mfCorePrivate::mInstances.contains( &mfMainWindow::staticMetaObject ) )
        mfCorePrivate::mInstances[&mfMainWindow::staticMetaObject] = new mfMainWindow();
    return qobject_cast<mfMainWindow*>( mfCorePrivate::mInstances[&mfMainWindow::staticMetaObject] );
}

mfMainWidget* mfCore::mainWidget()
{
    if ( !mfCorePrivate::mInstances.contains( &mfMainWidget::staticMetaObject ) )
        mfCorePrivate::mInstances[&mfMainWidget::staticMetaObject] = new mfMainWidget( 0, mainWindow() );
    return qobject_cast<mfMainWidget*>( mfCorePrivate::mInstances[&mfMainWidget::staticMetaObject] );
}

mfScriptor* mfCore::scriptor()
{
    if ( !mfCorePrivate::mInstances.contains( &mfScriptor::staticMetaObject ) )
        mfCorePrivate::mInstances[&mfScriptor::staticMetaObject] = new mfScriptor( qApp );
    return qobject_cast<mfScriptor*>( mfCorePrivate::mInstances[&mfScriptor::staticMetaObject] );
}

tkUpdateChecker* mfCore::updateChecker()
{
    if ( !mfCorePrivate::mInstances.contains( &tkUpdateChecker::staticMetaObject ) )
        mfCorePrivate::mInstances[&tkUpdateChecker::staticMetaObject] = new tkUpdateChecker( qApp );
    return qobject_cast<tkUpdateChecker*>( mfCorePrivate::mInstances[&tkUpdateChecker::staticMetaObject] );
}

tkTranslators* mfCore::translators()
{
    return tkTranslators::instance(qApp);
}

mfPluginsManager* mfCore::pluginsManager()
{
    if ( !mfCorePrivate::mInstances.contains( &mfPluginsManager::staticMetaObject ) )
        mfCorePrivate::mInstances[&mfPluginsManager::staticMetaObject] = new mfPluginsManager( qApp );
    return qobject_cast<mfPluginsManager*>( mfCorePrivate::mInstances[&mfPluginsManager::staticMetaObject] );
}

tkUserModel* mfCore::userModel()
{
    return tkUserModel::instance(qApp);
}

void mfCore::changeLanguage( const QString & lang )
{
    tkTranslators::instance(qApp)->changeLanguage( lang );
}

void mfCore::renewTemporaryFile()
{
    if ( mfCorePrivate::m_TempFile ) {  delete mfCorePrivate::m_TempFile;  mfCorePrivate::m_TempFile = 0;  }
    mfCorePrivate::m_TempFile = new QTemporaryFile( qApp );
    mfCorePrivate::m_TempFile->setFileTemplate( settings()->tempFileTemplate() );
}

QTemporaryFile * mfCore::temporaryFile()
{
    return mfCorePrivate::m_TempFile;
}

tkActionManager * mfCore::actionManager()
{
    return tkActionManager::instance();
}

//-------------------------------------------------------------------------------------------------------
//--------------------------------------------- Init Function -------------------------------------------
//-------------------------------------------------------------------------------------------------------
bool mfCore::init()
{
    // WINE compatibility
#ifdef Q_OS_WIN
    // For WINE testings
    if (QCoreApplication::arguments().contains("--wine") {
        mfCorePrivate::m_WineRunning = true;
        tkLog::addMessage( "mfCore", "Running under Wine environnement." );
        QFont::insertSubstitution("MS Shell Dlg", "Tahoma" );
        QFont::insertSubstitution("MS Shell Dlg 2", "Tahoma" );
    }
#endif

    tkLog::addMessage( "mfCore" , tkTr(STARTING_APPLICATION_AT_1).arg( QDateTime::currentDateTime().toString() ) );
    settings();

    QDir dir( QDir::cleanPath( settings()->path(tkSettings::QtPlugInsPath) ) ); //QApplication::applicationDirPath() + "/../plugins/qt/" ) );
    if (!tkGlobal::isDebugCompilation())
        QApplication::setLibraryPaths( QStringList() << dir.absolutePath() );
    tkLog::addMessage( "mfCore", tr("Setting library path to %1.").arg(dir.absolutePath()));

    // log infos about libraries
    foreach( QString l, QCoreApplication::libraryPaths() )
        tkLog::addMessage( "mfCore" , tkTr(USING_LIBRARY_1).arg( l ) );

    // create splashscreen
    QSplashScreen splash( tkTheme::splashScreen(FREEMEDFORMS_SPLASHSCREEN) );
    QFont ft( splash.font() );
#ifndef Q_OS_WIN
    ft.setPointSize( ft.pointSize() - 2 );
#endif
    ft.setBold( true );
    splash.setFont( ft );
    splash.show();

    // init translations
    showMessage( &splash, tkTr(INITIALIZING_TRANSLATIONS) );
    tkTranslators *t = translators();
    t->setPathToTranslations( settings()->translationPath() );
    t->addNewTranslator( "qt" );
    t->addNewTranslator( "fmf" );

    // initialize libs
    showMessage( &splash, tkTr(INITIALIZING_LIBRARIES) );
    tkGlobal::initLib();
    tkUserGlobal::initLib();

    // first time runnning ?
    if ( settings()->firstTimeRunning() ) {
        if (!mfCorePrivate::firstTimeRunning(splash))
            return false;
        settings()->noMoreFirstTimeRunning();
    } else if (settings()->licenseApprovedApplicationNumber() != qApp->applicationVersion()) {
        showMessage( &splash, tkTr(NEED_LICENSE_AGREEMENT ) );
        if (!tkGlobal::defaultLicenceAgreementDialog("", tkAboutDialog::BSD ))
            return false;
        settings()->setLicenseApprovedApplicationNumber(qApp->applicationVersion());
    }

    // before all identify user
    if ( settings()->needUserIdentification() ) {
        if (true) {
            tkUserIdentifier ident;
            if ( ident.exec() == QDialog::Rejected )
                return false;
            settings()->setValue(SETTING_LASTLOGIN, ident.login() );
            settings()->setValue(SETTING_LASTPASSWORD, ident.cryptedPassword() );
        }
    }

    // check applications directories
    showMessage( &splash, tkTr(CHECKING_DIRECTORY_STRUCTURE) );
//     if ( ! tkGlobal::checkDir( settings()->resourcesPath(), true, "Resources directory" ) )
//         return false;
    if ( ! tkGlobal::checkDir( settings()->translationPath(), true, "Translations directory" ) )
        return false;
    if ( ! tkGlobal::checkDir( settings()->temporaryPath(), true, "Temporary directory" ) )
        return false;
    if ( ! tkGlobal::checkDir( settings()->pluginPath(), true, "PlugIns directory" ) )
        return false;
    if ( ! tkGlobal::checkDir( settings()->formPath(), true, "Forms directory" ) )
        return false;
    if ( ! tkGlobal::checkDir( settings()->databasePath(), true, "Databases directory" ) )
        return false;

    // dezipping files ?
    showMessage( &splash, tr( "Check zipped files..." ) );
    mfGlobal::unzipAllFilesIntoDirs( QStringList() << settings()->databasePath() );

    // Check recovery file
    QDir tmpDir( settings()->temporaryPath() );
    tmpDir.setNameFilters( QStringList() << "*.tmp" );
    if ( tmpDir.entryList( QDir::Files ).count() ) {
        tkLog::addMessage( "mfCore" , tr( "Founded a crash recovering file at %1" ).arg( QDateTime::currentDateTime().toString() ) );
        bool ok = tkGlobal::yesNoMessageBox(tr( "A crash recovering file is available." ),
                                  tr( "Do you want to start the crash recoverer ?" ), "",
                                  qApp->applicationName());
//        tkGlobal::centerWidget( &msgBox );
        if (ok) {
            mfCrashRecovererDialog crash( 0 );
            crash.show();
            tkGlobal::centerWidget( &crash );
            crash.exec();
        }
    }

    // init pluginsmanager
    showMessage( &splash, tkTr( INITIALIZING_PLUGINS ) );
    pluginsManager()->loadPlugins();

    // creates the core scriptor
    scriptor();

    // initiate internet update checking
    showMessage( &splash, tkTr(CHECKING_UPDATES) );
    QObject::connect(updateChecker(), SIGNAL(updateFound()), mainWindow(), SLOT(updateFound()));
    updateChecker()->check(FREEMEDFORMS_UPDATE_URL);

    /** \todo connect updateChecker() with menu addition */

    // initiate temporaryFile
    renewTemporaryFile();

    // show main window
    mainWindow();
    if (tkGlobal::isRunningOnMac())
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
    tkActionManager::instance(mainWindow());
    tkContextManager::instance(mainWindow());
    mainWindow()->initialize();
    mainWindow()->show();

    // manage language changes
    connect(translators(), SIGNAL(languageChanged()), actionManager(), SLOT(retranslateMenusAndActions()));

    // finish splashscreen
    splash.finish( mainWindow() );

    // init pSettings
    //  pSettings::setIniInformations();

    // restore application style
    //  showMessage( &splash, tr( "Initializing Style..." ) );
    //  qApp->setStyle( settings()->value( "MainWindow/Style", "system" ).toString() );

    // set default settings if first time running
    //  if ( settings()->value( "FirstTimeRunning", true ).toBool() )
    //   settings()->setDefaultSettings();
    /*

      // start console manager
      showMessage( &splash, tr( "Initializing Console..." ) );
      consoleManager();

      // init main window
      showMessage( &splash, tr( "Initializing Main Window..." ) );
      mainWindow()->initGui();

      // init shortcuts editor
      showMessage( &splash, tr( "Initializing Shortcuts..." ) );
      MonkeyCore::actionManager()->setSettings( settings() );


      // restore window state
      showMessage( &splash, tr( "Restoring Workspace..." ) );
      mainWindow()->setSettings( settings() );

      // show main window
      mainWindow()->show();

      // restore session
      showMessage( &splash, tr( "Restoring Session..." ) );
      if ( pMonkeyStudio::restoreSessionOnStartup() )
       workspace()->fileSessionRestore_triggered();

      // ready
      showMessage( &splash, tr( "%1 v%2 Ready !" ).arg( PACKAGE_NAME, PACKAGE_VERSION ) );

      // finish splashscreen
      splash.finish( mainWindow() );

      // show settings dialog the first time user start program
      if ( settings()->value( "FirstTimeRunning", true ).toBool() )
      {
       // execute settings dialog
       if ( UISettings::instance()->exec() )
        settings()->setValue( "FirstTimeRunning", false );
      }

      // prepare apis
      pMonkeyStudio::prepareAPIs();

     */

    tkLog::addMessage( "mfCore" , tr( "Core intialization finished..." ) );
    return true;
}

bool mfCorePrivate::firstTimeRunning(QSplashScreen &splash)
{
    tkLog::addMessage( "mfCorePrivate", tkTr(FIRST_TIME_RUNNING) );
    showMessage( &splash, tkTr(NEED_LICENSE_AGREEMENT ) );
    if (!tkGlobal::defaultLicenceAgreementDialog("", tkAboutDialog::BSD ))
        return false;
    showMessage( &splash, tkTr(INITIALIZING_DEFAULTS_PARAMS) );
    mfSettings *s = mfCore::settings();
    s->noMoreFirstTimeRunning();
    s->setLicenseApprovedApplicationNumber(qApp->applicationVersion());
    /** \todo firstTimeForAllPlugins */
    return true;
}

/** \brief Assumes cleanings before closing the application */
void mfCore::endOfApplication()
{
    pluginsManager()->clearPlugins();
}

/*
pMenuBar* MonkeyCore::menuBar()
{ return mainWindow()->menuBar(); }


*/
