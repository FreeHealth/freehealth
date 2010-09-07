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
#include "protocolpreferencespage.h"
#include "constants.h"
#include "ui_protocolpreferencespage.h"

#include <drugsbaseplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

using namespace DrugsWidget;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  ProtocolPreferencesPage  /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
ProtocolPreferencesPage::ProtocolPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("ProtocolPreferencesPage"); }

ProtocolPreferencesPage::~ProtocolPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString ProtocolPreferencesPage::id() const { return objectName(); }
QString ProtocolPreferencesPage::name() const { return tkTr(Trans::Constants::PROTOCOLS); }
QString ProtocolPreferencesPage::category() const { return tkTr(Trans::Constants::DRUGS); }

void ProtocolPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void ProtocolPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}
void ProtocolPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA, QVariant());
    defaultvalues.insert(Constants::S_PROTOCOLCREATOR_DEFAULTBUTTON, Constants::S_VALUE_PRESCRIBEONLY);
    defaultvalues.insert(Constants::S_PROTOCOLCREATOR_AUTOCHANGE, true);
    defaultvalues.insert(Constants::S_PROTOCOLCREATOR_AUTOCHANGE_BUTTON, Constants::S_VALUE_SAVEPRESCRIBE);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k).isNull())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void ProtocolPreferencesPage::finish() { delete m_Widget; }

QWidget *ProtocolPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new ProtocolPreferencesWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  ProtocolPreferencesWidgetPrivate  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
ProtocolPreferencesWidget::ProtocolPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProtocolPreferencesWidget)
{
    ui->setupUi(this);
    setDatasToUi();
}

ProtocolPreferencesWidget::~ProtocolPreferencesWidget()
{
    delete ui;
    ui = 0;
}

void ProtocolPreferencesWidget::setDatasToUi()
{
    QString s = settings()->value(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA).toString();
    ui->drugFormRadio->setChecked(s.isEmpty());
    ui->intakeRadio->setChecked(s=="||");
    if (s.isEmpty() || s=="||") {
        ui->otherRadio->setChecked(false);
    } else {
        ui->otherRadio->setChecked(true);
        ui->lineEdit->setText(s);
    }

    m_ButtonChoices.insert(Constants::S_VALUE_SAVEPRESCRIBE, tr("Save protocol and prescribe"));
    m_ButtonChoices.insert(Constants::S_VALUE_PRESCRIBEONLY, tr("Prescribe only"));
    m_ButtonChoices.insert(Constants::S_VALUE_SAVEONLY, tr("Save protocol only"));
    m_ButtonChoices.insert(Constants::S_VALUE_TESTONLY, tr("Test interactions only"));

    ui->saveToggleCombo->addItems(m_ButtonChoices.values());
    ui->defaultButtonCombo->addItems(m_ButtonChoices.values());

    ui->saveToggleCombo->setCurrentIndex(m_ButtonChoices.keys().indexOf(settings()->value(Constants::S_PROTOCOLCREATOR_AUTOCHANGE_BUTTON).toString()));
    ui->defaultButtonCombo->setCurrentIndex(m_ButtonChoices.keys().indexOf(settings()->value(Constants::S_PROTOCOLCREATOR_DEFAULTBUTTON).toString()));
    ui->toggleCheckbox->setChecked(settings()->value(Constants::S_PROTOCOLCREATOR_AUTOCHANGE).toBool());
}


void ProtocolPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
    Utils::Log::addMessage("ProtocolPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("ProtocolPreferencesWidget"));
    set->setValue(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA, QString());
    set->setValue(Constants::S_PROTOCOLCREATOR_DEFAULTBUTTON, Constants::S_VALUE_PRESCRIBEONLY);
    set->setValue(Constants::S_PROTOCOLCREATOR_AUTOCHANGE, true);
    set->setValue(Constants::S_PROTOCOLCREATOR_AUTOCHANGE_BUTTON, Constants::S_VALUE_SAVEPRESCRIBE);
    set->sync();
}

void ProtocolPreferencesWidget::saveToSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
    if (ui->drugFormRadio->isChecked()) {
        set->setValue(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA, QVariant());
    } else if (ui->intakeRadio->isChecked()) {
        set->setValue(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA, "||");
    } else if (ui->otherRadio->isChecked()) {
        set->setValue(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA, ui->lineEdit->text());
    }
    set->setValue(Constants::S_PROTOCOLCREATOR_DEFAULTBUTTON, m_ButtonChoices.keys().at(ui->defaultButtonCombo->currentIndex()));
    set->setValue(Constants::S_PROTOCOLCREATOR_AUTOCHANGE, true);
    set->setValue(Constants::S_PROTOCOLCREATOR_AUTOCHANGE_BUTTON, m_ButtonChoices.keys().at(ui->saveToggleCombo->currentIndex()));
}

void ProtocolPreferencesWidget::changeEvent(QEvent *e)
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
