/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
#include <fmfcoreplugin/commandlineparser.h>
#include <coreplugin/dialogs/networkpreferences.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <utils/database.h>
#include <utils/databaseconnector.h>
#include <utils/widgets/languagecombobox.h>
#include <utils/widgets/pathchooser.h>

#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_user.h>
#include <translationutils/trans_current.h>

#include <extensionsystem/pluginmanager.h>

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QTimer>
#include <QProgressDialog>
#include <QNetworkProxyQuery>
#include <QNetworkProxy>

using namespace Core;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }
static inline QString serverConfigurationSqlScript() {return settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/server_config/config.sql";}

namespace {

//static void noBackButton(QWizard *wizard)
//{
//    wizard->setButtonLayout(QList<QWizard::WizardButton>() << QWizard::HelpButton << QWizard::Stretch << QWizard::CancelButton << QWizard::NextButton);
//    wizard->button(QWizard::BackButton)->setEnabled(false);
//}

    const char* const FIELD_TYPEOFINSTALL = "typeOfInstall";

    // Keep these constants sync with the AppConfigWizard typeOfInstall combo order
    enum typeOfInstallComboOrder {
        INSTALLING_MYSQL_SERVER = 0,
        INSTALLING_MYSQL_CLIENT,
        INSTALLING_SQLITE
    };

    class CoreFirstRunPage : public Core::IFirstConfigurationPage
    {
    public:
        CoreFirstRunPage(QObject *parent) : IFirstConfigurationPage(parent) {}
        int id() const {return IFirstConfigurationPage::FirstPage;}
        QWizardPage *createPage(QWidget *parent) {return new CoreConfigPage(parent);}
    };

    class ProxyFirstRunPage : public Core::IFirstConfigurationPage
    {
    public:
        ProxyFirstRunPage(QObject *parent) : IFirstConfigurationPage(parent) {}
        int id() const {return IFirstConfigurationPage::ProxyConfig;}
        QWizardPage *createPage(QWidget *parent) {return new ProxyPage(parent);}
    };

    class CoreFirstRunDatabaseCreationPage : public Core::IFirstConfigurationPage
    {
    public:
        CoreFirstRunDatabaseCreationPage(QObject *parent) : IFirstConfigurationPage(parent) {}
        int id() const {return IFirstConfigurationPage::DatabaseCreationPage;}
        QWizardPage *createPage(QWidget *parent) {return new CoreDatabaseCreationPage(parent);}
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

    static bool configureServer(const Utils::DatabaseConnector &connector)
    {
        // Create QSqlDatabase and open mysql database
        const QString connection = Utils::createUid();
        QSqlDatabase mysql = QSqlDatabase::addDatabase("QMYSQL", connection);
        mysql.setHostName(connector.host());
        mysql.setPort(connector.port());
        mysql.setUserName(connector.clearLog());
        mysql.setPassword(connector.clearPass());
        mysql.setDatabaseName("mysql");
        if (!mysql.open()) {
            LOG_FOR("AppConfigWzard::configureServer",
                    tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                    .arg(mysql.connectionName()).arg(mysql.lastError().text()));
            QSqlDatabase::removeDatabase(connection);
            return false;
        }

        // get grants on database for the user
        Utils::Database::Grants grants = Utils::Database::getConnectionGrants(connection);

        // if grants not sufficient -> warning dialog
        if (!((grants & Utils::Database::Grant_Select) &&
              (grants & Utils::Database::Grant_Update) &&
              (grants & Utils::Database::Grant_Insert) &&
              (grants & Utils::Database::Grant_Delete) &&
              (grants & Utils::Database::Grant_Create) &&
              (grants & Utils::Database::Grant_Drop) &&
              (grants & Utils::Database::Grant_Alter) &&
              (grants & Utils::Database::Grant_CreateUser)
              )) {
            Utils::warningMessageBox(QApplication::translate("Core::ServerConfigPage",
                                                             "Connection to the server: User privileges inadequate"),
                                     QApplication::translate("Core::ServerConfigPage",
                                                             "You need to connect with another user that has the following global privileges: "
                                                             "select, udpate, delete, insert, create, drop, alter and create user.\n"
                                                             "Please contact your server administrator."));
            QSqlDatabase::removeDatabase(connection);
            return false;
        }

        // Server is already configured (there is one fmf_admin user registered)
        QSqlQuery query(mysql);
        if (query.exec("SELECT * FROM `user` where User='fmf_admin';")) {
            if (query.next()) {
                LOG_FOR("AppConfigWzard::configureServer",
                        "Server already configured");
                Utils::informativeMessageBox(QApplication::translate("Core::ServerConfigPage", "Server already configured"),
                                             QApplication::translate("Core::ServerConfigPage", "The server is already configured for FreeHealth."));
                query.finish();
                QSqlDatabase::removeDatabase(connection);
                return true;
            }
        }

        // Configure server using global_resources server script
        LOG_FOR("AppConfigWzard::configureServer",
                "Executing server configuration SQL script");

        // Get original file
        QString sqlCommands = Utils::readTextFile(serverConfigurationSqlScript());
        // Replace __PREFIX__ with the current user database prefix
        QString prefix = connector.globalDatabasePrefix();
        // Always escape '_' for MySQL commands
        prefix = prefix.replace("_", "\\_");
        sqlCommands = sqlCommands.replace("__PREFIX__", prefix);

        // Execute SQL commands
        if (!Utils::Database::executeSQL(sqlCommands, mysql)) {
            LOG_ERROR_FOR("AppConfigWzard::configureServer",
                          "Server configuration script not processed");
            Utils::warningMessageBox(QApplication::translate("Core::ServerConfigPage", "An error occured..."),
                                     QApplication::translate("Core::ServerConfigPage",
                                                             "An error occured when trying to execute the script configuration script.\n"
                                                             "Please check out the log files and contact your administrator."),
                                     "");
            QSqlDatabase::removeDatabase(connection);
            return false;
        }
        LOG_FOR("AppConfigWzard::configureServer",
                "Server successfully configured");
        Utils::informativeMessageBox(QApplication::translate("Core::ServerConfigPage",
                                                             "Server configured"),
                                     QApplication::translate("Core::ServerConfigPage",
                                                             "The server was successfully configured."));
        QSqlDatabase::removeDatabase(connection);
        return true;
    }


}

AppConfigWizard::AppConfigWizard(QWidget *parent) :
    QWizard(parent)
{
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
    // create pages
    QList<IFirstConfigurationPage *> pages = pluginManager()->getObjects<IFirstConfigurationPage>();
    pages << new ::CoreFirstRunPage(this);
    pages << new ::ProxyFirstRunPage(this);
    pages << new ::CoreServerConfigPage(this);
    pages << new ::CoreClientConfigPage(this);
    pages << new ::CoreFirstRunDatabaseCreationPage(this);
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

    QPixmap pix = theme()->splashScreenPixmap("wizard-first.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    // Delete the login information in settings
    settings()->setValue(Core::Constants::S_LASTLOGIN, QVariant());
    settings()->setValue(Core::Constants::S_LASTPASSWORD, QVariant());

    Utils::centerWidget(this, qApp->desktop());
}

void AppConfigWizard::done(int r)
{
    if (r == QDialog::Rejected) {
        // TODO: code here
        //        bool yes = Utils::yesNoMessageBox(tr("WARNING! You did not save this user."),
        //                               tr("If you continue without saving, changes will be lost."
        //                                  "Do you really want to close this dialog?"),
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
    Utils::centerWidget(this, qApp->desktop());
}

void AppConfigWizard::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        setWindowTitle(tr("Application Configurator Wizard"));
    }
    QWizard::changeEvent(event);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////  CoreConfigPage  /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
CoreConfigPage::CoreConfigPage(QWidget *parent) :
    QWizardPage(parent),
    _proxyDectectionDone(false), _proxyDetected(false)
{
    setObjectName("FirstRun::CoreConfigPage");
    langLabel = new QLabel(this);
    langLabel->setWordWrap(true);

    typeLabel = new QLabel(this);
    typeLabel->setWordWrap(true);

    // combo language
    combo = new Utils::LanguageComboBox(this);
    combo->setDisplayMode(Utils::LanguageComboBox::AvailableTranslations);
    combo->setTranslationsPath(settings()->path(Core::ISettings::TranslationsPath));
    combo->setFlagsIconPath(settings()->path(Core::ISettings::SmallPixmapPath));
    connect(combo, SIGNAL(currentLanguageChanged(QLocale::Language)), Core::Translators::instance(), SLOT(changeLanguage(QLocale::Language)));
    combo->setCurrentLanguage(QLocale().language());

    // combo install type
    installCombo = new QComboBox(this);

    QGridLayout *layout = new QGridLayout(this);
    layout->setVerticalSpacing(30);
    layout->addWidget(langLabel, 2, 0, 1, 2);
    layout->addWidget(combo, 3, 1);
    layout->addWidget(typeLabel, 10, 0, 1, 2);
    layout->addWidget(installCombo, 11, 1);
    setLayout(layout);

    registerField(::FIELD_TYPEOFINSTALL, installCombo, "currentIndex");

    retranslate();
    QObject::connect(installCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     this, &CoreConfigPage::sqliteWarn);
}

void CoreConfigPage::retranslate()
{
    setTitle(tr("Welcome to %1").arg(qApp->applicationName() + " v" + qApp->applicationVersion()));
    setSubTitle(tr("This wizard will help you configure the base parameters "
                   "of the application.\n"
                   "Select your preferred language and the installation type."));

    langLabel->setText(tr("Select your language"));
    typeLabel->setText(tr("Select the type of installation"));

    installCombo->clear();
    // Keep typeOfInstall combo order sync with the constants:
    // INSTALLING_MYSQL_SERVER, INSTALLING_MYSQL_CLIENT, INSTALLING_SQLITE
    if (QSqlDatabase::drivers().contains("QMYSQL")) {
        // FIXME: test if mysql-client/mysql-server is available on this machine
        installCombo->addItem(theme()->icon(Constants::ICONNETWORK), tr("Create MySQL databases"));
        installCombo->addItem(theme()->icon(Constants::ICONNETWORK), tr("Connect to MySQL databases"));
    }
    installCombo->addItem(theme()->icon(Constants::ICONCOMPUTER), tr("Test without MySQL"));
}

bool CoreConfigPage::validatePage()
{
    setField(::FIELD_TYPEOFINSTALL, installCombo->currentIndex());
    switch (installCombo->currentIndex()) {
    case INSTALLING_SQLITE:
    {
        // Define the default database connector for the SQLite version
        Utils::DatabaseConnector connector;
        connector.setClearLog("fmf_admin");
        connector.setClearPass("fmf_admin");
        connector.setDriver(Utils::Database::SQLite);
        connector.setAccessMode(Utils::DatabaseConnector::ReadWrite);
        // Paths are automatically informed by settings()
        settings()->setDatabaseConnector(connector);
        break;
    }
    case INSTALLING_MYSQL_CLIENT:
    case INSTALLING_MYSQL_SERVER:
        break;
    }
    return true;
}

int CoreConfigPage::nextId() const
{
    // If a proxy is found -> ProxyPage
    if (!_proxyDectectionDone) {
        _proxyDectectionDone = true;
        LOG("Trying to find system proxy.");
        QNetworkProxyQuery npq(QUrl("http://www.google.com"));
        QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
        foreach(const QNetworkProxy &p, listOfProxies) {
            if (p.type()==QNetworkProxy::HttpProxy && !p.hostName().isEmpty()) {
                LOG("Proxy detected: " + p.hostName());
                _proxyDetected = true;
            }
        }
    }

    if (_proxyDetected)
        return Core::IFirstConfigurationPage::ProxyConfig;

    // Fallback to install procedure
    switch (installCombo->currentIndex())
    {
    case INSTALLING_SQLITE:
        return Core::IFirstConfigurationPage::DatabaseCreationPage;
    case INSTALLING_MYSQL_CLIENT:
        return Core::IFirstConfigurationPage::ServerClientConfig;
    case INSTALLING_MYSQL_SERVER:
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

void CoreConfigPage::sqliteWarn(int i)
{
    if (i==INSTALLING_SQLITE) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Test FreeMedForms without MySQL");
        msgBox.setInformativeText(tr("<b>This simplified installation procedure is"
                                     " for testing purposes only.</b> \n"
                                     "It is using SQLite as a temporary"
                                     " database. We do not recommend that you use"
                                     " SQLite to store medical data because "
                                     " FreeHealth support of SQLite will end soon. If you want to"
                                     " install FreeHealth for professional use,"
                                     " please choose <b>Create MySQL databases</b> instead.\n"
                                     "Click Ok to test FreeHealth, or click Cancel"
                                     " to install FreeHealth with MySQL."));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
          case QMessageBox::Ok:
              // Save was clicked
              break;
          case QMessageBox::Cancel:
              installCombo->setCurrentIndex(INSTALLING_MYSQL_SERVER);
              break;
          default:
              // should never be reached
              break;
        }
    }
    return;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  ProxyPage  ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ProxyPage::ProxyPage(QWidget *parent) :
        QWizardPage(parent)
{
    _proxyWidget = new Internal::ProxyPreferencesWidget(this);
    _proxyWidget->autoDetectProxy();

    retranslate();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_proxyWidget);
    setLayout(layout);
}

void ProxyPage::retranslate()
{
    setTitle(tr("Proxy definition"));
    setSubTitle(tr("%1 has detected a network proxy on your computer. \n"
                   "Please check and validate the proxy settings.")
                .arg(qApp->applicationName()));
}

bool ProxyPage::validatePage()
{
    _proxyWidget->saveToSettings();
    return true;
}

int ProxyPage::nextId() const
{
    switch (field(::FIELD_TYPEOFINSTALL).toInt())
    {
    case INSTALLING_SQLITE:
        return Core::IFirstConfigurationPage::DatabaseCreationPage;
    case INSTALLING_MYSQL_CLIENT:
        return Core::IFirstConfigurationPage::ServerClientConfig;
    case INSTALLING_MYSQL_SERVER:
        return Core::IFirstConfigurationPage::ServerConfig;
    }
    return Core::IFirstConfigurationPage::FirstPage;
}

void ProxyPage::changeEvent(QEvent *e)
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
    serverWidget->setUserLoginGroupTitle(Trans::Constants::CONSTANTS_TR_CONTEXT, Trans::Constants::ENTER_YOUR_PERSONAL_LOGIN);
    layout->addWidget(serverWidget, 0, 0);
    setLayout(layout);

    QPixmap pix = theme()->splashScreenPixmap("wizard-network.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    settings()->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, true);

    connect(serverWidget, SIGNAL(userConnectionChanged(bool)), this, SIGNAL(completeChanged()));
    connect(serverWidget, SIGNAL(hostConnectionChanged(bool)), this, SIGNAL(completeChanged()));
}

void ClientConfigPage::initializePage()
{
//    noBackButton(wizard());
}

void ClientConfigPage::retranslate()
{
    setTitle(tr("Network client configuration"));
    setSubTitle(tr("You must configure the server using your personnal login and password to connect the database."));
}

bool ClientConfigPage::isComplete() const
{
    return serverWidget->connectionSucceeded();
}

bool ClientConfigPage::validatePage()
{
    if (!serverWidget->connectionSucceeded())
        return false;

    // Test server credentials
    settings()->setValue(Core::Constants::S_LASTLOGIN, QString());
    settings()->setValue(Core::Constants::S_LASTPASSWORD, QString());

    // try to connect the MySQL server and test existence of a FreeMedForms configuration
    Utils::DatabaseConnector connector = settings()->databaseConnector();
    const QString connection = Utils::createUid();
    QSqlDatabase mysql = QSqlDatabase::addDatabase("QMYSQL", connection);
    mysql.setHostName(connector.host());
    mysql.setPort(connector.port());
    mysql.setUserName(connector.clearLog());
    mysql.setPassword(connector.clearPass());
    mysql.setDatabaseName("mysql");
    if (!mysql.open()) {
        LOG_ERROR("Unable to connect to MySQL databases");
        Q_EMIT completeChanged();
        return false;
    }

    // Test server configuration
    // all databases are prefixed with fmf_
    // test the fmf_* databases existence
    QSqlQuery query(mysql);
    int tries = 0;
    bool ok = false;
    QString prefix;

    while (tries < 4) {
        tries++;
        // Get the database prefix to use
        prefix = Utils::askUser(tr("Database global prefix"), tr("Please specify the database prefix to use (please ask your software administrator)"));
        if (!query.exec(QString("show databases like '%1fmf_%';").arg(prefix))) {
            LOG_QUERY_ERROR(query);
            Q_EMIT completeChanged();
            return false;
        }
        if (query.size() < 5) {
            ok = false;
            Utils::warningMessageBox(tr("No server configuration detected"),
                                     tr("You are trying to configure a network client. "
                                        "It is manadatory to connect to a network server.\n"
                                        "While the host connection is valid, no configuration was "
                                        "found on this host.\n\n"
                                        "Please check that this host contains a server configuration."));
            LOG_ERROR("No configuration detected on the server");
        } else {
            ok = true;
            query.finish();
            break;
        }
        query.finish();
    }
    if (!ok) {
        Q_EMIT completeChanged();
        return false;
    }

    // Inform connector of the prefix
    connector.setGlobalDatabasePrefix(prefix);
    settings()->setDatabaseConnector(connector);

    // Connect databases
    QProgressDialog dlg(tkTr(Trans::Constants::CONNECTING_DATABASE),
                        tkTr(Trans::Constants::PLEASE_WAIT), 0, 0);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setMinimumDuration(1000);
    dlg.show();
    dlg.setFocus();
    dlg.setValue(0);

    Core::ICore::instance()->requestFirstRunDatabaseCreation();

    return true;
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
    serverWidget->showUseDefaultAdminLogCheckbox(false);
    serverWidget->setUserLoginGroupTitle(Trans::Constants::CONSTANTS_TR_CONTEXT, Trans::Constants::ENTER_MYSQL_ADMIN_LOGIN);
    layout->addWidget(serverWidget, 0, 0);
    setLayout(layout);

    QPixmap pix = theme()->splashScreenPixmap("wizard-network.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    settings()->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, true);

    connect(serverWidget, SIGNAL(userConnectionChanged(bool)), this, SIGNAL(completeChanged()));
    connect(serverWidget, SIGNAL(hostConnectionChanged(bool)), this, SIGNAL(completeChanged()));
}

void ServerConfigPage::initializePage()
{
    if (!QFileInfo(serverConfigurationSqlScript()).exists()) {
        Utils::warningMessageBox(tr("Missing files."),
                                 tr("The configuration script is missing. You can not "
                                    "configure the server without this script.<br><br>"
                                    "<b>%1</b>").arg(tkTr(Trans::Constants::CONTACT_DEV_TEAM)));
    }
}


void ServerConfigPage::retranslate()
{
    setTitle(tr("Network server configuration"));
    setSubTitle(tr("You must configure the server using the server super-administrator login and password to connect the database."));
}

bool ServerConfigPage::isComplete() const
{
    return serverWidget->connectionSucceeded();
}

bool ServerConfigPage::validatePage()
{
    if (!serverWidget->connectionSucceeded())
        return false;
    return true;
}

int ServerConfigPage::nextId() const
{
    return Core::IFirstConfigurationPage::DatabaseCreationPage;
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
/////////////////////////////////  CoreDatabaseCreationPage  //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
CoreDatabaseCreationPage::CoreDatabaseCreationPage(QWidget *parent) :
    QWizardPage(parent),
    _progressBar(0),
    _prefixLbl(0),
    _sqlitePathLbl(0),
    _sqlitePath(0),
    _prefix(0),
    _createBaseButton(0),
    _completed(false)
{
    setObjectName("CoreDatabaseCreationPage");
    layout = new QGridLayout(this);
    layout->setVerticalSpacing(30);
    setLayout(layout);

    _prefixLbl = new QLabel(this);
    _prefix = new QLineEdit(this);
    layout->addWidget(_prefixLbl, 2, 0, 1, 2);
    layout->addWidget(_prefix, 3, 1);

    _createBaseButton = new QPushButton(this);
    connect(_createBaseButton, SIGNAL(clicked()), this, SLOT(startDbCreation()));
    layout->addWidget(_createBaseButton, 20, 1);

    _progressBar = new QProgressBar(this);
    _progressBar->setRange(0, 1);
    _progressBar->setValue(0);
    layout->addWidget(_progressBar, 25, 1);

    retranslate();
}

void CoreDatabaseCreationPage::initializePage()
{
    retranslate();
    if (field(::FIELD_TYPEOFINSTALL).toInt() == INSTALLING_SQLITE) {
        _sqlitePathLbl = new QLabel(this);
        _sqlitePath = new Utils::PathChooser(this);
        _sqlitePath->setExpectedKind(Utils::PathChooser::Directory);
        layout->addWidget(_sqlitePathLbl, 10, 0, 1, 2);
        layout->addWidget(_sqlitePath, 11, 1);
    }
}

void CoreDatabaseCreationPage::startDbCreation()
{
    if (_completed)
        return;
    _progressBar->setRange(0, 0);
    _progressBar->setValue(0);

    // Catch path for SQLite databases and global database prefix
    Utils::DatabaseConnector c = settings()->databaseConnector();
    if (field(::FIELD_TYPEOFINSTALL).toInt() == INSTALLING_SQLITE) {
        if (!_sqlitePath->path().isEmpty())
            c.setAbsPathToReadWriteSqliteDatabase(_sqlitePath->path());
    }
    if (!_prefix->text().isEmpty())
        c.setGlobalDatabasePrefix(_prefix->text());
    // Store database settings to the app settings
    settings()->setDatabaseConnector(c);

    // If selected installation mode is "MySQL server" -> configure the server
    if (field(::FIELD_TYPEOFINSTALL).toInt() == INSTALLING_MYSQL_SERVER) {
        LOG("Preparing server configuration before creating databases");
        if (!configureServer(settings()->databaseConnector())) {
            LOG_ERROR("Unable to configure MySQL server");
            return;
        }
    }

    Core::ICore::instance()->requestFirstRunDatabaseCreation();
    _completed = true;
    Q_EMIT completeChanged();
    qApp->processEvents();
    wizard()->next();
    qApp->processEvents();
}

bool CoreDatabaseCreationPage::isComplete() const
{
    return _completed;
}

bool CoreDatabaseCreationPage::validatePage()
{
    return _completed;
}

int CoreDatabaseCreationPage::nextId() const
{
    return Core::IFirstConfigurationPage::UserCreation;
}

void CoreDatabaseCreationPage::retranslate()
{
    setTitle(tr("Preparing databases"));
    setSubTitle(tr("Preparing databases. Please wait..."));
    _prefixLbl->setText(tr("Use this prefix for all databases<br><i>&nbsp;&nbsp;Optional, you can safely leave this editor empty</i>"));
    _prefix->setToolTip(tr("If you define a global prefix, all database will be named {YourPrefix}{DatabaseName} for all configuration."));
    if (field(::FIELD_TYPEOFINSTALL).toInt() == INSTALLING_MYSQL_SERVER)
        _createBaseButton->setText(tr("Configure the server and create all database"));
    else
        _createBaseButton->setText(tr("Create all database"));
    if (_sqlitePathLbl)
        _sqlitePathLbl->setText(tr("Select the path where to store your personal databases<br><i>&nbsp;&nbsp;Optional, you can safely leave this editor empty</i>"));
    if (_sqlitePath)
        _sqlitePath->setToolTip(tr("You can store your local personal database anywhere you want, just select a path."));
}

void CoreDatabaseCreationPage::changeEvent(QEvent *e)
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
    QWizardPage(parent),
    lblDb(0),
    comboDb(0),
    lblVirtual(0),
    comboVirtual(0)
{
    QGridLayout *l = new QGridLayout(this);
    setLayout(l);

    // Ask for database renewal
    if (Utils::isDebugWithoutInstallCompilation()) {
        // Database renew management
        lblDb = new QLabel(this);
        lblDb->setWordWrap(true);
        lblDb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        comboDb = new QComboBox(this);
        comboDb->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        l->addWidget(lblDb, 0, 0, 1, 2);
        l->addWidget(comboDb, 1, 1);
    }

    // Virtual data management
    lblVirtual = new QLabel(this);
    lblVirtual->setWordWrap(true);
    lblVirtual->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    comboVirtual = new QComboBox(this);
    comboVirtual->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    l->addWidget(lblVirtual, 3, 0, 1, 2);
    l->addWidget(comboVirtual, 4, 1);

    // add infos
    label_message = new QLabel(this);
    label_message->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    label_url = new QLabel(this);
    label_url->setOpenExternalLinks(true);
    label_url->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    l->addWidget(label_message, 5, 0, 1, 2);
    l->addWidget(label_url, 6, 1);

    retranslate();
    // Set indexes to combos
    if (comboDb) {
        connect(comboDb, SIGNAL(activated(int)), this, SLOT(comboDbActivated(int)));
        if (commandLine()->value(Core::ICommandLine::ClearUserDatabases, false).toBool()) {
            comboDb->setCurrentIndex(1);
        } else {
            comboDb->setCurrentIndex(0);
        }
    }
    if (comboVirtual) {
        connect(comboVirtual, SIGNAL(activated(int)), this, SLOT(comboVirtualActivated(int)));
        if (commandLine()->value(Core::ICommandLine::CreateVirtuals, false).toBool()) {
            comboVirtual->setCurrentIndex(1);
        } else {
            comboVirtual->setCurrentIndex(0);
        }
    }
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

void EndConfigPage::comboDbActivated(int index)
{
    CommandLine *cmd = qobject_cast<CommandLine *>(commandLine());
    if (!cmd)
        return;
    if (index==1) {
        cmd->setValue(Core::ICommandLine::ClearUserDatabases, true);
    } else {
        cmd->setValue(Core::ICommandLine::ClearUserDatabases, false);
    }
}

void EndConfigPage::comboVirtualActivated(int index)
{
    CommandLine *cmd = qobject_cast<CommandLine *>(commandLine());
    if (!cmd)
        return;
    if (index==1) {
        cmd->setValue(Core::ICommandLine::CreateVirtuals, true);
    } else {
        cmd->setValue(Core::ICommandLine::CreateVirtuals, false);
    }
}

void EndConfigPage::retranslate()
{
    setTitle(tr("%1 is now configured").arg(qApp->applicationName() + " v"+qApp->applicationVersion()));
    setSubTitle(tr("Please read the online user guide."));
    if (lblDb)
        lblDb->setText(tr("You can clean and recreate all your databases. Select the option above. If you select the clean option, all databases will be erased with <b>definitive data loss</b>."));
    int current = 0;
    if (comboDb) {
        current = comboDb->currentIndex();
        comboDb->clear();
        comboDb->addItems(QStringList() << tr("Don't clean databases") << tr("Clean and recreate database"));
        comboDb->setCurrentIndex(current);
    }
    lblVirtual->setText(tr("You can create virtual data to test the application. Select the option above."));
    current = comboVirtual->currentIndex();
    comboVirtual->clear();
    comboVirtual->addItems(QStringList() << tr("Don't create virtual data") << tr("Create virtual data"));
    comboVirtual->setCurrentIndex(current);
    label_message->setText(tr("Help, support and information:"));
    label_url->setText(QString("<a href=\"%1\">%1</a>").arg(settings()->path(Core::ISettings::WebSiteUrl)));
}

void EndConfigPage::changeEvent(QEvent *e)
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

