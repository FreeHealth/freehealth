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
#include "drugselectorpreferences.h"

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

DrugsSelectorWidget::DrugsSelectorWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDataToUi();
}

void DrugsSelectorWidget::setDataToUi()
{
    drugsBrandNameBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME).toBool());
    routeBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWROUTE).toBool());
    formBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWFORM).toBool());
    compoBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES).toBool());
    strengthBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH).toBool());

    useBackgroundForDosages->setChecked(settings()->value(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES).toBool());
    backgroundDosagesAvailableButton->setColor(QColor(settings()->value(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString()));
    backgroundAllergiesButton->setColor(QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString()));
    backgroundIntoleranceButton->setColor(QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString()));
}

void DrugsSelectorWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME, drugsBrandNameBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWROUTE, routeBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWFORM, formBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES, compoBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH, strengthBox->isChecked());

    s->setValue(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES, useBackgroundForDosages->isChecked());
    s->setValue(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, backgroundDosagesAvailableButton->color());
    s->setValue(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR, backgroundAllergiesButton->color());
    s->setValue(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR, backgroundIntoleranceButton->color());

    if (DrugsWidget::DrugsWidgetManager::instance()->currentView())
        DrugsWidget::DrugsWidgetManager::instance()->currentView()->drugSelector()->updateDrugsViewColumns();
}

void DrugsSelectorWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    LOG_FOR("DrugsSelectorWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsSelectorWidget"));

    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME, true);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWROUTE, false);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWFORM, false);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES, false);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH, false);

    s->setValue(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES,true);
    s->setValue(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);
    s->setValue(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR, QColor("red"));
    s->setValue(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR, QColor("yellow"));

    //    DrugsWidget::DrugsWidgetManager::instance()->currentView()->drugSelector()->updateDrugsViewColumns();
}

void DrugsSelectorWidget::changeEvent(QEvent *e)
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

DrugsSelectorOptionsPage::DrugsSelectorOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsSelectorOptionsPage"); }

DrugsSelectorOptionsPage::~DrugsSelectorOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsSelectorOptionsPage::id() const { return objectName(); }
QString DrugsSelectorOptionsPage::name() const { return tr("Selector"); }
QString DrugsSelectorOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }
QString DrugsSelectorOptionsPage::title() const {return tr("Drug's selector preferences");}
int DrugsSelectorOptionsPage::sortIndex() const {return 40;}

void DrugsSelectorOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void DrugsSelectorOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsSelectorOptionsPage::finish() { delete m_Widget; }
QString DrugsSelectorOptionsPage::helpPage()
{
    QString l = QLocale().name().left(2);
    if (l=="fr")
        return Constants::H_PREFERENCES_SELECTOR_FR;
    return Constants::H_PREFERENCES_SELECTOR_EN;
}

void DrugsSelectorOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME,true);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWROUTE, false);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWFORM, false);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH, false);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES, false);
    defaultvalues.insert(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES,true);
    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);
    defaultvalues.insert(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR, QColor("red"));
    defaultvalues.insert(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR, QColor("yellow"));

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *DrugsSelectorOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsSelectorWidget(parent);
    return m_Widget;
}
