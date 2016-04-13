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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "protocolpreferencespage.h"
#include "constants.h"
#include "ui_protocolpreferencespage.h"

#include <drugsbaseplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

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
QString ProtocolPreferencesPage::displayName() const { return tkTr(Trans::Constants::PROTOCOLS); }
QString ProtocolPreferencesPage::category() const { return tkTr(Trans::Constants::DRUGS); }
QString ProtocolPreferencesPage::title() const {return tr("Drug's protocole preferences");}
int ProtocolPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_DRUGS + 70;
}

void ProtocolPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void ProtocolPreferencesPage::apply()
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
QString ProtocolPreferencesPage::helpPage()
{
    QString l = QLocale().name().left(2);
    if (l=="fr")
        return Constants::H_PREFERENCES_PROTOCOL_FR;
    return Constants::H_PREFERENCES_PROTOCOL_EN;
}

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
    setDataToUi();
}

ProtocolPreferencesWidget::~ProtocolPreferencesWidget()
{
    delete ui;
    ui = 0;
}

void ProtocolPreferencesWidget::setDataToUi()
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
