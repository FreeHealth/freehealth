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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "diMainWindow.h"

// include drugsinteractions headers
#include "diCore.h"
#include "diAboutDialog.h"
#include "diMedinTux.h"
#include "diPatient.h"

// include drugs widgets headers
#include <drugswidget/mfDrugsCentralWidget.h>
#include <drugspreferences/mfDrugsPreferences.h>
#include <drugsmodel/mfDrugsIO.h>
#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsManager.h>

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkSettings.h>
#include <tkTranslators.h>
#include <tkTheme.h>
#include <tkDebugDialog.h>
#include <tkPrinter.h>
#include <tkActionManager.h>
#include <tkUpdateChecker.h>
#include <tkConstantTranslations.h>
#include <tkHelpDialog.h>
#include <tkActionManager.h>
#include <tkContextManager.h>

// include Qt headers
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QTextBrowser>

//#define DEBUG

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

/** \brief Constructor */
diMainWindow::diMainWindow( QWidget *parent )
        : tkMainWindow(parent),
        prefAct(0), aboutAct(0), debugAct(0), configureMedinTuxAct(0), saveAct(0)
{
    // some initializing
    setObjectName( "diMainWindow" );
}

void diMainWindow::initialize()
{
    // create menus
    createFileMenu();
    createFileActions(A_FileOpen | A_FileSave | A_FileSaveAs | A_FileQuit);
    connectFileActions();
    tkActionManager::instance()->actionContainer(MENUBAR)->appendGroup(mfDrugsConstants::G_PLUGINS_DRUGS);

    createConfigurationMenu();
    createConfigurationActions(A_AppPreferences | A_ConfigureMedinTux);
    connectConfigurationActions();
    createHelpMenu();
    createHelpActions(A_AppAbout| A_AppHelp | A_DebugDialog);
    connectHelpActions();

    setupUi(this);

    readSettings();

    morePatientInfoButton->setIcon( tkTheme::icon(ICONADD) );
    patientInformations->hide();
    refreshPatient();

    mfDrugsManager::instance();
    m_CentralWidget->initialize();
    mfDrugsManager::instance()->setCurrentView(m_CentralWidget);

    tkContextManager::instance()->updateContext();
    tkActionManager::instance()->retranslateMenusAndActions();

    setWindowTitle( qApp->applicationName() + " - " + qApp->applicationVersion() );
    tkSettings *s = tkSettings::instance();
    changeFontTo( QFont(s->value( MFDRUGS_SETTING_VIEWFONT ).toString(), s->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt()) );

//    m_CentralWidget->setFocus();
}

/** \brief Destructor */
diMainWindow::~diMainWindow()
{
}

/**
  \brief Refresh the ui data refering to the patient
  \sa diCore::patient(), diPatient
*/
void diMainWindow::refreshPatient() const
{
    patientName->setText( diCore::patient()->value(diPatient::FullName).toString() );
    patientName->setToolTip( QString("Nom : %1<br />Date de naissance : %2<br />Poids : %3<br />"
                                     "Taille : %4<br />Clearance : %5")
                             .arg( diCore::patient()->value(diPatient::FullName).toString(),
                                   diCore::patient()->value(diPatient::DateOfBirth).toString(),
                                   diCore::patient()->value(diPatient::Weight).toString() )
                             .arg( diCore::patient()->value(diPatient::Size).toString(),
                                   diCore::patient()->value(diPatient::CreatinClearance).toString() ));

    patientWeight->setValue( diCore::patient()->value(diPatient::Weight).toInt() );
    patientSize->setValue( diCore::patient()->value(diPatient::Size).toInt() );
    patientClCr->setValue( diCore::patient()->value(diPatient::CreatinClearance).toDouble() );
    patientCreatinin->setValue( diCore::patient()->value(diPatient::Creatinin).toDouble() );
    listOfAllergies->setText(  diCore::patient()->value(diPatient::DrugsAllergies).toString() );
    sexCombo->setCurrentIndex( sexCombo->findText(diCore::patient()->value(diPatient::Sex).toString()) );
}

/** \brief Close the main window and the application */
void diMainWindow::closeEvent( QCloseEvent *event )
{
    writeSettings();
    // if is a medintux plugins --> save prescription to exchange file
    if (!diMedinTux::medintuxExchangeFileName().isEmpty()) {
        QString tmp = mfDrugsIO::instance()->prescriptionToHtml();
        tmp.replace("font-weight:bold;", "font-weight:600;");
        tkGlobal::saveStringToFile( tkGlobal::toHtmlAccent(tmp) , diMedinTux::medintuxExchangeFileName(), tkGlobal::DontWarnUser );
    } else if (!diCore::exchangeFileName().isEmpty()) {
        savePrescription(diCore::exchangeFileName());
    }
    event->accept();
}

/** \brief Reads main window's settings */
void diMainWindow::readSettings()
{
    tkSettings * s = diCore::settings();
    s->restoreState( this, MFDRUGS_SETTINGS_STATEPREFIX );
}

/** \brief Writes main window's settings */
void diMainWindow::writeSettings()
{
    tkSettings * s = diCore::settings();
    s->saveState( this, MFDRUGS_SETTINGS_STATEPREFIX );
    s->sync();
}

/** \obsolete */
void diMainWindow::createStatusBar()
{
    statusBar()->showMessage( tkTr(READY), 2000 );
}

/**
  \brief Shows the about dialog.
  \sa diAboutdialog
*/
bool diMainWindow::aboutApplication()
{
    diAboutDialog dialog(this);
    dialog.exec();
    return true;
}

/**
  \brief Shows the help dialog.
  \sa tkHelpDialog
*/
bool diMainWindow::applicationHelp()
{
    tkHelpDialog::showIndex();
    return true;
}

/**
  \brief Open the preferences dialog
  \sa mfDrugsPreferences
*/
bool diMainWindow::applicationPreferences()
{
    QTime t;
    t.start();
    tkSettings *s = diCore::settings();
    QDialog dlg(this);
    QVBoxLayout l(&dlg);
    mfDrugsPreferences prefs(&dlg);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
    l.addWidget( &prefs );
    l.addWidget( &buttonBox );
    dlg.setWindowTitle( tkTr(PREFERENCES_TEXT) + " - " + qApp->applicationName() );
    //    tkLog::logTimeElapsed(t, this->objectName(), "OpeningPreferences" );
    dlg.exec();
    if ( dlg.result() == QDialog::Accepted ) {
        prefs.saveToSettings(s);
        changeFontTo( QFont(s->value( MFDRUGS_SETTING_VIEWFONT ).toString(), s->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt()) );
    }
    return true;
}

/** \brief Change the font of the viewing widget */
void diMainWindow::changeFontTo( const QFont &font )
{
    m_CentralWidget->changeFontTo(font);
    patientName->setFont(font);
}


/**
  \brief Show the debugging dialog
  \sa tkDebugDialog
*/
bool diMainWindow::debugDialog()
{
    tkDebugDialog dialog( this, diCore::settings() );
    dialog.exec();
    return true;
}

/**
  \brief Prints the prescription using the header, footer and watermark.
  \sa tkPrinter
*/
bool diMainWindow::print()
{
    return m_CentralWidget->printPrescription();
}

/** \brief Runs the MedinTux configurator */
bool diMainWindow::configureMedintux()
{
    diMedinTux::configureMedinTux();
    return true;
}

bool diMainWindow::saveAsFile()
{
    return savePrescription();
}

bool diMainWindow::saveFile()
{
    return savePrescription();
}

/**
  \brief Saves a prescription. If fileName is empty, user is ask about a file name.
  \sa openPrescription()
  \sa mfDrugsIO
*/
bool diMainWindow::savePrescription( const QString &fileName )
{
    QString xmlExtra = diCore::patient()->toXml();
    return mfDrugsIO::savePrescription(xmlExtra, fileName);
}

/**
  \brief Opens a prescription saved using savePrescription().
  \sa savePrescription()
  \sa mfDrugsIO
*/
bool diMainWindow::openFile()
{
    QString f = QFileDialog::getOpenFileName(this,
                                             tkTr(OPEN_FILE),
                                             QDir::homePath(),
                                             tr(DRUGINTERACTION_FILEFILTER) );
    if (f.isEmpty())
        return false;
    //    QString f = "/Users/eric/prescription.di";
    QHash<QString,QString> datas;
    if (DRUGMODEL->rowCount() > 0) {
        int r = tkGlobal::withButtonsMessageBox(
                tr("Opening a prescription : merge or replace ?"),
                tr("There is a prescription inside editor, do you to replace it or to add the opened prescription ?"),
                QString(), QStringList() << tr("Replace prescription") << tr("Add to prescription"),
                tr("Open a prescription") + " - " + qApp->applicationName());
        if (r == 0) {
            mfDrugsIO::loadPrescription(f, datas, mfDrugsIO::ReplacePrescription);
        } else if (r==1) {
            mfDrugsIO::loadPrescription(f, datas, mfDrugsIO::AppendPrescription);
        }
    } else
        mfDrugsIO::loadPrescription(f, datas, mfDrugsIO::ReplacePrescription);

    diCore::patient()->setValue(diPatient::FullName, QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
    patientName->setText( QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
    return true;
}

/** \brief Always keep uptodate patient's datas */
void diMainWindow::on_patientName_textChanged(const QString &text)
{
    diCore::patient()->setValue(diPatient::FullName, text);
}

/** \brief Always keep uptodate patient's datas */
void diMainWindow::on_sexCombo_currentIndexChanged(const QString &text)
{
    diCore::patient()->setValue(diPatient::Sex, text);
}

/** \brief Always keep uptodate patient's datas */
void diMainWindow::on_patientWeight_valueChanged(const QString &text)
{
    diCore::patient()->setValue(diPatient::Weight, text);
}

/** \brief Always keep uptodate patient's datas */
void diMainWindow::on_patientSize_valueChanged(const QString & text)
{
    diCore::patient()->setValue(diPatient::Size, text);
}

/** \brief Always keep uptodate patient's datas */
void diMainWindow::on_patientClCr_valueChanged(const QString & text)
{
    diCore::patient()->setValue(diPatient::CreatinClearance, text);
}

/** \brief Always keep uptodate patient's datas */
void diMainWindow::on_patientCreatinin_valueChanged(const QString & text)
{
    diCore::patient()->setValue(diPatient::Creatinin, text);
}

/** \brief Always keep uptodate patient's datas */
void diMainWindow::on_listOfAllergies_textChanged(const QString &text)
{
    diCore::patient()->setValue(diPatient::DrugsAllergies, text);
}
