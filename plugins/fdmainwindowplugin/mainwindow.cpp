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
#include "mainwindow.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>

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
#include <fdcoreplugin/commandlineparser.h>
#include <fdcoreplugin/patient.h>

#include <fdmainwindowplugin/medintux.h>

#include <drugsplugin/mfDrugsConstants.h>
#include <drugsplugin/drugsmodel/mfDrugsIO.h>
#include <drugsplugin/drugsdatabase/mfDrugsBase.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_mainwindow.h"

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

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

namespace MainWin {
namespace Internal {
    static bool transmitDosage()
    {
        Utils::Log::addMessage("Core", QCoreApplication::translate("MainWindow", "Preparing dosage transmission"));
        Drugs::Internal::DrugsIO::instance()->startsDosageTransmission();
        return true;
    }
    static const char* const  SETTINGS_COUNTDOWN = "transmissionCountDown";
} // namespace Internal
} // namespace Core

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent )
          : Core::IMainWindow(parent)
{
    setObjectName("MainWindow");
//    recentFiles.clear();
}

bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    // create menus
    createFileMenu();
//    createEditMenu();
    Core::ICore::instance()->actionManager()->actionContainer(Core::Constants::MENUBAR)->appendGroup(mfDrugsConstants::G_PLUGINS_DRUGS);
//    createPluginsMenu();
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
//            Core::MainWindowActions::A_PluginsPreferences |
            Core::MainWindowActions::A_LangageChange |
            Core::MainWindowActions::A_ConfigureMedinTux);
    actions.setHelpActions(
            Core::MainWindowActions::A_AppAbout |
            Core::MainWindowActions::A_AppHelp |
            Core::MainWindowActions::A_DebugDialog |
            Core::MainWindowActions::A_QtAbout);
    actions.createEditActions(false);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

//    Core::ICore::instance()->contextManager()->updateContext();
    Core::ICore::instance()->actionManager()->retranslateMenusAndActions();

    readSettings();

    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());

    return true;
}

void MainWindow::extensionsInitialized()
{
    Core::CommandLine *cl = Core::ICore::instance()->commandLine();
    Core::ISettings *s = Core::ICore::instance()->settings();

    // Update countdown to dosage transmission
    int count = s->value(Internal::SETTINGS_COUNTDOWN,0).toInt();
    ++count;
    if ((count==30) || (cl->value(Core::CommandLine::CL_TransmitDosage).toBool())) {
        s->setValue(Internal::SETTINGS_COUNTDOWN,0);
        transmitDosage();
    } else {
        s->setValue(Internal::SETTINGS_COUNTDOWN,count);
    }

    // Initialize drugs database
    Drugs::Internal::DrugsBase::instance();

    // Creating MainWindow interface
    m_ui = new Internal::Ui::MainWindow();
    m_ui->setupUi(this);

    m_ui->morePatientInfoButton->setIcon(Core::ICore::instance()->theme()->icon(Core::Constants::ICONADD) );
    m_ui->patientInformations->hide();
    refreshPatient();

    m_ui->m_CentralWidget->initialize();
//    Drugs::Internal::DrugsManager::instance()->setCurrentView(m_ui->m_CentralWidget);

    setWindowTitle(qApp->applicationName());
    show();

    // If needed read exchange file
    const QString &exfile = cl->value(Core::CommandLine::CL_ExchangeFile).toString();
    if (!exfile.isEmpty()) {
        if (cl->value(Core::CommandLine::CL_MedinTux).toBool()) {
            Utils::Log::addMessage(this, tr( "Reading a MedinTux exchange file." ) );
            QString tmp = Utils::readTextFile(exfile, Utils::DontWarnUser);
            if (tmp.contains(mfDrugsConstants::ENCODEDHTML_FREEDIAMSTAG)) {
                int begin = tmp.indexOf(mfDrugsConstants::ENCODEDHTML_FREEDIAMSTAG) + QString(mfDrugsConstants::ENCODEDHTML_FREEDIAMSTAG).length();
                int end = tmp.indexOf("\"", begin);
                QString encoded = tmp.mid( begin, end - begin );
                Drugs::Internal::DrugsIO::instance()->prescriptionFromXml( QByteArray::fromBase64( encoded.toAscii() ) );
            } else if (tmp.contains("DrugsInteractionsEncodedPrescription:")) {
                /** \todo Manage wrong file encoding */
                int begin = tmp.indexOf("DrugsInteractionsEncodedPrescription:") + QString("DrugsInteractionsEncodedPrescription:").length();
                int end = tmp.indexOf("\"", begin);
                QString encoded = tmp.mid( begin, end - begin );
                Drugs::Internal::DrugsIO::instance()->prescriptionFromXml( QByteArray::fromBase64( encoded.toAscii() ) );
            }
        } else {
            QString extras;
            Drugs::Internal::DrugsIO::loadPrescription(exfile, extras);
            Core::ICore::instance()->patient()->fromXml(extras);
        }
    }

    Utils::Log::addMessage(this , tkTr(Trans::Constants::RAISING_APPLICATION) );
    raise();
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
    Core::Patient *p = Core::Internal::CoreImpl::instance()->patient();
    m_ui->patientName->setText( p->value(Core::Patient::FullName).toString() );
    m_ui->patientName->setToolTip( QString("Nom : %1<br />Date de naissance : %2<br />Poids : %3<br />"
                                     "Taille : %4<br />Clearance : %5")
                             .arg( p->value(Core::Patient::FullName).toString(),
                                   p->value(Core::Patient::DateOfBirth).toString(),
                                   p->value(Core::Patient::Weight).toString() )
                             .arg( p->value(Core::Patient::Size).toString(),
                                   p->value(Core::Patient::CreatinClearance).toString() ));

    m_ui->patientWeight->setValue( p->value(Core::Patient::Weight).toInt() );
    m_ui->patientSize->setValue( p->value(Core::Patient::Size).toInt() );
    m_ui->patientClCr->setValue( p->value(Core::Patient::CreatinClearance).toDouble() );
    m_ui->patientCreatinin->setValue( p->value(Core::Patient::Creatinin).toDouble() );
    m_ui->listOfAllergies->setText(  p->value(Core::Patient::DrugsAllergies).toString() );
    m_ui->sexCombo->setCurrentIndex( m_ui->sexCombo->findText(p->value(Core::Patient::Sex).toString()) );
}

/** \brief Close the main window and the application */
void MainWindow::closeEvent( QCloseEvent *event )
{
    Utils::Log::addMessage(this, "Closing MainWindow");
    writeSettings();
    Core::CommandLine *cl = Core::ICore::instance()->commandLine();
    QString exfile = cl->value(Core::CommandLine::CL_ExchangeFile).toString();
    if (exfile.isEmpty()) {
        event->accept();
        return;
    }
    if (!QFile(exfile).exists()) {
        Utils::Log::addError(this,tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(exfile));
    }
    Utils::Log::addMessage(this, QString("Exchange File : %1 ").arg(exfile));
    Utils::Log::addMessage(this, QString("Running as MedinTux plug : %1 ").arg(cl->value(Core::CommandLine::CL_MedinTux).toString()));
    // if is a medintux plugins --> save prescription to exchange file
    if (cl->value(Core::CommandLine::CL_MedinTux).toBool()) {
        QString tmp = Drugs::Internal::DrugsIO::instance()->prescriptionToHtml();
        tmp.replace("font-weight:bold;", "font-weight:600;");
        Utils::saveStringToFile( Utils::toHtmlAccent(tmp) , exfile, Utils::DontWarnUser );
    } else {
        savePrescription(exfile);
    }
    event->accept();
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
    s->restoreState( this, mfDrugsConstants::MFDRUGS_SETTINGS_STATEPREFIX );
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
    s->saveState( this, mfDrugsConstants::MFDRUGS_SETTINGS_STATEPREFIX );
    s->sync();
}

/** \obsolete */
void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tkTr(Trans::Constants::READY), 2000 );
}

bool MainWindow::newFile()
{
    /** \todo new prescription */
    return true;
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
    return true;
}
/**
  \brief Open the preferences dialog
  \sa mfDrugsPreferences
*/
bool MainWindow::applicationPreferences()
{
    /** \todo here */
    Core::SettingsDialog dlg(this);
    dlg.exec();
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
    Internal::configureMedinTux();
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
    /** \todo here */
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
                                             tr(Core::Constants::FREEDIAMS_FILEFILTER) );
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
    Core::Patient *p = Core::Internal::CoreImpl::instance()->patient();
    p->setValue(Core::Patient::FullName, QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
    m_ui->patientName->setText( QByteArray::fromBase64(datas.value("NAME").toAscii() ) );
    return true;
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientName_textChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Core::Patient::FullName, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_sexCombo_currentIndexChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Core::Patient::Sex, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientWeight_valueChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Core::Patient::Weight, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientSize_valueChanged(const QString & text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Core::Patient::Size, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientClCr_valueChanged(const QString & text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Core::Patient::CreatinClearance, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientCreatinin_valueChanged(const QString & text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Core::Patient::Creatinin, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_listOfAllergies_textChanged(const QString &text)
{
    Core::Internal::CoreImpl::instance()->patient()->setValue(Core::Patient::DrugsAllergies, text);
}
