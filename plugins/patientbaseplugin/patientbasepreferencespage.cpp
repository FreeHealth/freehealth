/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "patientbasepreferencespage.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "patientcore.h"
#include "patientselector.h"
#include "patientwidgetmanager.h"

#include "ui_patientbasepreferencespage.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iphotoprovider.h>
#include <coreplugin/constants_menus.h>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

PatientBasePreferencesPage::PatientBasePreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("PatientBasePreferencesPage");
}

PatientBasePreferencesPage::~PatientBasePreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PatientBasePreferencesPage::id() const {
    return objectName();
}

QString PatientBasePreferencesPage::displayName() const {
    return tkTr(Trans::Constants::PATIENTS);
}

QString PatientBasePreferencesPage::category() const {
    return tkTr(Trans::Constants::PATIENT_DATABASE);
}

QString PatientBasePreferencesPage::title() const {
    return tr("Patient database preferences");
}

int PatientBasePreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_PATIENTS + 10;
}

void PatientBasePreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void PatientBasePreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PatientBasePreferencesPage::finish() { delete m_Widget; }

void PatientBasePreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_SELECTOR_FIELDSTOSHOW, PatientSelector::Default);
    defaultvalues.insert(Constants::S_SELECTOR_USEGENDERCOLORS, true);
    defaultvalues.insert(Constants::S_PATIENTBARCOLOR, int(Qt::white));
    defaultvalues.insert(Core::Constants::S_PATIENTCHANGEONCREATION, true);
    defaultvalues.insert(Constants::S_SEARCHWHILETYPING, true);
    defaultvalues.insert(Constants::S_RECENTPATIENT_MAX, 10);
    defaultvalues.insert(Constants::S_RECENTPATIENT_LIST, QString());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
}

QWidget *PatientBasePreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PatientBasePreferencesWidget(parent);
    return m_Widget;
}

PatientBasePreferencesWidget::PatientBasePreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    ui = new Ui::PatientBasePreferencesWidget;
    setObjectName("PatientBasePreferencesWidget");
    ui->setupUi(this);
    populatePhotoProviderCombo();
    setDataToUi();
}

PatientBasePreferencesWidget::~PatientBasePreferencesWidget()
{
    delete ui;
}

void PatientBasePreferencesWidget::setDataToUi()
{
    ui->selectNewlyCreatedBox->setChecked(settings()->value(Core::Constants::S_PATIENTCHANGEONCREATION).toBool());
    ui->genderColor->setChecked(settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool());
    ui->patientBarColor->setColor(QColor(settings()->value(Constants::S_PATIENTBARCOLOR).toString()));
    ui->searchWhileTyping->setChecked(settings()->value(Constants::S_SEARCHWHILETYPING).toBool());
    ui->maxRecentPatient->setValue(settings()->value(Constants::S_RECENTPATIENT_MAX, 10).toInt());
    ui->defaultCity->setText(settings()->value(Constants::S_NEWPATIENT_DEFAULTCITY).toString());
    ui->defaultZip->setText(settings()->value(Constants::S_NEWPATIENT_DEFAULTZIP).toString());

    // find the id of the photo source in the combo box items
    const int photoSourceIndex = ui->comboDefaultPhotoSource->findData(
                settings()->value(Constants::S_DEFAULTPHOTOSOURCE).toString());
    ui->comboDefaultPhotoSource->setCurrentIndex(photoSourceIndex);
}

void PatientBasePreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Core::Constants::S_PATIENTCHANGEONCREATION, ui->selectNewlyCreatedBox->isChecked());
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, ui->genderColor->isChecked());
    s->setValue(Constants::S_PATIENTBARCOLOR, ui->patientBarColor->color());
    s->setValue(Constants::S_SEARCHWHILETYPING, ui->searchWhileTyping->isChecked());
    s->setValue(Constants::S_RECENTPATIENT_MAX, ui->maxRecentPatient->value());
    s->setValue(Constants::S_NEWPATIENT_DEFAULTCITY, ui->defaultCity->text());
    s->setValue(Constants::S_NEWPATIENT_DEFAULTZIP, ui->defaultZip->text());

    // save the id of the provider to identify it the next time.
    const QString photoSourceId = ui->comboDefaultPhotoSource->itemData(
                ui->comboDefaultPhotoSource->currentIndex()).toString();
    s->setValue(Constants::S_DEFAULTPHOTOSOURCE, photoSourceId);

    // inform PatientSelector of the refreshsearchresult method
    Patients::PatientSelector::RefreshSearchResult method;
    if (ui->searchWhileTyping->isChecked())
        method = Patients::PatientSelector::WhileTyping;
    else
        method = Patients::PatientSelector::ReturnPress;

    //&& update refresh patientselector
    PatientCore::instance()->patientWidgetManager()->refreshSettings();
    PatientCore::instance()->patientWidgetManager()->selector()->setRefreshSearchResultMethod(method);
}

void PatientBasePreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    LOG_FOR("PatientBasePreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("Patient preferences"));
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, true);
    s->setValue(Constants::S_PATIENTBARCOLOR, int(Qt::white));
    s->setValue(Core::Constants::S_PATIENTCHANGEONCREATION, true);
    s->setValue(Constants::S_SEARCHWHILETYPING, true);
    s->setValue(Constants::S_RECENTPATIENT_MAX, 10);

    QList<Core::IPhotoProvider*> providerList = ExtensionSystem::PluginManager::instance()->getObjects<Core::IPhotoProvider>();
    qSort(providerList);
    if (providerList.isEmpty())
        s->setValue(Constants::S_DEFAULTPHOTOSOURCE, "");
    else
        s->setValue(Constants::S_DEFAULTPHOTOSOURCE, providerList.first()->id());
    s->sync();
}

void PatientBasePreferencesWidget::populatePhotoProviderCombo()
{
    QList<Core::IPhotoProvider*> providerList = ExtensionSystem::PluginManager::instance()->getObjects<Core::IPhotoProvider>();
    qSort(providerList);
    ui->comboDefaultPhotoSource->clear();
    foreach(Core::IPhotoProvider *provider, providerList) {
        ui->comboDefaultPhotoSource->addItem(provider->displayText(), QVariant(provider->id()));
    }
    ui->comboDefaultPhotoSource->setEnabled(!providerList.isEmpty());
}


void PatientBasePreferencesWidget::changeEvent(QEvent *e)
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
