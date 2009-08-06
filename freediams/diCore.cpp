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
 *   This code is inspired of the Monkey Studio project                    *
 *   http://www.monkeystudio.org/                                          *
 *   Filipe AZEVEDO aka Nox P@sNox <pasnox@gmail.com>                      *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "diCore.h"

// including drugsinsteractions headers
#include "diMainWindow.h"
#include "diMedinTux.h"

// including drugs plugins headers
#include <drugsmodel/mfDrugsModel.h>
#include <drugspreferences/mfDrugsPreferences.h>
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsIO.h>
#include <mfDrugsManager.h>

// including toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkTranslators.h>
#include <tkSettings.h>
#include <tkUpdateChecker.h>
#include <tkTheme.h>
#include <tkActionManager.h>
#include <tkContextManager.h>

// including medintux toolkit headers
#include <tkMedintuxConfiguration.h>

// including Qt headers
#include <QObject>
#include <QMessageBox>
#include <QSplashScreen>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QSplashScreen>


namespace diCorePrivateConstants {
    const char* const  COMMANDLINE_ISMEDINTUXPLUGINS  = "--medintux";
    const char* const  COMMANDLINE_EXCHANGEFILE       = "--exchange";
    const char* const  COMMANDLINE_PATIENTNAME        = "--patientname";
    const char* const  COMMANDLINE_PATIENTDOB         = "--dateofbirth";
    const char* const  COMMANDLINE_PATIENTWEIGHT      = "--weight";
    const char* const  COMMANDLINE_PATIENTSIZE        = "--size";
    const char* const  COMMANDLINE_PATIENTCLCR        = "--clcr";
    const char* const  COMMANDLINE_CHRONOMETER        = "--chrono";
    const char* const  COMMANDLINE_CREATININ          = "--creatinin";
    const char* const  COMMANDLINE_TRANSMITDOSAGES    = "--transmit-dosage";
//    const char* const  COMMANDLINE_  = "--";
//    const char* const  COMMANDLINE_  = "--";

    const char* const  SETTINGS_COUNTDOWN             = "transmissionCountDown";
}

Q_TK_USING_CONSTANTS
using namespace diCorePrivateConstants;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////   PRIVATE PART   /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class diCorePrivate
{
public:

    static void commandLineParsing()
    {
        QStringList args = QCoreApplication::arguments();
        foreach(QString arg, args) {
            //        tkLog::addMessage("diCore",arg);
            if (arg.startsWith(COMMANDLINE_ISMEDINTUXPLUGINS))
                isMedintuxPlugins = true;
            else if (arg.startsWith(COMMANDLINE_EXCHANGEFILE))
                m_ExchangeFile = arg.mid( arg.indexOf("=") +1 ).remove("\"");
            else if (arg.contains(COMMANDLINE_PATIENTNAME)) {
                diCore::patient()->setValue(diPatient::FullName, arg.mid( arg.indexOf("=") + 1 ).remove("\"") );
            } else if (arg.contains(COMMANDLINE_PATIENTDOB)) {
                diCore::patient()->setValue(diPatient::DateOfBirth, arg.mid( arg.indexOf("=") + 1 ).remove("\""));
            } else if (arg.contains(COMMANDLINE_PATIENTWEIGHT)) {
                diCore::patient()->setValue(diPatient::Weight, arg.mid( arg.indexOf("=") + 1 ).remove("\""));
            } else if (arg.contains(COMMANDLINE_PATIENTSIZE)) {
                diCore::patient()->setValue(diPatient::Size, arg.mid( arg.indexOf("=") + 1 ).remove("\""));
            } else if (arg.contains(COMMANDLINE_PATIENTCLCR)) {
                diCore::patient()->setValue(diPatient::CreatinClearance, arg.mid( arg.indexOf("=") + 1 ).remove("\""));
            } else if (arg.contains(COMMANDLINE_CREATININ)) {
                diCore::patient()->setValue(diPatient::Creatinin, arg.mid( arg.indexOf("=") + 1 ).remove("\""));
            } else if (arg.contains(COMMANDLINE_CHRONOMETER)) {
                m_Chrono = true;
            } else if (arg.contains(COMMANDLINE_TRANSMITDOSAGES)) {
                m_TransmitDosage = true;
            }
        }
//        if (tkGlobal::isDebugCompilation())
//            m_Chrono = true;
    }

    static bool transmitDosage()
    {
        tkLog::addMessage("diCore", QCoreApplication::translate("diCore", "Preparing dosage transmission"));
        mfDrugsIO::instance()->startsDosageTransmission();
        return true;
    }

public:
    // instances of object
    static QHash<const QMetaObject*, QObject*> mInstances;
    static diPatient *m_Patient;
    static bool isMedintuxPlugins;
    static bool isMedintuxPluginsTested;
    static QString m_ExchangeFile;
    static bool m_WineRunning;
    static bool m_Chrono;
    static bool m_TransmitDosage;
};

QHash<const QMetaObject*, QObject*> diCorePrivate::mInstances;
diPatient *diCorePrivate::m_Patient = 0;
bool diCorePrivate::isMedintuxPlugins = false;
bool diCorePrivate::isMedintuxPluginsTested = false;
QString diCorePrivate::m_ExchangeFile = "";
bool diCorePrivate::m_WineRunning = false;
bool diCorePrivate::m_Chrono = false;
bool diCorePrivate::m_TransmitDosage = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////   PUBLIC PART   /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showMessage( QSplashScreen* s, const QString& m )
{ s->showMessage( m, Qt::AlignRight | Qt::AlignBottom, Qt::black ); }

/** \brief Returns the settings manager */
tkSettings* diCore::settings()
{
    return tkSettings::instance(qApp);
}

/** \brief Returns the mainwindow */
diMainWindow* diCore::mainWindow()
{
    if ( !diCorePrivate::mInstances.contains( &diMainWindow::staticMetaObject ) )
        diCorePrivate::mInstances[&diMainWindow::staticMetaObject] = new diMainWindow();
    return qobject_cast<diMainWindow*>( diCorePrivate::mInstances[&diMainWindow::staticMetaObject] );
}

/** \brief Returns the translators manager */
tkTranslators* diCore::translators()
{
    return tkTranslators::instance(qApp);
}

/** \brief Returns the update checker manager */
tkUpdateChecker *diCore::updateChecker()
{
    if ( !diCorePrivate::mInstances.contains( &tkUpdateChecker::staticMetaObject ) )
        diCorePrivate::mInstances[&tkUpdateChecker::staticMetaObject] = new tkUpdateChecker(qApp);
    return qobject_cast<tkUpdateChecker*>( diCorePrivate::mInstances[&tkUpdateChecker::staticMetaObject] );
}

/**
  \brief Returns the unique drugs model
  \sa mfDrugsManager::DRUGMODEL
*/
mfDrugsModel* diCore::drugsModel()
{
    return DRUGMODEL;
}

/**
  \brief Returns the medintux manager
  \sa tkMedintuxConfiguration::instance()
*/
tkMedintuxConfiguration *medintuxConfiguration()
{
    return tkMedintuxConfiguration::instance();  // need to be deleted
}

/** \brief Returns the unique instance of diPatient class. It represents the actual patient */
diPatient *diCore::patient()
{
    if (!diCorePrivate::m_Patient)
        diCorePrivate::m_Patient = new diPatient();
    return diCorePrivate::m_Patient;
}

//-------------------------------------------------------------------------------------------------------
//--------------------------------------------- Init Function -------------------------------------------
//-------------------------------------------------------------------------------------------------------
/**
  \brief initialize the core manager.
  \todo start a thread that : opens dosagemodel, filter to non transmitted dosage, transform them to xml, send over the web
*/
bool diCore::init()
{
    tkLog::muteConsoleWarnings();
    // Get settings
    settings();
    // Set application libraries
    if (!tkGlobal::isDebugCompilation()) {
        QApplication::setLibraryPaths( QStringList() << settings()->path(tkSettings::QtPlugInsPath) );
    }
    foreach( QString l, QCoreApplication::libraryPaths() )
        tkLog::addMessage( "diCore" , tkTr(USING_LIBRARY_1).arg( l ) );

    QTime chrono;
    chrono.start();

    // WINE compatibility (only for testing under ubuntu when crosscompiling)
#ifdef Q_OS_WIN
    // For WINE testings
    if (QCoreApplication::arguments().contains("--wine")) {
        diCorePrivate::m_WineRunning = true;
        tkLog::addMessage( "diCore", "Running under Wine environnement." );
        QFont::insertSubstitution("MS Shell Dlg", "Tahoma" );
        QFont::insertSubstitution("MS Shell Dlg 2", "Tahoma" );
    }
#endif

    diCorePrivate::commandLineParsing();
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "command line parsing");

    // create splashscreen
    tkLog::addMessage( "diCore" , tkTr(STARTING_APPLICATION_AT_1).arg( QDateTime::currentDateTime().toString() ) );
    QSplashScreen splash( tkTheme::splashScreen(tkConstants::FREEDIAMS_SPLASHSCREEN) );
    splash.show();
    QFont ft( splash.font() );
//#ifndef Q_OS_WIN
//    QFont::insertSubstitution("Lucida Grande", "Arial");
//#endif
    ft.setPointSize( ft.pointSize() - 2 );
    ft.setBold( true );
    splash.setFont( ft );
    splash.show();

    // initialize the settings
    showMessage( &splash, tkTr(GETTING_SETTINGS_FILE));
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "settings and splash");

    // init translations
    showMessage( &splash, tkTr(INITIALIZING_TRANSLATIONS) );
    tkTranslators *t = translators();
    t->setPathToTranslations( settings()->path( tkSettings::TranslationsPath ) );
    t->addNewTranslator( "mfDrugsWidget" );
    t->addNewTranslator( "qt" );
    tkGlobal::initLib();
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", tkTr(INITIALIZING_TRANSLATIONS));

    // first time runnning ?
    if (settings()->firstTimeRunning()) {
        // show the license agreement dialog
        showMessage( &splash,tkTr(NEED_LICENSE_AGREEMENT));
        if (!tkGlobal::defaultLicenceAgreementDialog("", tkAboutDialog::BSD ))
            return false;
        showMessage( &splash, tkTr(INITIALIZING_DEFAULTS_PARAMS) );
        tkSettings *s = settings();
        s->noMoreFirstTimeRunning();
        settings()->setLicenseApprovedApplicationNumber( qApp->applicationVersion());
        mfDrugsPreferences::writeDefaultSettings(settings());
    } else if (settings()->licenseApprovedApplicationNumber() != qApp->applicationVersion()) {
        // show the license agreement dialog
        if (!tkGlobal::defaultLicenceAgreementDialog(QCoreApplication::translate("diCore", "You are running a new version of FreeDiams, you need to renew the licence agreement."), tkAboutDialog::BSD ))
            return false;
        settings()->setLicenseApprovedApplicationNumber( qApp->applicationVersion());
    }

    // intialize drugsBase
    showMessage( &splash, tkTr(INITIALIZATING_DATABASES) );
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "initializing drugs base");
    mfDrugsBase::instance()->logChronos( diCorePrivate::m_Chrono );

    // instanciate managers
    mainWindow();
    tkActionManager::instance(mainWindow());
    tkContextManager::instance(mainWindow());
    tkContextManager::instance()->addAdditionalContext(tkUID->uniqueIdentifier(mfDrugsConstants::C_DRUGS_PLUGINS));
    mainWindow()->initialize();
    // show main window
    mainWindow()->show();

    // Manage exchange file
    showMessage( &splash, QCoreApplication::translate( "diCore", "Checking command line parameters..." ) );
    if (!diMedinTux::isMedinTuxPlugIns(&splash)) {
        if (!diCorePrivate::m_ExchangeFile.isEmpty()) {
            showMessage( &splash, QCoreApplication::translate( "diCore", "Reading exchange file..." ) );
            QString extras;
            mfDrugsIO::loadPrescription(diCorePrivate::m_ExchangeFile, extras);
            patient()->fromXml(extras);
        }
    }

    if (tkGlobal::isRunningOnMac())
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    // start update checking
    showMessage( &splash, tkTr(CHECKING_UPDATES) );
    QObject::connect(updateChecker(), SIGNAL(updateFound()), mainWindow(), SLOT(updateFound()));
    updateChecker()->check(FREEDIAMS_UPDATE_URL);
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", tkTr(CHECKING_UPDATES));

    if (diMedinTux::isMedinTuxPlugIns( &splash )) {
        showMessage( &splash, tkTr(RAISING_APPLICATION) );
        tkLog::addMessage( "diCore" , tkTr(RAISING_APPLICATION) );
        qApp->setActiveWindow( mainWindow() );
        mainWindow()->activateWindow();
        mainWindow()->raise();
        if (diCorePrivate::m_Chrono)
            tkLog::logTimeElapsed(chrono, "diCore", "medintux plugins preparation");
    }
    mfDrugsManager::instance()->drugsModelChanged();

    // Update countdown to dosage transmission
    int count = settings()->value(SETTINGS_COUNTDOWN,0).toInt();
    ++count;
    if ((count==30) || (diCorePrivate::m_TransmitDosage)) {
        settings()->setValue(SETTINGS_COUNTDOWN,0);
        diCorePrivate::transmitDosage();
    } else
        settings()->setValue(SETTINGS_COUNTDOWN,count);

    // ready
    showMessage( &splash, QCoreApplication::translate( "diCore", "%1 v%2 Ready !" ).arg( PACKAGE_NAME, PACKAGE_VERSION ) );
    // finish splashscreen
    splash.finish( mainWindow() );

    tkLog::addMessage( "diCore" , QCoreApplication::translate( "diCore", "Core intialization finished..." ) );
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "end of core intialization");

    return true;
}

QString diCore::exchangeFileName()
{
    return diCorePrivate::m_ExchangeFile;
}
