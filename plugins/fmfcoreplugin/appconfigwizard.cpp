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
#include "appconfigwizard.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/ioptionspage.h>
#include <coreplugin/ifirstconfigurationpage.h>
#include <coreplugin/iuser.h>
#include <coreplugin/dialogs/serverpreferenceswidget.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <utils/widgets/languagecombobox.h>
#include <utils/database.h>

#include <extensionsystem/pluginmanager.h>

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QDesktopWidget>

using namespace Core;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::Translators *translators() { return Core::ICore::instance()->translators(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

static inline QString serverConfigurationSqlScript() {return settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/server_config/config.sql";}

namespace {

    class CoreFirstRunPage : public Core::IFirstConfigurationPage
    {
    public:
        CoreFirstRunPage(QObject *parent) : IFirstConfigurationPage(parent) {}
        int id() const {return IFirstConfigurationPage::FirstPage;}
        QWizardPage *createPage(QWidget *parent) {return new CoreConfigPage(parent);}
    };

    class CoreServerConfigPage : public Core::IFirstConfigurationPage
    {
    public:
        CoreServerConfigPage(QObject *parent) : IFirstConfigurationPage(parent) {}
        int id() const {return IFirstConfigurationPage::ServerConfig;}
        QWizardPage *createPage(QWidget *parent) {return new ServerConfigPage(parent);}
    };

    class CoreClientConfigPage : public Core::IFirstConfigurationPage
    {
    public:
        CoreClientConfigPage(QObject *parent) : IFirstConfigurationPage(parent) {}
        int id() const {return IFirstConfigurationPage::ServerClientConfig;}
        QWizardPage *createPage(QWidget *parent) {return new ClientConfigPage(parent);}
    };

    class CoreLastRunPage : public Core::IFirstConfigurationPage
    {
    public:
        CoreLastRunPage(QObject *parent) : IFirstConfigurationPage(parent) {}
        int id() const {return IFirstConfigurationPage::LastPage;}
        QWizardPage *createPage(QWidget *parent) {return new EndConfigPage(parent);}
    };
}

AppConfigWizard::AppConfigWizard(QWidget *parent)
    : QWizard(parent)
{
    // create pages
    QList<IFirstConfigurationPage *> pages = pluginManager()->getObjects<IFirstConfigurationPage>();
    pages << new ::CoreFirstRunPage(this);
    pages << new ::CoreServerConfigPage(this);
    pages << new ::CoreClientConfigPage(this);
    pages << new ::CoreLastRunPage(this);

    // add pages to wizard
    for(int i = 0 ; i < pages.count(); ++i) {
        IFirstConfigurationPage *page = pages.at(i);
        QWizardPage *p = page->createPage(this);
        setPage(page->id(), p);
    }

    // set window text
    setWindowTitle(tr("Application Configurator Wizard"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);

    QPixmap pix = theme()->splashScreenPixmap("freemedforms-wizard-first.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    // Delete the login information in settings
    settings()->setValue(Core::Constants::S_LASTLOGIN, QVariant());
    settings()->setValue(Core::Constants::S_LASTPASSWORD, QVariant());
}

void AppConfigWizard::done(int r)
{
    if (r == QDialog::Rejected) {
        /** \todo code here */
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

void AppConfigWizard::resizeEvent(QResizeEvent *event)
{
    QWizard::resizeEvent(event);
    Utils::centerWidget(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////  CoreConfigPage  /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
CoreConfigPage::CoreConfigPage(QWidget *parent) :
        QWizardPage(parent)//, m_Wizard(parent)
{
    langLabel = new QLabel(this);
    langLabel->setWordWrap(true);

    typeLabel = new QLabel(this);
    typeLabel->setWordWrap(true);

    // combo language
    combo = new Utils::LanguageComboBox(this);
    combo->setDisplayMode(Utils::LanguageComboBox::AvailableTranslations);
    combo->setTranslationsPath(settings()->path(Core::ISettings::TranslationsPath));
    combo->setFlagsIconPath(settings()->path(Core::ISettings::SmallPixmapPath));
    /** \todo connection here to retranslate (changeEvent())... */
    connect(combo, SIGNAL(currentLanguageChanged(QLocale::Language)), Core::Translators::instance(), SLOT(changeLanguage(QLocale::Language)));
    combo->setCurrentLanguage(QLocale().language());

    // combo install type
    installCombo = new QComboBox(this);

    retranslate();

    QGridLayout *layout = new QGridLayout(this);
    layout->setVerticalSpacing(30);
    layout->addWidget(langLabel, 2, 0, 1, 2);
    layout->addWidget(combo, 3, 1);
    layout->addWidget(typeLabel, 10, 0, 1, 2);
    layout->addWidget(installCombo, 11, 1);
    setLayout(layout);
}

void CoreConfigPage::retranslate()
{
    setTitle(tr("Welcome to %1").arg(qApp->applicationName() + " v" + qApp->applicationVersion()));
    setSubTitle(tr("<b>Welcome to %1</b><br /><br />"
                   "This wizard will help you to configure the base parameters "
                   "of the application.<br />"
                   "At any time, you can cancel this wizard, the default "
                   "values will be activated "
                   "for the undefined parameters.")
                .arg(qApp->applicationName() + " v" + qApp->applicationVersion()));

    langLabel->setText(tr("Select your language"));
    typeLabel->setText(tr("Select the type of installation"));

    installCombo->clear();
    installCombo->addItem(theme()->icon(Constants::ICONCOMPUTER), tr("Single computer"));
    installCombo->addItem(theme()->icon(Constants::ICONNETWORK), tr("Network (as client)"));
    installCombo->addItem(theme()->icon(Constants::ICONNETWORK), tr("Network (as server)"));
}

bool CoreConfigPage::validatePage()
{
    return true;
}

int CoreConfigPage::nextId() const
{
    switch (installCombo->currentIndex())
    {
    case 0: // No server
        return Core::IFirstConfigurationPage::UserCreation;
    case 1: // Network as client
        return Core::IFirstConfigurationPage::ServerClientConfig;
    case 2: // Network as server
        return Core::IFirstConfigurationPage::ServerConfig;
    }
    return Core::IFirstConfigurationPage::FirstPage;
}

void CoreConfigPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslate();
        break;
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  ClientConfigPage  ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ClientConfigPage::ClientConfigPage(QWidget *parent) :
        QWizardPage(parent),
        serverWidget(0)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setVerticalSpacing(30);
    serverWidget = new Core::ServerPreferencesWidget(this);
    serverWidget->setUserLoginGroupTitle(tr("Personnal login and password"));
    layout->addWidget(serverWidget, 0, 0);
    setLayout(layout);

    QPixmap pix = theme()->splashScreenPixmap("freemedforms-wizard-network.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    settings()->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, true);
}

void ClientConfigPage::retranslate()
{
    setTitle(tr("Network client configuration"));
    setSubTitle(tr("There were no automatic server configuration file found.<br />"
                   "You must configure the server manually.<br />"
                   "Use your <b>personnal login and password</b> to connect the database."));
}

bool ClientConfigPage::validatePage()
{
    if (serverWidget->connectionSucceeded()) {
        // remove log and pass from settings
        settings()->setValue(Core::Constants::S_LASTLOGIN, QString());
        settings()->setValue(Core::Constants::S_LASTPASSWORD, QString());
        return true;
    }
    return false;
}

int ClientConfigPage::nextId() const
{
    return Core::IFirstConfigurationPage::LastPage;
}

void ClientConfigPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslate();
        break;
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  ServerConfigPage  ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ServerConfigPage::ServerConfigPage(QWidget *parent) :
        QWizardPage(parent),
        serverWidget(0)
{
    setObjectName("ServerConfigPage");
    QGridLayout *layout = new QGridLayout(this);
    layout->setVerticalSpacing(30);
    serverWidget = new Core::ServerPreferencesWidget(this);
    layout->addWidget(serverWidget, 0, 0);
    setLayout(layout);

    QPixmap pix = theme()->splashScreenPixmap("freemedforms-wizard-network.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    settings()->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, true);
}

void ServerConfigPage::retranslate()
{
    setTitle(tr("Network server configuration"));
    setSubTitle(tr("There were no automatic server configuration file found.<br />"
                   "You must configure the server manually.<br />"
                   "Use the <b>server super-administrator login and password</b> to connect the database."));
    serverWidget->setUserLoginGroupTitle(tr("Server super-administrator login and password"));
}

bool ServerConfigPage::validatePage()
{
    if (!serverWidget->connectionSucceeded())
        return false;

    // get grants on database for the user
    Utils::Database::Grants grants = serverWidget->grantsOnLastConnectedDatabase();

    // if grants not suffisant -> warning dialog
    if (!((grants & Utils::Database::Grant_Select) &&
          (grants & Utils::Database::Grant_Update) &&
          (grants & Utils::Database::Grant_Insert) &&
          (grants & Utils::Database::Grant_Delete) &&
          (grants & Utils::Database::Grant_Create) &&
          (grants & Utils::Database::Grant_Drop) &&
          (grants & Utils::Database::Grant_Alter) &&
          (grants & Utils::Database::Grant_CreateUser)
          )) {
        Utils::warningMessageBox(tr("Connection to the server: User rights inadequate"),
                                 tr("You need to connect with another user that have rights to "
                                    "select, udpate, delete, insert, create, drop, alter and create user.\n"
                                    "Please contact your server administrator."));
        return false;
    }

    // execute the server configuration SQL script
    {
        QSqlDatabase test = QSqlDatabase::addDatabase("QMYSQL", "__APP_CONNECTION_TESTER");
        test.setHostName(serverWidget->hostName());
        test.setPort(serverWidget->port());
        test.setUserName(serverWidget->login());
        test.setPassword(serverWidget->password());
        test.setDatabaseName("mysql");
        if (!test.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(test.connectionName()).arg(test.lastError().text()));
            QSqlDatabase::removeDatabase("__APP_CONNECTION_TESTER");
            return false;
        }
//        qWarning() << test;
        LOG("Executing server configuration SQL script");
        Utils::Database::executeSqlFile("__APP_CONNECTION_TESTER", serverConfigurationSqlScript());
    }
    QSqlDatabase::removeDatabase("__APP_CONNECTION_TESTER");

    return true;
}

int ServerConfigPage::nextId() const
{
    return Core::IFirstConfigurationPage::UserCreation;
}

void ServerConfigPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslate();
        break;
    default:
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  EndConfigPage  ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
EndConfigPage::EndConfigPage(QWidget *parent) :
        QWizardPage(parent)//, m_Wizard(parent)
{
    setTitle(tr("%1 is now configured").arg(qApp->applicationName() + " v"+qApp->applicationVersion()));
    setSubTitle(tr("Please read the user's manual. "
                   "If you have any question, you can ask them to "
                   "the mailing list."));

    // add labels
    QVBoxLayout *l = new QVBoxLayout(this);
    setLayout(l);
    QLabel *lbl = new QLabel(tr("Default virtual patients were created (Kirk, Picard, Doe)."), this);
    lbl->setOpenExternalLinks(true);

    QLabel *lbl1 = new QLabel(tr("French/english mailing list : "
                                "<a href=\"mailto:freemedforms@googlegroups.com\">"
                                "freemedforms@googlegroups.com</a>"), this);
    lbl1->setOpenExternalLinks(true);
    QLabel *lbl2 = new QLabel(tr("Application main web site: "
                                "<a href=\"%1\">"
                                "%1</a>").arg(settings()->path(Core::ISettings::WebSiteUrl)), this);
    lbl2->setOpenExternalLinks(true);
    l->addWidget(lbl);
    l->addWidget(lbl1);
    l->addWidget(lbl2);
}

void EndConfigPage::initializePage()
{
    // Create virtual patients
    QList<Core::IOptionsPage*> pages = pluginManager()->getObjects<Core::IOptionsPage>();
    for(int i = 0; i < pages.count(); ++i) {
        if (pages.at(i)->id() == "VirtualPatientBasePage") {
            pages.at(i)->resetToDefaults();
        }
    }
}
