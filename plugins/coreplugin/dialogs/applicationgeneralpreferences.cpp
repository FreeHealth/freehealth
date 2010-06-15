/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "applicationgeneralpreferences.h"
#include "ui_applicationgeneralpreferences.h"

#include <utils/log.h>
#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/constants_tokensandsettings.h>

using namespace Trans::ConstantTranslations;
using namespace Core;
using namespace Internal;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

ApplicationGeneralPreferencesWidget::ApplicationGeneralPreferencesWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ApplicationGeneralPreferencesWidget)
{
    setObjectName("ApplicationGeneralPreferencesWidget");
    ui->setupUi(this);
    connect(ui->useExternalDB, SIGNAL(clicked(bool)), this, SLOT(toggleDatabaseConfiguration(bool)));
    connect(ui->testButton, SIGNAL(clicked()), this, SLOT(on_testButton_clicked()));
    setDatasToUi();
    ui->log->setIcon(theme()->icon(Core::Constants::ICONEYES));
    ui->pass->setIcon(theme()->icon(Core::Constants::ICONEYES));
    ui->pass->toogleEchoMode();
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool())
        on_testButton_clicked();
}

void ApplicationGeneralPreferencesWidget::toggleDatabaseConfiguration(bool state)
{
    ui->server->setEnabled(state);
    ui->host->setEnabled(state);
    ui->log->setEnabled(state);
    ui->pass->setEnabled(state);
    ui->port->setEnabled(state);
    ui->testButton->setEnabled(state);
}

void ApplicationGeneralPreferencesWidget::setDatasToUi()
{
    ui->autoSave->setChecked(settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING).toBool());
    ui->updateCheckingCombo->setCurrentIndex(settings()->value(Utils::Constants::S_CHECKUPDATE).toInt());
    ui->useExternalDB->setChecked(settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool());
    ui->host->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST).toByteArray()));
    ui->log->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG).toByteArray()));
    ui->pass->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS).toByteArray()));
    ui->port->setValue(QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT).toByteArray())).toInt());
}

static void saveExternalDatabase(Core::ISettings *s, bool use, const QString &host, const QString &log, const QString &pass, const int port)
{
    if (s->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool() != use) {
        s->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, use);
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_HOST, QString(host.toAscii().toBase64()));
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PORT, QString::number(port).toAscii().toBase64());
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_LOG, QString(log.toAscii().toBase64()));
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PASS, QString(pass.toAscii().toBase64()));
        Core::ICore::instance()->databaseServerLoginChanged();
    }
}

void ApplicationGeneralPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, ui->autoSave->isChecked());
    s->setValue(Utils::Constants::S_CHECKUPDATE, ui->updateCheckingCombo->currentIndex());
    saveExternalDatabase(s, ui->useExternalDB->isChecked(), ui->host->text(), ui->log->text(), ui->pass->text(), ui->port->value());
}

void ApplicationGeneralPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("ApplicationGeneralPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
    s->setValue(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, true);
    s->setValue(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
    s->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, false);
    s->sync();
}

void ApplicationGeneralPreferencesWidget::on_testButton_clicked()
{
    ui->testConnectionLabel->setText(tr("Test in progress..."));
    {
        QSqlDatabase test = QSqlDatabase::addDatabase("QMYSQL", "__APP_CONNECTION_TESTER");
        test.setHostName(ui->host->text());
        test.setPort(ui->port->value());
        test.setUserName(ui->log->text());
        test.setPassword(ui->pass->text());
        if (!test.open()) {
            ui->testButton->setIcon(theme()->icon(Core::Constants::ICONERROR));
            ui->testConnectionLabel->setText(tr("Connection error: %1").arg(test.lastError().number()));
            ui->testConnectionLabel->setToolTip(test.lastError().text());
        } else {
            ui->testButton->setIcon(theme()->icon(Core::Constants::ICONOK));
            ui->testConnectionLabel->setText(tr("Connected"));
        }
    }
    QSqlDatabase::removeDatabase("__APP_CONNECTION_TESTER");
}

void ApplicationGeneralPreferencesWidget::changeEvent(QEvent *e)
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




ApplicationGeneralPreferencesPage::ApplicationGeneralPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("ApplicationGeneralPreferencesPage"); }

ApplicationGeneralPreferencesPage::~ApplicationGeneralPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString ApplicationGeneralPreferencesPage::id() const { return objectName(); }
QString ApplicationGeneralPreferencesPage::name() const { return tkTr(Trans::Constants::GENERAL); }
QString ApplicationGeneralPreferencesPage::category() const { return tkTr(Trans::Constants::GENERAL); }

void ApplicationGeneralPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void ApplicationGeneralPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void ApplicationGeneralPreferencesPage::finish() { delete m_Widget; }

void ApplicationGeneralPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, true);
    defaultvalues.insert(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
    defaultvalues.insert(Core::Constants::S_USE_EXTERNAL_DATABASE, false);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *ApplicationGeneralPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new Internal::ApplicationGeneralPreferencesWidget(parent);
    return m_Widget;
}

