/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
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

ServerPreferencesWidget::ServerPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::ServerPreferencesWidget),
    m_HostReachable(false),
    m_ConnectionSucceeded(false),
    m_Grants(Utils::Database::Grant_NoGrant)
{
    setObjectName("ServerPreferencesWidget");
    ui->setupUi(this);
    connect(ui->useDefaultAdminLog, SIGNAL(clicked(bool)), this, SLOT(toggleLogPass(bool)));
    setDatasToUi();
    ui->log->setIcon(theme()->icon(Core::Constants::ICONEYES));
    ui->pass->setIcon(theme()->icon(Core::Constants::ICONEYES));
    ui->pass->toogleEchoMode();
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool())
        on_testButton_clicked();
    connect(ui->testHostButton, SIGNAL(clicked()), this, SLOT(testHost()));
//    connect(ui->host, SIGNAL(textChanged(QString)), this, SLOT(testHost(QString)));
//    ui->userGroupBox->setEnabled(m_HostReachable);
    ui->testButton->setEnabled(m_HostReachable);
}

ServerPreferencesWidget::~ServerPreferencesWidget()
{
    delete ui;
}

bool ServerPreferencesWidget::connectionSucceeded() const
{
    return m_ConnectionSucceeded;
}

void ServerPreferencesWidget::setUserLoginGroupTitle(const QString &text)
{
    /** \todo this does not work ???? */
    ui->userGroupBox->setTitle(text);
    repaint();
}

QString ServerPreferencesWidget::hostName() const
{
    return ui->host->text();
}
int ServerPreferencesWidget::port() const
{
    return ui->port->value();
}
QString ServerPreferencesWidget::login() const
{
    return ui->log->text();
}
QString ServerPreferencesWidget::password() const
{
    return ui->pass->text();
}

void ServerPreferencesWidget::setDatasToUi()
{
    // Get from settings()->databaseConnector()
    const Utils::DatabaseConnector &db = settings()->databaseConnector();
//    ui->host->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST).toByteArray()));
//    ui->log->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG).toByteArray()));
//    ui->pass->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS).toByteArray()));
    ui->host->setText(db.host());
    ui->log->setText(db.clearLog());
    ui->pass->setText(db.clearPass());
    ui->port->setValue(db.port());
    if (Utils::isDebugCompilation()) {
        if (db.host().isEmpty()) {
            ui->host->setText("localhost");
            testHost("localhost");
        }
        ui->port->setValue(3306);
//        ui->host->setText("192.168.0.20");
//        testHost("192.168.0.20");
    }

//    ui->autoSave->setChecked(settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING).toBool());
//    ui->useExternalDB->setChecked(settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool());

//    if (!settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT).isNull()) {
//        ui->port->setValue(QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT).toByteArray())).toInt());
//    } else {
//        ui->port->setValue(3306);
//    }
}

void ServerPreferencesWidget::testHost()
{
    testHost(ui->host->text());
}

void ServerPreferencesWidget::testHost(const QString &hostName)
{
    if (hostName.length() < 3) {
        m_HostReachable = false;
        return;
    }
    QHostInfo info = QHostInfo::fromName(hostName);
    if (info.error()==QHostInfo::NoError) {
        QPalette palette = ui->host->palette();
        palette.setColor(QPalette::Active, QPalette::Text, Qt::darkBlue);
        ui->host->setPalette(palette);
        ui->labelHost->setPalette(palette);
        m_HostReachable = true;
    } else {
        QPalette palette = ui->host->palette();
        palette.setColor(QPalette::Active, QPalette::Text, Qt::darkRed);
        ui->host->setPalette(palette);
        ui->labelHost->setPalette(palette);
        m_HostReachable = false;
    }
    ui->userGroupBox->setEnabled(m_HostReachable);
    ui->testButton->setEnabled(m_HostReachable);
}

void ServerPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    if (!m_HostReachable) {
        LOG_ERROR_FOR("ServerPreferencesWidget", tr("Host name error (%1:%2)").arg(ui->host->text()).arg(ui->port->value()));
        return;
    }
    LOG("saving host");
    Utils::DatabaseConnector db(ui->log->text(), ui->pass->text(), ui->host->text(), ui->port->value());
    db.setDriver(Utils::Database::MySQL);
//    s->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, true);
//    s->setValue(Core::Constants::S_EXTERNAL_DATABASE_HOST, QString(ui->host->text().toAscii().toBase64()));
//    s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PORT, QString::number(ui->port->value()).toAscii().toBase64());
    if (ui->useDefaultAdminLog->isChecked()) {
        db.setClearLog("fmf_admin");
        db.setClearPass("fmf_admin");
//        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_LOG, QString(QString("fmf_admin").toAscii().toBase64()));
//        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PASS, QString(QString("fmf_admin").toAscii().toBase64()));
    }
    s->setDatabaseConnector(db);
//    s->sync();
    Core::ICore::instance()->databaseServerLoginChanged();
}

void ServerPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    LOG_FOR("ServerPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("ServerPreferencesWidget"));
//    s->setValue(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, true);
//    s->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, false);
//    s->setValue(Core::Constants::S_EXTERNAL_DATABASE_HOST, QString());
//    s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PORT, QString());
//    s->setValue(Core::Constants::S_EXTERNAL_DATABASE_LOG, QString());
//    s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PASS, QString());
    Utils::DatabaseConnector db;
    s->setDatabaseConnector(db);
    s->sync();
}

void ServerPreferencesWidget::on_testButton_clicked()
{
    if (!m_HostReachable) {
        ui->testConnectionLabel->setText(tr("Host not reachable..."));
        return;
    }
    if (ui->log->text().isEmpty() && !ui->useDefaultAdminLog->isChecked()) {
        ui->testConnectionLabel->setText(tr("No anonymous connection allowed"));
        return;
    }
    ui->testConnectionLabel->setText(tr("Test in progress..."));
    {
        QSqlDatabase test = QSqlDatabase::addDatabase("QMYSQL", "__APP_CONNECTION_TESTER");
        test.setHostName(ui->host->text());
        test.setPort(ui->port->value());
        if (ui->useDefaultAdminLog->isChecked()) {
            test.setUserName("fmf_admin");
            test.setPassword("fmf_admin");
        } else {
            test.setUserName(ui->log->text());
            test.setPassword(ui->pass->text());
        }
        if (!test.open()) {
            ui->testButton->setIcon(theme()->icon(Core::Constants::ICONERROR));
            ui->testConnectionLabel->setText(tr("Connection error: %1").arg(test.lastError().number()));
            ui->testConnectionLabel->setToolTip(test.lastError().text());
            m_ConnectionSucceeded = false;
            m_Grants = Utils::Database::Grant_NoGrant;
        } else {
            ui->testButton->setIcon(theme()->icon(Core::Constants::ICONOK));
            ui->testConnectionLabel->setText(tr("Connected"));
            m_ConnectionSucceeded = true;
            m_Grants = Utils::Database::getConnectionGrants("__APP_CONNECTION_TESTER");
//            qWarning() << "GRANTS" << m_Grants;
            saveToSettings();
        }
    }
    QSqlDatabase::removeDatabase("__APP_CONNECTION_TESTER");
}

void ServerPreferencesWidget::toggleLogPass(bool state)
{
    ui->log->setEnabled(!state);
    ui->pass->setEnabled(!state);
}

void ServerPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
