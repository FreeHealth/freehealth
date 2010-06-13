/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#include "appconfigwizard.h"
#include "virtualdatabasepreferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>

#include <formmanagerplugin/formfilesselectorwidget.h>

#include <usermanagerplugin/widgets/userwizard.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>

#include "ui_mainwindowpreferenceswidget.h"


using namespace MainWin;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

AppConfigWizard::AppConfigWizard(QWidget *parent)
    : QWizard(parent)
{
    addPage(new BeginConfigPage(this));
    addPage(new CreateNewUserPage(this));
    addPage(new DatabaseConfigurationPage(this));
    addPage(new PatientFilePage(this));
    addPage(new VirtualDatabasePage(this));
    addPage(new EndConfigPage(this));
    this->setWindowTitle(tr("Application Configurator Wizard"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    //    setAttribute(Qt::WA_DeleteOnClose);
}

void AppConfigWizard::done(int r)
{
    if (r == QDialog::Rejected) {
//        bool yes = Utils::yesNoMessageBox(tr("WARNING ! You don't save this user."),
//                               tr("If you continue changes will be lost.\n"
//                                  "Do you really want to close this dialog ?"),
//                               "", tr("Data losing"));
//        if (yes)
            QDialog::done(r);
    } else {

            QDialog::done(r);
    }
}

BeginConfigPage::BeginConfigPage(QWidget *parent)
    : QWizardPage(parent)
{
    intro = new QLabel(this);
    langLabel = new QLabel(this);
    retranslate();

    QComboBox *cbLanguage = new QComboBox(this);
    cbLanguage->addItems(Core::Translators::availableLocales());
    int actual = Core::Translators::availableLocales().indexOf(QLocale().name().left(2));
    if (actual == -1) {
        cbLanguage->setCurrentIndex(0);
    } else {
        cbLanguage->setCurrentIndex(actual);
    }
    /** \todo connection here ... */
    connect(cbLanguage, SIGNAL(activated(QString)), Core::Translators::instance(), SLOT(changeLanguage(const QString &)));

    registerField("Language", cbLanguage , "currentIndex");

    QGridLayout *layout = new QGridLayout(this);
    layout->setVerticalSpacing(30);
    layout->addWidget(intro, 0, 0, 1, 1);
    layout->addWidget(langLabel, 2, 0);
    layout->addWidget(cbLanguage, 2, 1);
    setLayout(layout);
}

void BeginConfigPage::retranslate()
{
    setTitle(tr("Welcome to FreeMedForms"));
    langLabel->setText(tr("Select your language"));
    intro->setText(tr("<b>Welcome to FreeMedForms</b><br /><br />"
                      "This wizard will help you to configure the base parameters of the application.<br />"
                      "At any time, you can cancel this wizard, the default values will be activated "
                      "for the undefined parameters."));
}



CreateNewUserPage::CreateNewUserPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Create a new user"));
    setSubTitle(tr("It is recommended to create a new user instead of using the default one."));
    QPushButton *but = new QPushButton(tr("Click here to create a new user"), this);
    newUserName = new QLabel(" ", this);
    connect(but, SIGNAL(clicked()), this, SLOT(createNewUser()));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(but, 0, 0);
    layout->addWidget(newUserName, 1, 0);
    setLayout(layout);
}

void CreateNewUserPage::createNewUser()
{
    UserPlugin::UserWizard wiz(this);
    wiz.createUser(true);
    wiz.exec();
    newUserName->setText(tr("New user created."));
}

DatabaseConfigurationPage::DatabaseConfigurationPage(QWidget *parent) :
        QWizardPage(parent), m_ui(0)
{
    setTitle(tr("Update checking and database configuration"));
    setSubTitle(tr("By default, FreeMedForms is configured to use a local SQLite database. You can choose an external server."));
    m_ui = new Internal::Ui::MainWindowPreferencesWidget;
    m_ui->setupUi(this);
    m_ui->label_2->hide();
    m_ui->line->hide();
    m_ui->log->setIcon(theme()->icon(Core::Constants::ICONEYES));
    m_ui->pass->setIcon(theme()->icon(Core::Constants::ICONEYES));
    m_ui->pass->toogleEchoMode();
    m_ui->updateCheckingCombo->setCurrentIndex(settings()->value(Utils::Constants::S_CHECKUPDATE).toInt());
    m_ui->useExternalDB->setChecked(settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool());
    m_ui->host->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST).toByteArray()));
    m_ui->log->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG).toByteArray()));
    m_ui->pass->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS).toByteArray()));
    m_ui->port->setValue(QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT).toByteArray())).toInt());
}

DatabaseConfigurationPage::~DatabaseConfigurationPage()
{
    if (m_ui) {
        delete m_ui;
        m_ui = 0;
    }
}

static void saveExternalDatabase(bool use, const QString &host, const QString &log, const QString &pass, const int port)
{
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool() != use) {
        settings()->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, use);
        settings()->setValue(Core::Constants::S_EXTERNAL_DATABASE_HOST, QString(host.toAscii().toBase64()));
        settings()->setValue(Core::Constants::S_EXTERNAL_DATABASE_PORT, QString::number(port).toAscii().toBase64());
        settings()->setValue(Core::Constants::S_EXTERNAL_DATABASE_LOG, QString(log.toAscii().toBase64()));
        settings()->setValue(Core::Constants::S_EXTERNAL_DATABASE_PASS, QString(pass.toAscii().toBase64()));
        Core::ICore::instance()->databaseServerLoginChanged();
    }
}

bool DatabaseConfigurationPage::validatePage()
{
    settings()->setValue(Utils::Constants::S_CHECKUPDATE, m_ui->updateCheckingCombo->currentIndex());
    saveExternalDatabase(m_ui->useExternalDB->isChecked(), m_ui->host->text(), m_ui->log->text(), m_ui->pass->text(), m_ui->port->value());
    return true;
}

void DatabaseConfigurationPage::on_testButton_clicked()
{
    m_ui->testConnectionLabel->setText(tr("Test in progress..."));
    {
        QSqlDatabase test = QSqlDatabase::addDatabase("QMYSQL", "APPCONFIGWIZARD_TEST");
        test.setHostName(m_ui->host->text());
        test.setPort(m_ui->port->value());
        test.setUserName(m_ui->log->text());
        test.setPassword(m_ui->pass->text());
        if (!test.open()) {
            m_ui->testButton->setIcon(theme()->icon(Core::Constants::ICONERROR));
            m_ui->testConnectionLabel->setText(tr("Connection error: %1").arg(test.lastError().number()));
            m_ui->testConnectionLabel->setToolTip(test.lastError().text());
        } else {
            m_ui->testButton->setIcon(theme()->icon(Core::Constants::ICONOK));
            m_ui->testConnectionLabel->setText(tr("Connected"));
        }
    }
    QSqlDatabase::removeDatabase("APPCONFIGWIZARD_TEST");
}



PatientFilePage::PatientFilePage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Patients Forms File"));
    setSubTitle(tr("FreeMedForms allows you to define your own patient forms file. You can select it from here. All patients will have the same forms."));

    selector = new Form::FormFilesSelectorWidget(this);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(selector, 0, 0);
    setLayout(layout);
}

bool PatientFilePage::validatePage()
{
    return QFile(settings()->value(Core::Constants::S_PATIENTFORMS_FILENAME).toString()).exists();
}


VirtualDatabasePage::VirtualDatabasePage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Create Virtual Patients"));
    setSubTitle(tr("You can create some virtual patients and users in order to test FreeMedForms.\n"
                   "Just select the number of patients/users you want to create and click on the "
                   "'populate' button."));

    Internal::VirtualDatabasePreferences *vd = new Internal::VirtualDatabasePreferences(this);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setSpacing(0);
    l->setMargin(0);
    l->addWidget(vd);
}


EndConfigPage::EndConfigPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("FreeMedForms is now configured"));
    setSubTitle(tr("Please read the user's manual. If you have any question, you can ask them to the mailing list."));
    QLabel *lbl = new QLabel(tr("French/english mailing list : <a href=\"mailto:freemedforms@googlegroups.com\">freemedforms@googlegroups.com</a>"), this);
    lbl->setOpenExternalLinks(true);
}
