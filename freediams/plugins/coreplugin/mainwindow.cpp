/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
#include "mainwindow.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iformio.h>
#include <coreplugin/iformitem.h>
#include <coreplugin/iformwidgetfactory.h>
#include <coreplugin/formmanager.h>

#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/settingsdialog.h>
#include <coreplugin/dialogs/helpdialog.h>

#include <fdcoreplugin/coreimpl.h>
#include <fdcoreplugin/patient.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_mainwindow.h"

// include freemedforms headers
//#include <mfPrinter.h>
//#include <mfAbstractWidget.h>
//#include <mfDialogInterpret.h>
//#include <mfPluginsManager.h>
//#include <mfSettings.h>
//#include <mfAboutDialog.h>

// include toolkit headers
//#include <tkDebugDialog.h>
//#include <tkTheme.h>
//#include <tkActionManager.h>
//#include <tkContextManager.h>
//#include <tkConstantTranslations.h>

// include usertoolkit headers
//#include <tkUserManager.h>

// include Qt headers
#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>

// GlobalFormModel TESTING
#include <QTreeView>
#include <coreplugin/formglobalmodel.h>
// END

using namespace Core;
using namespace Trans::ConstantTranslations;

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent )
          : Core::IMainWindow(parent)//, fileToolBar(0), editToolBar(0),
//          m_pRootObject(0), m_MainWidget(0), m_AutomaticSaveInterval(0), m_TimerId(0),
//          m_UserManager(0)
{
    setObjectName( "Core::MainWindow" );
    recentFiles.clear();
    setWindowTitle( qApp->applicationName() );
}

bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    // create menus
    createFileMenu();
    createEditMenu();
    createFormatMenu();
    createPluginsMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
            Core::MainWindowActions::A_FileNew  |
            Core::MainWindowActions::A_FileOpen |
            Core::MainWindowActions::A_FileSave |
            Core::MainWindowActions::A_FileSaveAs |
            Core::MainWindowActions::A_FilePrint |
            Core::MainWindowActions::A_FileQuit);
    actions.setConfigurationActions(
            Core::MainWindowActions::A_AppPreferences |
            Core::MainWindowActions::A_PluginsPreferences |
            Core::MainWindowActions::A_LangageChange |
            Core::MainWindowActions::A_ConfigureMedinTux);
    actions.setHelpActions(
            Core::MainWindowActions::A_AppAbout |
            Core::MainWindowActions::A_AppHelp |
            Core::MainWindowActions::A_DebugDialog |
            Core::MainWindowActions::A_QtAbout);
    actions.createEditActions(true);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

//    Core::ActionManager::instance()->actionContainer(Constants::MENUBAR)->appendGroup(mfDrugsConstants::G_PLUGINS_DRUGS);

    readSettings();

    m_ui->setupUi(this);

    m_ui->morePatientInfoButton->setIcon(Core::ICore::instance()->theme()->icon(Constants::ICONADD) );
    m_ui->patientInformations->hide();
//    refreshPatient();

//    Drugs::::Internal::DrugsManager::instance();
    m_ui->m_CentralWidget->initialize();
//    Drugs::Internal::DrugsManager::instance()->setCurrentView(m_ui->m_CentralWidget);

    Core::ICore::instance()->contextManager()->updateContext();
    Core::ICore::instance()->actionManager()->retranslateMenusAndActions();

    setWindowTitle( qApp->applicationName() + " - " + qApp->applicationVersion() );

//    m_CentralWidget->setFocus();
    return true;
}

void MainWindow::extensionsInitialized()
{
    show();
}

MainWindow::~MainWindow()
{
}

/**
  \brief Refresh the ui data refering to the patient
  \sa Core::Internal::CoreImpl::instance()->patient(), diPatient
*/
void MainWindow::refreshPatient() const
{
    m_ui->patientName->setText( Core::Internal::CoreImpl::instance()->patient()->value(Patient::FullName).toString() );
    m_ui->patientName->setToolTip( QString("Nom : %1<br />Date de naissance : %2<br />Poids : %3<br />"
                                     "Taille : %4<br />Clearance : %5")
                             .arg( Core::Internal::CoreImpl::instance()->patient()->value(Patient::FullName).toString(),
                                   Core::Internal::CoreImpl::instance()->patient()->value(Patient::DateOfBirth).toString(),
                                   Core::Internal::CoreImpl::instance()->patient()->value(Patient::Weight).toString() )
                             .arg( Core::Internal::CoreImpl::instance()->patient()->value(Patient::Size).toString(),
                                   Core::Internal::CoreImpl::instance()->patient()->value(Patient::CreatinClearance).toString() ));

    m_ui->patientWeight->setValue( Core::Internal::CoreImpl::instance()->patient()->value(Patient::Weight).toInt() );
    m_ui->patientSize->setValue( Core::Internal::CoreImpl::instance()->patient()->value(Patient::Size).toInt() );
    m_ui->patientClCr->setValue( Core::Internal::CoreImpl::instance()->patient()->value(Patient::CreatinClearance).toDouble() );
    m_ui->patientCreatinin->setValue( Core::Internal::CoreImpl::instance()->patient()->value(Patient::Creatinin).toDouble() );
    m_ui->listOfAllergies->setText(  Core::Internal::CoreImpl::instance()->patient()->value(Patient::DrugsAllergies).toString() );
    m_ui->sexCombo->setCurrentIndex( m_ui->sexCombo->findText(Core::Internal::CoreImpl::instance()->patient()->value(Patient::Sex).toString()) );
}

/** \brief Close the main window and the application */
void MainWindow::closeEvent( QCloseEvent *event )
{
    writeSettings();
    // TODO: here */
    // if is a medintux plugins --> save prescription to exchange file
//    if (!diMedinTux::medintuxExchangeFileName().isEmpty()) {
//        QString tmp = DrugsIO::instance()->prescriptionToHtml();
//        tmp.replace("font-weight:bold;", "font-weight:600;");
//        tkGlobal::saveStringToFile( tkGlobal::toHtmlAccent(tmp) , diMedinTux::medintuxExchangeFileName(), tkGlobal::DontWarnUser );
//    } else if (!diCore::exchangeFileName().isEmpty()) {
//        savePrescription(diCore::exchangeFileName());
//    }
    event->accept();
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
//    s->restoreState( this, MFDRUGS_SETTINGS_STATEPREFIX );
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
//    s->saveState( this, MFDRUGS_SETTINGS_STATEPREFIX );
    s->sync();
}

/** \obsolete */
void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tkTr(Trans::Constants::READY), 2000 );
}

/**
  \brief Shows the help dialog.
  \sa tkHelpDialog
*/
bool MainWindow::applicationHelp()
{
    Core::HelpDialog::showIndex();
    return true;
}

bool MainWindow::aboutPlugins()
{
    Core::PluginDialog dialog(this);
    dialog.exec();
}
/**
  \brief Open the preferences dialog
  \sa mfDrugsPreferences
*/
bool MainWindow::applicationPreferences()
{
    // TODO: here */
//    QTime t;
//    t.start();
//    Core::ISettings *s = Core::ICore::instance()->settings();
//    QDialog dlg(this);
//    QVBoxLayout l(&dlg);
//    mfDrugsPreferences prefs(&dlg);
//    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
//    connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
//    connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
//    l.addWidget( &prefs );
//    l.addWidget( &buttonBox );
//    dlg.setWindowTitle( tkTr(PREFERENCES_TEXT) + " - " + qApp->applicationName() );
//    //    tkLog::logTimeElapsed(t, this->objectName(), "OpeningPreferences" );
//    dlg.exec();
//    if ( dlg.result() == QDialog::Accepted ) {
//        prefs.saveToSettings(s);
//        changeFontTo( QFont(s->value( MFDRUGS_SETTING_VIEWFONT ).toString(), s->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt()) );
//    }
    return true;
}

/** \brief Change the font of the viewing widget */
void MainWindow::changeFontTo( const QFont &font )
{
    m_ui->m_CentralWidget->changeFontTo(font);
    m_ui->patientName->setFont(font);
}


/**
  \brief Prints the prescription using the header, footer and watermark.
  \sa tkPrinter
*/
bool MainWindow::print()
{
    return m_ui->m_CentralWidget->printPrescription();
}

/** \brief Runs the MedinTux configurator */
bool MainWindow::configureMedintux()
{
//    diMedinTux::configureMedinTux();
    return true;
}

bool MainWindow::saveAsFile()
{
    return savePrescription();
}

bool MainWindow::saveFile()
{
    return savePrescription();
}

/**
  \brief Saves a prescription. If fileName is empty, user is ask about a file name.
  \sa openPrescription()
  \sa DrugsIO
*/
bool MainWindow::savePrescription( const QString &fileName )
{
    QString xmlExtra = Core::Internal::CoreImpl::instance()->patient()->toXml();
//    return Drugs::DrugsIO::savePrescription(xmlExtra, fileName);
    // TODO: here */
    return true;
}

/**
  \brief Opens a prescription saved using savePrescription().
  \sa savePrescription()
  \sa DrugsIO
*/
bool MainWindow::openFile()
{
    QString f = QFileDialog::getOpenFileName(this,
                                             tkTr(Trans::Constants::OPEN_FILE),
                                             QDir::homePath(),
                                             tr(Constants::FREEDIAMS_FILEFILTER) );
    if (f.isEmpty())
        return false;
    //    QString f = "/Users/eric/prescription.di";
    QHash<QString,QString> datas;
//    if (DRUGMODEL->rowCount() > 0) {
//        int r = Utils::withButtonsMessageBox(
//                tr("Opening a prescription : merge or replace ?"),
//                tr("There is a prescription inside editor, do you to replace it or to add the opened prescription ?"),
//                QString(), QStringList() << tr("Replace prescription") << tr("Add to prescription"),
//                tr("Open a prescription") + " - " + qApp->applicationName());
//        if (r == 0) {
//            DrugsIO::loadPrescription(f, datas, DrugsIO::ReplacePrescription);
//        } else if (r==1) {
//            DrugsIO::loadPrescription(f, datas, DrugsIO::AppendPrescription);
//        }
//    } else
//        DrugsIO::loadPrescription(f, datas, DrugsIO::ReplacePrescription);

    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::FullName, QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
    m_ui->patientName->setText( QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
    return true;
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientName_textChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::FullName, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_sexCombo_currentIndexChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::Sex, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientWeight_valueChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::Weight, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientSize_valueChanged(const QString & text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::Size, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientClCr_valueChanged(const QString & text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::CreatinClearance, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientCreatinin_valueChanged(const QString & text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::Creatinin, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_listOfAllergies_textChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Patient::DrugsAllergies, text);
}
