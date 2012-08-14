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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "druggeneralpreferences.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/drugselector.h>
#include <drugsbaseplugin/idrug.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>

#include <QPixmap>

using namespace DrugsWidget;
using namespace Internal;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

DrugsViewWidget::DrugsViewWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
    testerBox->hide();
#ifdef FREEMEDFORMS
    patientNameOrder->hide();
    patientNameOrderLabel->hide();
#endif
}

void DrugsViewWidget::setDatasToUi()
{
    Core::ISettings *s = settings();
    //    fontSizeSpin->setValue(m_fontSize);
    QString userName = s->value(S_USER).toString();
    if ((!userName.isEmpty()) && (userName.startsWith("test_"))) {
        testerBox->setChecked(true);
        userNameEdit->setText(userName);
        userPasswordEdit->setText(s->value(S_PASSWORD).toString());
        userPasswordEdit->setEnabled(false);
    }
    else
        testerBox->setChecked(false);

    viewFontSizeSpin->setValue(s->value(S_VIEWFONTSIZE, 12).toInt());
    historicSizeSpin->setValue(s->value(S_HISTORYSIZE).toInt());
    levelOfWarningCombo->setCurrentIndex(s->value(DrugsDB::Constants::S_LEVELOFWARNING_STATICALERT).toInt());

    viewFontCombo->setCurrentFont(s->value(S_VIEWFONT).toString());
    viewFontSizeSpin->setValue(s->value(S_VIEWFONTSIZE).toInt());
    patientNameOrder->setCurrentIndex(s->value(S_PATIENTNAMESORDER, 0).toInt());
    showIconsCheck->setChecked(s->value(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION).toBool());

    dynamicAlerts->setChecked(s->value(DrugsDB::Constants::S_USEDYNAMICALERTS).toBool());
    int level = s->value(DrugsDB::Constants::S_LEVELOFWARNING_DYNAMICALERT).toInt();
    switch (level)
    {
    case DrugsDB::Constants::MinimumLevelOfWarning: dynamicAlertsLevel->setCurrentIndex(2); break;
    case DrugsDB::Constants::ModerateLevelOfWarning: dynamicAlertsLevel->setCurrentIndex(1); break;
    case DrugsDB::Constants::MaximumLevelOfWarning: dynamicAlertsLevel->setCurrentIndex(0); break;
    }
}

void DrugsViewWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    // manage font size
    s->setValue(S_VIEWFONTSIZE, viewFontSizeSpin->value());
    // set testers datas
    if ((userNameEdit->text().startsWith("test_")))
        s->setValue(S_USER, userNameEdit->text());

    if (!userPasswordEdit->text().isEmpty())
        s->setValue(S_PASSWORD, userPasswordEdit->text());

    // manage history size
    s->setValue(S_HISTORYSIZE, historicSizeSpin->value());
    s->setValue(S_DRUGHISTORY, QVariant());
    s->setValue(DrugsDB::Constants::S_LEVELOFWARNING_STATICALERT , levelOfWarningCombo->currentIndex());

    s->setValue(S_VIEWFONT , viewFontCombo->currentFont());
    s->setValue(S_VIEWFONTSIZE, viewFontSizeSpin->value());
    QFont font = viewFontCombo->currentFont();
    font.setPointSize(viewFontSizeSpin->value());
    if (DrugsWidget::DrugsWidgetManager::instance()->currentView())
        DrugsWidget::DrugsWidgetManager::instance()->currentView()->changeFontTo(font);

    s->setValue(S_PATIENTNAMESORDER, patientNameOrder->currentIndex());
    s->setValue(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION, showIconsCheck->isChecked());

    s->setValue(DrugsDB::Constants::S_USEDYNAMICALERTS, dynamicAlerts->isChecked());
    switch (dynamicAlertsLevel->currentIndex())
    {
    case 2: s->setValue(DrugsDB::Constants::S_LEVELOFWARNING_DYNAMICALERT, DrugsDB::Constants::MinimumLevelOfWarning); break;
    case 1: s->setValue(DrugsDB::Constants::S_LEVELOFWARNING_DYNAMICALERT, DrugsDB::Constants::ModerateLevelOfWarning); break;
    case 0: s->setValue(DrugsDB::Constants::S_LEVELOFWARNING_DYNAMICALERT, DrugsDB::Constants::MaximumLevelOfWarning); break;
    }
}

void DrugsViewWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    LOG_FOR("DrugsViewWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget"));
    s->setValue(S_CONFIGURED, true);
    s->setValue(S_VIEWFONT , QFont());
    s->setValue(S_VIEWFONTSIZE, QFont().pointSize());
    s->setValue(S_HISTORYSIZE, 20);
    s->setValue(S_DRUGHISTORY, QVariant());
    s->setValue(DrugsDB::Constants::S_LEVELOFWARNING_STATICALERT , DrugsDB::Constants::MaximumLevelOfWarning);
    s->setValue(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION , true);
    s->setValue(DrugsDB::Constants::S_USEDYNAMICALERTS, true);
    s->setValue(DrugsDB::Constants::S_LEVELOFWARNING_DYNAMICALERT, DrugsDB::Constants::MinimumLevelOfWarning);
    s->setValue(S_PATIENTNAMESORDER, 0);

    s->setValue(S_DRUGFONT , QFont().toString());
    s->setValue(S_PRESCRIPTIONFONT , QFont().toString());
}

void DrugsViewWidget::changeEvent(QEvent *e)
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

DrugsViewOptionsPage::DrugsViewOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsViewOptionsPage"); }

DrugsViewOptionsPage::~DrugsViewOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsViewOptionsPage::id() const { return objectName(); }
QString DrugsViewOptionsPage::name() const { return tr("View"); }
QString DrugsViewOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }
QString DrugsViewOptionsPage::title() const {return tr("Drug's view preferences");}

void DrugsViewOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugsViewOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsViewOptionsPage::finish() { delete m_Widget; }

QString DrugsViewOptionsPage::helpPage()
{
    QString l = QLocale().name().left(2);
    if (l=="fr")
        return Constants::H_PREFERENCES_VIEW_FR;
    return Constants::H_PREFERENCES_VIEW_EN;
}

void DrugsViewOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(S_VIEWFONT, QFont());
    defaultvalues.insert(S_VIEWFONTSIZE, QFont().pointSize());
    defaultvalues.insert(S_HISTORYSIZE, 20);
    defaultvalues.insert(S_DRUGHISTORY, QVariant());
    defaultvalues.insert(S_DRUGFONT,QFont());
    defaultvalues.insert(S_PRESCRIPTIONFONT,QFont());
    defaultvalues.insert(DrugsDB::Constants::S_LEVELOFWARNING_STATICALERT, DrugsDB::Constants::MaximumLevelOfWarning);
    defaultvalues.insert(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION,true);
    defaultvalues.insert(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES,true);
    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);
    defaultvalues.insert(DrugsDB::Constants::S_USEDYNAMICALERTS, true);
    defaultvalues.insert(DrugsDB::Constants::S_LEVELOFWARNING_DYNAMICALERT, DrugsDB::Constants::MinimumLevelOfWarning);
    defaultvalues.insert(S_PATIENTNAMESORDER, 0);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *DrugsViewOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsViewWidget(parent);
    return m_Widget;
}
