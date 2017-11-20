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
/**
 * \class Core::ServerPreferencesWidget
 * Allow you to edit Server connection (like MySQL). The widget automatically
 * catches data from the settings in Ctor.
 */

#include "serverpreferenceswidget.h"
#include "ui_serverpreferenceswidget.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_icons.h>

#include <QHostInfo>

using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

namespace Core {
namespace Internal {
class ServerPreferencesWidgetPrivate
{

public:
    ServerPreferencesWidgetPrivate(ServerPreferencesWidget *parent) :
        ui(new Internal::Ui::ServerPreferencesWidget),
        _hostNameLengthValid(false),
        _hostReachable(false),
        _connectionSucceeded(false),
        _grants(Utils::Database::Grant_NoGrant),
        q(parent)
    {}

    ~ServerPreferencesWidgetPrivate()
    {
        delete ui;
    }

    void setupUi()
    {
        ui->setupUi(q);
        ui->log->setIcon(theme()->icon(Core::Constants::ICONEYES));
        ui->pass->setIcon(theme()->icon(Core::Constants::ICONEYES));
        ui->pass->toogleEchoMode();
        ui->useDefaultAdminLog->setVisible(false);
        ui->testMySQLButton->setEnabled(_hostReachable);
        ui->userGroupBox->setEnabled(false);
        ui->testConnectionLabel->setVisible(false);

        // Populate with settings()->databaseConnector() data
        const Utils::DatabaseConnector &db = settings()->databaseConnector();
        ui->host->setText(db.host());
        ui->log->setText(db.clearLog());
        ui->pass->setText(db.clearPass());
        ui->port->setValue(db.port());
        ui->port->setValue(3306);
        ui->testHostConnectionLabel->setText(QT_TR_NOOP("Enter host name..."));
    }

public:
    Internal::Ui::ServerPreferencesWidget *ui;
    bool _hostNameLengthValid, _hostReachable, _connectionSucceeded;
    Utils::Database::Grants _grants;
    QString _groupTitle, _groupTitleTrContext;

private:
    ServerPreferencesWidget *q;
};
} // namespace Internal
} // namespace Core

ServerPreferencesWidget::ServerPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    d(new ServerPreferencesWidgetPrivate(this))
{
    setObjectName("ServerPreferencesWidget");

    d->setupUi();

    // This is a work around the wrong attribution of the taborder due to widget promotion
    setTabOrder(d->ui->server, d->ui->host);
    setTabOrder(d->ui->host, d->ui->port);
    setTabOrder(d->ui->port, d->ui->testHostButton);
    setTabOrder(d->ui->testHostButton, d->ui->useDefaultAdminLog);
    setTabOrder(d->ui->useDefaultAdminLog, d->ui->log);
    setTabOrder(d->ui->log, d->ui->pass);
    setTabOrder(d->ui->pass, d->ui->testMySQLButton);

//    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool())
//        on_testMySQLButton_clicked();

    connect(d->ui->host, SIGNAL(textChanged(QString)), this, SLOT(checkHostNameLength(QString)));
    connect(d->ui->useDefaultAdminLog, SIGNAL(clicked(bool)), this, SLOT(toggleLogPass(bool)));
    connect(d->ui->testHostButton, SIGNAL(clicked()), this, SLOT(testHost()));
}

ServerPreferencesWidget::~ServerPreferencesWidget()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/**
 * Returns \e true if the connection to the database succeeded. This means
 * that connection to the server is correct and the user login and password
 * were accepted.
 */
bool ServerPreferencesWidget::connectionSucceeded() const
{
    return d->_connectionSucceeded;
}

/** Set the translation to use on the user login group */
void ServerPreferencesWidget::setUserLoginGroupTitle(const QString &trContext, const QString &untranslatedtext)
{
    d->_groupTitle = untranslatedtext;
    d->_groupTitleTrContext = trContext;
    d->ui->userGroupBox->setTitle(QApplication::translate(trContext.toUtf8(), untranslatedtext.toUtf8()));
}

/**
 * You can show a checkbox that allow user to automatically use the
 * default administrator login and password to test the connection.
 * By default, this box is not visible
 */
void ServerPreferencesWidget::showUseDefaultAdminLogCheckbox(bool show)
{
    d->ui->useDefaultAdminLog->setVisible(show);
}

/** Returns current host name */
QString ServerPreferencesWidget::hostName() const
{
    return d->ui->host->text();
}

/** Returns current port */
int ServerPreferencesWidget::port() const
{
    return d->ui->port->value();
}

/** Returns the current login (uncrypted) */
QString ServerPreferencesWidget::login() const
{
    return d->ui->log->text();
}

/** Returns the current password (uncrypted) */
QString ServerPreferencesWidget::password() const
{
    // FIXME: potential security issue
    return d->ui->pass->text();
}

/** Returns current user grants on database (only available if connection is reached) */
Utils::Database::Grants ServerPreferencesWidget::grantsOnLastConnectedDatabase() const
{
    return d->_grants;
}

void ServerPreferencesWidget::checkHostNameLength(const QString &hostName)
{
    // Hostname has changed: disable database credentials groupbox and button
    // Hide testConnectionLabel
    d->ui->userGroupBox->setEnabled(false);
    d->ui->testMySQLButton->setEnabled(false);
    d->ui->testConnectionLabel->setVisible(false);

    // Check if host name is valid (RFC1034, RFC1035, RFC2181)
    if ((hostName.length() < 1) || hostName.length() > 255) {
        d->_hostNameLengthValid = false;
    } else {
        d->_hostNameLengthValid = true;
    }
    // Colorize hostname red if invalid, blue if valid
    QPalette palette = d->ui->host->palette();
    palette.setColor(QPalette::Active, QPalette::Text, d->_hostNameLengthValid ? Qt::darkBlue : Qt::darkRed);
    d->ui->host->setPalette(palette);
    d->ui->labelHost->setPalette(palette);
    // Disable testHostButton
    d->ui->testHostButton->setEnabled(d->_hostNameLengthValid);
    if (hostName.isEmpty()) {
        d->ui->testHostConnectionLabel->setText(tr("Enter hostname..."));
        d->ui->testHostConnectionLabel->setToolTip(QString());
    } else if (d->_hostNameLengthValid) {
        d->ui->testHostConnectionLabel->setText(tr("Host name is valid..."));
        d->ui->testHostConnectionLabel->setToolTip(QString());
    } else {
        d->ui->testHostConnectionLabel->setText(tr("Host name is not valid..."));
        d->ui->testHostConnectionLabel->setToolTip("Must be between 1 and 255 characters long.");
    }
}

/** Slot, test connection to host using the UI hostname */
void ServerPreferencesWidget::testHost()
{
    testHost(d->ui->host->text());
}

/** Slot, test connection to host using a specific \e hostName */
void ServerPreferencesWidget::testHost(const QString &hostName)
{
    QString error;

    // Check that hostname is reachable on the network
    QHostInfo info = QHostInfo::fromName(hostName);
    d->_hostReachable = (info.error()==QHostInfo::NoError);
    error = info.errorString();
    // TODO: Test that a database server is configured


    // Change palette of the host edit
    QPalette palette = d->ui->host->palette();
    palette.setColor(QPalette::Active, QPalette::Text, d->_hostReachable ? Qt::darkBlue : Qt::darkRed);
    d->ui->host->setPalette(palette);
    d->ui->labelHost->setPalette(palette);

    // Change the login/password edit enable state according to host reached or not
    d->ui->userGroupBox->setEnabled(d->_hostReachable);
    d->ui->testMySQLButton->setEnabled(d->_hostReachable);

    if (!d->_hostReachable) {
        LOG_ERROR(QString("Host (%1:%2) not reachable: %3").arg(d->ui->host->text()).arg(d->ui->port->text()).arg(error));
        d->ui->testHostConnectionLabel->setText(tr("Host not reachable..."));
        d->ui->testHostConnectionLabel->setToolTip(error);
    } else {
        LOG(QString("Host available: %1:%2").arg(d->ui->host->text()).arg(d->ui->port->text()));
        d->ui->testHostConnectionLabel->setText(tr("Host available..."));
    }

    Q_EMIT hostConnectionChanged(d->_hostReachable);
    d->ui->userGroupBox->setEnabled(d->_hostReachable);
}

void ServerPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    if (!d->_hostReachable) {
        LOG_ERROR_FOR("ServerPreferencesWidget", tr("Host name error (%1:%2)").arg(d->ui->host->text()).arg(d->ui->port->value()));
        return;
    }
    LOG("saving host");
    Utils::DatabaseConnector db(login(), password(), hostName(), port());

    // FIXME: only working with MySQL as network database
    db.setDriver(Utils::Database::MySQL);
    if (d->ui->useDefaultAdminLog->isChecked()) {
        db.setClearLog("fmf_admin");
        db.setClearPass("fmf_admin");
    }
    s->setDatabaseConnector(db);
    Core::ICore::instance()->databaseServerLoginChanged();
}

void ServerPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    LOG_FOR("ServerPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("ServerPreferencesWidget"));
    Utils::DatabaseConnector db;
    s->setDatabaseConnector(db);
    s->sync();
}

/** Test mysql database connection */
void ServerPreferencesWidget::on_testMySQLButton_clicked()
{
    d->ui->testConnectionLabel->setVisible(true);
    d->ui->testConnectionLabel->setText(QString());
    if (!d->_hostReachable) {
        d->ui->testConnectionLabel->setVisible(true);
        d->ui->testConnectionLabel->setText(tr("Host not reachable..."));
        d->ui->userGroupBox->setEnabled(false);
        Q_EMIT userConnectionChanged(false);
        return;
    }
    d->ui->userGroupBox->setEnabled(true);
    if (login().isEmpty() && !d->ui->useDefaultAdminLog->isChecked()) {
        d->ui->testConnectionLabel->setText(tr("No anonymous connection allowed"));
        Q_EMIT userConnectionChanged(false);
        return;
    }

    // Test connection settings
    d->ui->testConnectionLabel->setText(tr("Test in progress..."));
    const QString connection = Utils::createUid();
    QSqlDatabase test = QSqlDatabase::addDatabase("QMYSQL", connection);
    test.setHostName(hostName());
    test.setPort(port());
    test.setDatabaseName("mysql");
    if (d->ui->useDefaultAdminLog->isChecked()) {
        test.setUserName("fmf_admin");
        test.setPassword("fmf_admin");
    } else {
        test.setUserName(login());
        test.setPassword(password());
    }
    // Try to open a database
    if (!test.open()) {
        Utils::Log::addDatabaseLog(this->objectName(), test);
        Utils::Log::saveLog();
        d->ui->testMySQLButton->setIcon(theme()->icon(Core::Constants::ICONERROR));
        d->ui->testConnectionLabel->setText(tr("Connection error: %1").arg(test.lastError().databaseText()));
        d->ui->testConnectionLabel->setToolTip(test.lastError().driverText());
        d->_connectionSucceeded = false;
        d->_grants = Utils::Database::Grant_NoGrant;
        Q_EMIT userConnectionChanged(false);
    } else {
        d->ui->testMySQLButton->setIcon(theme()->icon(Core::Constants::ICONOK));
        d->ui->testConnectionLabel->setText(tr("Connected"));
        d->_connectionSucceeded = true;
        d->_grants = Utils::Database::getConnectionGrants(connection);
        saveToSettings();
        Q_EMIT userConnectionChanged(true);
    }
    QSqlDatabase::removeDatabase(connection);
}

/** Toggle login and password edits to enabled/disabled */
void ServerPreferencesWidget::toggleLogPass(bool state)
{
    d->ui->log->setEnabled(!state);
    d->ui->pass->setEnabled(!state);
}

void ServerPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        if (!d->_groupTitle.isEmpty())
            d->ui->userGroupBox->setTitle(QApplication::translate(d->_groupTitleTrContext.toUtf8(), d->_groupTitle.toUtf8()));
        break;
    default:
        break;
    }
}
