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

// include drugs widgets headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugswidget/mfDrugSelector.h>
#include <drugswidget/mfDosageDialog.h>
#include <drugswidget/mfDosageCreatorDialog.h>
#include <drugswidget/mfPrescriptionViewer.h>
#include <drugswidget/mfDrugsPreferences.h>
#include <drugsmodel/mfDrugsIO.h>

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

const char* const A_CONFIG_MEDINTUX          = "configureMedinTuxAction";
const char* const CONFIGMEDINTUX_TEXT        = QT_TRANSLATE_NOOP("diMainWindow", "Configure MedinTux");

namespace diMainWindowPrivate {

void readName(QString &serializedPrescription)
{
    int begin = serializedPrescription.indexOf("<name>") + 6;
    int end = serializedPrescription.indexOf("</name>", begin);
    QString name = QByteArray::fromBase64(( serializedPrescription.mid( begin, end - begin).toAscii() ));
    diCore::setPatientName( name );
}

}  // end namespace diMainWindowPrivate


/** \brief Constructor */
diMainWindow::diMainWindow( QWidget *parent )
          : QMainWindow(parent),
          m_DrugPrescriptor( 0 ),
          m_PrescriptionModel(0),
          m_DosageDialog(0),
          prefAct(0), aboutAct(0), debugAct(0), configureMedinTuxAct(0), saveAct(0)
{
    // some initializing
    setObjectName( "diMainWindow" );
    setupUi(this);
    createMenus();
    readSettings();

    morePatientInfoButton->setIcon( tkTheme::icon(ICONADD) );
    patientInformations->hide();
    patientName->setText( diCore::patientName() );
    patientName->setToolTip( QString("Nom : %1<br />Date de naissance : %2<br />Poids : %3<br />"
                                     "Taille : %4<br />Clearance : %5")
                             .arg( diCore::patientName(), diCore::patientDateOfBirth(), diCore::patientWeight() )
                             .arg( diCore::patientSize(), diCore::patientClCr() ));
    patientWeight->setValue( diCore::patientWeight().toInt() );
    patientSize->setValue( diCore::patientSize().toInt() );
    patientClCr->setValue( diCore::patientClCr().toDouble() );
//    patientCreatinin->setValue( diCore::patientCreatinin() );

    // create model view for selected drugs list
    m_PrescriptionModel = mfDrugsModel::instance();
    m_PrescriptionView->initialize();
    m_PrescriptionView->setModel( m_PrescriptionModel );
    m_PrescriptionView->setModelColumn( Drug::FullPrescription );

    createConnections();

    setWindowTitle( qApp->applicationName() + " - " + qApp->applicationVersion() );
    tkSettings *s = tkSettings::instance();
    changeFontTo( QFont(s->value( MFDRUGS_SETTING_VIEWFONT ).toString(), s->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt()) );

    m_DrugSelector->setFocus();
}

/** \brief Destructor */
diMainWindow::~diMainWindow()
{
}

/** \brief Creates connections */
void diMainWindow::createConnections()
{
    tkActionManager *am = tkActionManager::instance();
    connect( am->action(A_FILE_OPEN), SIGNAL( triggered() ), this, SLOT( openPrescription() ) );
    connect( am->action(A_FILE_SAVE), SIGNAL( triggered() ), this, SLOT( savePrescription() ) );
    connect( am->action(A_FILE_PRINT), SIGNAL( triggered() ), this, SLOT( printPrescription() ) );
    connect( am->action(A_FILE_EXIT), SIGNAL( triggered() ), this, SLOT( close() ) );

    connect( am->action(A_PREFERENCES), SIGNAL( triggered() ), this, SLOT( preferences() ) );
    connect( am->action(A_ABOUT), SIGNAL( triggered() ), this, SLOT( about() ) );
    connect( am->action(A_DEBUGHELPER), SIGNAL( triggered() ), this, SLOT( debugDialog() ) );
    connect( am->action(A_CONFIG_MEDINTUX), SIGNAL( triggered() ), this, SLOT( configureMedinTux() ) );

    connect( m_DrugSelector, SIGNAL(drugSelected(int)), this, SLOT( on_selector_drugSelected(const int) ) );
    connect( m_PrescriptionView, SIGNAL(printTriggered()), this, SLOT(printPrescription()));
    connect( m_PrescriptionView->listview(), SIGNAL(activated(const QModelIndex &)), this, SLOT(showDosageDialog(const QModelIndex&)) );
}

/** \brief Close the main window and the application */
void diMainWindow::closeEvent( QCloseEvent *event )
{
    writeSettings();
    // if is a medintux plugins --> save prescription to exchange file
    if ( !diMedinTux::medintuxExchangeFileName().isEmpty() ) {
        QString tmp = mfDrugsIO::instance()->prescriptionToHtml();
        tmp.replace("font-weight:bold;", "font-weight:600;");
        tkGlobal::saveStringToFile( tkGlobal::toHtmlAccent(tmp) , diMedinTux::medintuxExchangeFileName(), tkGlobal::DontWarnUser );
    }
    event->accept();
}

void diMainWindow::on_selector_drugSelected( const int CIS )
{
    // if exists dosage for that drug show the dosageSelector widget
    // else show the dosage creator widget
    if (m_PrescriptionModel->containsDrug(CIS)) {
        tkGlobal::warningMessageBox(tr("Can not add this drug to your prescription."),
                                    tr("Prescription can not contains twice the sample pharmaceutical drug.\n"
                                       "Drug %1 is already in your prescription").arg(m_PrescriptionModel->drugData(CIS,Drug::Denomination).toString()),
                                    tr("If you want to change the dosage of this drug please double-click on it in the prescription box."));
        return;
    }
    int drugPrescriptionRow = m_PrescriptionModel->addDrug(CIS);
    mfDosageCreatorDialog dlg(this, mfDrugsModel::instance()->dosageModel(CIS));
    if (dlg.exec()==QDialog::Rejected) {
        m_PrescriptionModel->removeLastInsertedDrug();
    }
    m_PrescriptionView->listview()->update();

//    if (!m_DosageDialog)
//        m_DosageDialog = new mfDosageDialog( this, drugPrescriptionRow, 0 );
//    else
//        m_DosageDialog->changeRow(drugPrescriptionRow,0);
//
//    int r = m_DosageDialog->exec();
//    if ( r == QDialog::Rejected )
//        m_PrescriptionModel->removeLastInsertedDrug();
//    m_PrescriptionView->listview()->update();
}

/** \brief Opens mfDosageDialog with the good params */
void diMainWindow::showDosageDialog(const QModelIndex &item)
{
    int CIS = mfDrugsModel::instance()->index( item.row(), Drug::CIS ).data().toInt();
    if (!m_DosageDialog)
        m_DosageDialog = new mfDosageDialog(this);
    m_DosageDialog->changeRow(CIS,0) ;
    m_DosageDialog->exec();
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

/** \brief Creates the menubar of the application using the tkActionManager */
void diMainWindow::createMenus()
{
    tkActionManager *am = tkActionManager::instance();
    // create menu structure
    am->createMenuBar(MENUBAR, this);
    am->createDefaultFileMenu(this);
    am->createMenu( M_CONFIGURATION  ,  MENUBAR, M_CONFIGURATION_TEXT, "", this );
    am->appendGroup( G_CONFIGURATION,   M_CONFIGURATION );
    am->createDefaultAboutMenu(this);
    // add default actions
    am->createDefaultFileMenuActions(this);
    am->createDefaultConfigurationMenuActions(this);
    am->createDefaultAboutMenuActions(this);
    // create specific actions
    am->createAction(A_CONFIG_MEDINTUX, G_CONFIGURATION, this);
    am->setActionDatas(A_CONFIG_MEDINTUX, CONFIGMEDINTUX_TEXT, CONFIGMEDINTUX_TEXT, ICONMEDINTUX);
    am->refreshMenuBar(MENUBAR);
    setMenuBar(am->menubar(MENUBAR));
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
void diMainWindow::about()
{
   diAboutDialog dialog(this);
   dialog.exec();
}

/**
  \brief Open the preferences dialog
  \sa mfDrugsPreferences
*/
void diMainWindow::preferences()
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
    dlg.setWindowTitle( tkTr(PREFERENCES) + " - " + qApp->applicationName() );
//    tkLog::logTimeElapsed(t, this->objectName(), "OpeningPreferences" );
    dlg.exec();
    if ( dlg.result() == QDialog::Accepted ) {
        prefs.saveToSettings(s);
        changeFontTo( QFont(s->value( MFDRUGS_SETTING_VIEWFONT ).toString(), s->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt()) );
    }
}

/** \brief Change the font of the viewing widget */
void diMainWindow::changeFontTo( const QFont &font )
{
    m_DrugSelector->setFont(font);
    m_PrescriptionView->listview()->setFont(font);
    patientName->setFont(font);
}


/**
  \brief Show the debugging dialog
  \sa tkDebugDialog
*/void diMainWindow::debugDialog()
{
    tkDebugDialog dialog( this, diCore::settings() );
    dialog.exec();
}

/**
  \brief Prints the prescription using the header, footer and watermark.
  \sa tkPrinter
*/
void diMainWindow::printPrescription()
{
    tkPrinter p(this);
    p.askForPrinter(this);
    p.printWithDuplicata(true);
    QString header = diCore::settings()->value( MFDRUGS_SETTING_USERHEADER ).toString();
    tkGlobal::replaceToken(header, TOKEN_PATIENTNAME, patientName->text() );
    tkGlobal::replaceToken(header, TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    tkGlobal::replaceToken(header, TOKEN_WEIGHT, diCore::patientWeight() );
    tkGlobal::replaceToken(header, TOKEN_SIZE, diCore::patientSize() );
    tkGlobal::replaceToken(header, TOKEN_DATEOFBIRTH, diCore::patientDateOfBirth() );
    tkGlobal::replaceToken(header, TOKEN_CLCR, diCore::patientClCr() );
    p.setHeader( header );
    p.setFooter( diCore::settings()->value( MFDRUGS_SETTING_USERFOOTER ).toString() );
    p.addHtmlWatermark( diCore::settings()->value( MFDRUGS_SETTING_WATERMARK_HTML ).toString(),
                        tkPrinter::Presence(diCore::settings()->value( MFDRUGS_SETTING_WATERMARKPRESENCE ).toInt()),
                       Qt::AlignmentFlag(diCore::settings()->value( MFDRUGS_SETTING_WATERMARKALIGNEMENT ).toInt()));
    p.printWithDuplicata(true);
    p.print( mfDrugsIO::instance()->prescriptionToHtml() );
}

/** \brief Runs the MedinTux configurator */
void diMainWindow::configureMedinTux()
{
    diMedinTux::configureMedinTux();
}

/**
  \brief Saves a prescription.
  \sa openPrescription()
  \sa mfDrugsIO
*/
void diMainWindow::savePrescription()
{
    QHash<QString, QString> hash;
    hash.insert("NAME", diCore::patientName().toAscii().toBase64());
    hash.insert("CLEARANCE", diCore::patientClCr().toAscii().toBase64());
    mfDrugsIO::savePrescription(hash);
}

/**
  \brief Opens a prescription saved using savePrescription().
  \sa savePrescription()
  \sa mfDrugsIO
*/
void diMainWindow::openPrescription()
{
    QString f = QFileDialog::getOpenFileName(this,
                                             tkTr(OPEN_FILE),
                                             QDir::homePath(),
                                             tr(DRUGINTERACTION_FILEFILTER) );
    if (f.isEmpty())
        return;
//    QString f = "/Users/eric/prescription.di";
    QHash<QString,QString> datas;
    if (m_PrescriptionModel->rowCount() > 0) {
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

    diCore::setPatientName( QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
    patientName->setText( QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
}

/**
  \brief Slot connected to the tkUpdateChecker::updateFounded()
  Add a menu to the menu bar and connect the only action to the update dialog.
  \sa mfCore::init()
*/
void diMainWindow::on_updateFounded()
{
    // add a new menu to menuBar
    tkActionManager *am = tkActionManager::instance();
    menuBar()->addMenu(am->createMenu(M_UPDATE, MENUBAR, M_UPDATE_TEXT, "", this));
    am->appendGroup(G_UPDATE, M_UPDATE);
    am->createAction(A_VIEWUPDATE, G_UPDATE, this);
    am->setActionDatas(A_VIEWUPDATE, VIEWUPDATE_TEXT, VIEWUPDATE_TEXT, ICONSOFTWAREUPDATEAVAILABLE);
    am->refreshMenu(M_UPDATE);
    QAction *a = am->action(A_VIEWUPDATE);
    connect(a, SIGNAL(triggered()), diCore::updateChecker(), SLOT(showUpdateInformations()));
}

void diMainWindow::on_patientName_textChanged(const QString &text)
{
    diCore::setPatientName(text);
}
