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
#include <drugswidget/mfDrugsPreferences.h>
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsIO.h>

// including toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkTranslators.h>
#include <tkSettings.h>
//#include <tkSerialNumber.h>
#include <tkUpdateChecker.h>
#include <tkTheme.h>

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
                m_MedintuxExchangeFile = arg.mid( arg.indexOf("=") +1 ).remove("\"");
            else if (arg.contains(COMMANDLINE_PATIENTNAME)) {
                m_PatientName = arg.mid( arg.indexOf("=") + 1 ).remove("\"");
            } else if (arg.contains(COMMANDLINE_PATIENTDOB)) {
                m_PatientDOB = arg.mid( arg.indexOf("=") + 1 ).remove("\"");
            } else if (arg.contains(COMMANDLINE_PATIENTWEIGHT)) {
                m_PatientWeight = arg.mid( arg.indexOf("=") + 1 ).remove("\"");
            } else if (arg.contains(COMMANDLINE_PATIENTSIZE)) {
                m_PatientSize = arg.mid( arg.indexOf("=") + 1 ).remove("\"");
            } else if (arg.contains(COMMANDLINE_PATIENTCLCR)) {
                m_PatientClCr = arg.mid( arg.indexOf("=") + 1 ).remove("\"");
            } else if (arg.contains(COMMANDLINE_CREATININ)) {
                m_PatientCreat = arg.mid( arg.indexOf("=") + 1 ).remove("\"");
            } else if (arg.contains(COMMANDLINE_CHRONOMETER)) {
                m_Chrono = true;
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
    static bool isMedintuxPlugins;
    static bool isMedintuxPluginsTested;
    static QString m_MedintuxExchangeFile;
    static QString m_PatientName;
    static QString m_PatientDOB;
    static QString m_PatientWeight;
    static QString m_PatientSize;
    static QString m_PatientClCr;
    static QString m_PatientCreat;
    static bool m_WineRunning;
    static bool m_Chrono;
};

QHash<const QMetaObject*, QObject*> diCorePrivate::mInstances;
bool diCorePrivate::isMedintuxPlugins = false;
bool diCorePrivate::isMedintuxPluginsTested = false;
QString diCorePrivate::m_MedintuxExchangeFile = "";
QString diCorePrivate::m_PatientName = "";
QString diCorePrivate::m_PatientDOB = "";
QString diCorePrivate::m_PatientWeight = "";
QString diCorePrivate::m_PatientSize = "";
QString diCorePrivate::m_PatientClCr = "";
QString diCorePrivate::m_PatientCreat = "";
bool diCorePrivate::m_WineRunning = false;
bool diCorePrivate::m_Chrono = false;

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
  \sa mfDrugsModel::instance()
*/
mfDrugsModel* diCore::drugsModel()
{
    return mfDrugsModel::instance();
}

/**
  \brief Returns the medintux manager
  \sa tkMedintuxConfiguration::instance()
*/
tkMedintuxConfiguration *medintuxConfiguration()
{
    return tkMedintuxConfiguration::instance();  // need to be deleted
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
    QTime chrono;
    chrono.start();

    diCorePrivate::commandLineParsing();
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "command line parsing");

    // create splashscreen
    tkLog::addMessage( "diCore" , QCoreApplication::translate( "diCore", "Starting application at %1" ).arg( QDateTime::currentDateTime().toString() ) );
    settings();
    QSplashScreen splash( tkTheme::splashScreen(DRUGSINTERACTIONS_SPLASHSCREEN) );
    splash.show();
    QFont ft( splash.font() );
#ifndef Q_OS_WIN
    QFont::insertSubstitution("Lucida Grande", "Arial");
#endif
    ft.setPointSize( ft.pointSize() - 2 );
    ft.setBold( true );
    splash.setFont( ft );
    splash.show();

    // initialize the settings
    showMessage( &splash, QCoreApplication::translate( "diCore", "Getting settings file..." ) );
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "settings and splash");

    // log infos about libraries
    QApplication::addLibraryPath( settings()->path(tkSettings::QtPlugInsPath) );
    foreach( QString l, QCoreApplication::libraryPaths() )
        tkLog::addMessage( "diCore" , QCoreApplication::translate( "diCore", "DrugInteractions using library : %1" ).arg( l ) );

    // init translations
    showMessage( &splash, QCoreApplication::translate( "diCore", "Initializing Translations..." ) );
    tkTranslators *t = translators();
    t->setPathToTranslations( settings()->path( tkSettings::TranslationsPath ) );
    t->addNewTranslator( "mfDrugsWidget" );
    t->addNewTranslator( "qt" );
    tkGlobal::initLib();
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "initializing translations");

    // first time runnning ?
    if ( settings()->firstTimeRunning() ) {
        // show the license agreement dialog
        if (!tkGlobal::defaultLicenceAgreementDialog("", tkAboutDialog::BSD ))
            return false;
        showMessage( &splash, QCoreApplication::translate( "diCore", "Initializing Default Parameters..." ) );
        tkSettings *s = settings();
        s->noMoreFirstTimeRunning();
        mfDrugsPreferences::writeDefaultSettings(settings());
    }

    // intialize drugsBase
    mfDrugsBase::instance()->logChronos( diCorePrivate::m_Chrono );

    showMessage( &splash, QCoreApplication::translate( "diCore", "Initializing Drugs database..." ) );
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "initializing drugs base");


    // WINE compatibility
#ifdef Q_OS_WIN
    // For WINE testings
    if (QCoreApplication::arguments().contains("--wine")) {
        diCorePrivate::m_WineRunning = true;
        tkLog::addMessage( "diCore", "Running under Wine environnement." );
        QFont::insertSubstitution("MS Shell Dlg", "Tahoma" );
        QFont::insertSubstitution("MS Shell Dlg 2", "Tahoma" );
    }
#endif

    showMessage( &splash, QCoreApplication::translate( "diCore", "Checking command line parameters..." ) );
    diMedinTux::isMedinTuxPlugIns(&splash);

    // show main window
    mainWindow()->show();

    // start update checking
    showMessage( &splash, QCoreApplication::translate( "diCore", "Checking for updates..." ) );
    QObject::connect(updateChecker(), SIGNAL(updateFounded()), mainWindow(), SLOT(on_updateFounded()));
    updateChecker()->check(DRUGSINTERACTIONS_UPDATE_URL);
    if (diCorePrivate::m_Chrono)
        tkLog::logTimeElapsed(chrono, "diCore", "start update checker");

    if (diMedinTux::isMedinTuxPlugIns( &splash )) {
        showMessage( &splash, QCoreApplication::translate( "diCore", "Raising Application..." ) );
        tkLog::addMessage( "diCore" , QCoreApplication::translate( "diCore", "Trying to raise application" ) );
        qApp->setActiveWindow( mainWindow() );
        mainWindow()->activateWindow();
        mainWindow()->raise();
        if (diCorePrivate::m_Chrono)
            tkLog::logTimeElapsed(chrono, "diCore", "medintux plugins preparation");
    }

    // Update countdown to transmission
    int count = settings()->value(SETTINGS_COUNTDOWN,0).toInt();
    ++count;
    if (count) {
        settings()->setValue(SETTINGS_COUNTDOWN,0);
//        diCorePrivate::transmitDosage();
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

/** \brief Returns the patient name. */
QString & diCore::patientName()
{
    return diCorePrivate::m_PatientName;
}

/** \brief Set patient name. */
void diCore::setPatientName( const QString &name)
{
    diCorePrivate::m_PatientName = name;
}

/** \brief Patient date of birth */
QString & diCore::patientDateOfBirth()
{
    return diCorePrivate::m_PatientDOB;
}

/** \todo Todo */
QString & diCore::patientAge()
{
    return diCorePrivate::m_PatientDOB;
}

/** \brief Patient date of birth */
void diCore::setPatientDateOfBirth( const QString &date)
{
    diCorePrivate::m_PatientDOB = date;
}

/** \brief Patient weight */
QString & diCore::patientWeight()
{
    return diCorePrivate::m_PatientWeight;
}

/** \brief Patient size */
QString & diCore::patientSize()
{
    return diCorePrivate::m_PatientSize;
}

/**
  \brief Patient Clearance of creatinin
  \todo Automatically calculates clcr is possible
*/
QString & diCore::patientClCr()
{
    return diCorePrivate::m_PatientClCr;
}
