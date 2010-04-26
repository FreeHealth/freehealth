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
#include "mainwindowpreferences.h"

#include <utils/log.h>
#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/constants.h>

using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }


MainWindowPreferencesPage::MainWindowPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("MainWindowPreferencesPage"); }

MainWindowPreferencesPage::~MainWindowPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString MainWindowPreferencesPage::id() const { return objectName(); }
QString MainWindowPreferencesPage::name() const { return tr("General"); }
QString MainWindowPreferencesPage::category() const { return tr("General"); }

void MainWindowPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void MainWindowPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void MainWindowPreferencesPage::finish() { delete m_Widget; }

void MainWindowPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
    defaultvalues.insert(Core::Constants::S_USE_EXTERNAL_DATABASE, false);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *MainWindowPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new MainWindowPreferencesWidget(parent);
    return m_Widget;
}



MainWindowPreferencesWidget::MainWindowPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
    log->setIcon(theme()->icon(Core::Constants::ICONEYES));
    pass->setIcon(theme()->icon(Core::Constants::ICONEYES));
    pass->toogleEchoMode();
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool())
        on_testButton_clicked();
}

void MainWindowPreferencesWidget::setDatasToUi()
{
    updateCheckingCombo->setCurrentIndex(settings()->value(Utils::Constants::S_CHECKUPDATE).toInt());
    useExternalDB->setChecked(settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool());
    host->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST).toByteArray()));
    log->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG).toByteArray()));
    pass->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS).toByteArray()));
    port->setValue(QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT).toByteArray())).toInt());
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

void MainWindowPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Utils::Constants::S_CHECKUPDATE, updateCheckingCombo->currentIndex());
    saveExternalDatabase(s, useExternalDB->isChecked(), host->text(), log->text(), pass->text(), port->value());
}

void MainWindowPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("MainWindowPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
    s->setValue(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
    s->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, false);
    s->sync();
}

void MainWindowPreferencesWidget::on_testButton_clicked()
{
    testConnectionLabel->setText(tr("Test in progress..."));
    {
        QSqlDatabase test = QSqlDatabase::addDatabase("QMYSQL", "FREEDIAMS_MAINWIN_CONNECTION_TESTER");
        test.setHostName(host->text());
        test.setPort(port->value());
        test.setUserName(log->text());
        test.setPassword(pass->text());
        if (!test.open()) {
            testButton->setIcon(theme()->icon(Core::Constants::ICONERROR));
            testConnectionLabel->setText(tr("Connection error: %1").arg(test.lastError().number()));
            testConnectionLabel->setToolTip(test.lastError().text());
        } else {
            testButton->setIcon(theme()->icon(Core::Constants::ICONOK));
            testConnectionLabel->setText(tr("Connected"));
        }
    }
    QSqlDatabase::removeDatabase("FREEDIAMS_MAINWIN_CONNECTION_TESTER");
}


void MainWindowPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
