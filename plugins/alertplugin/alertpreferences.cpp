/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertpreferences.h"
#include "ui_alertpreferences.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

using namespace Alert;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/*! Creates a new preferences widget with a given parent. */
AlertPreferencesWidget::AlertPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlertPreferencesWidget)
{
    ui->setupUi(this);
}

AlertPreferencesWidget::~AlertPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void AlertPreferencesWidget::setDataToUi()
{
}

/*! Saves the settings in the ui to the settings data model. */
void AlertPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    // if no sets given as param, take default interface
//    Core::ISettings *s = sets? sets : settings();
}

/*! Writes the default settings to the data model. */
void AlertPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("AlertPreferencesWidget"));
}

/*! Retranslates the ui widgets to the changed language. */
void AlertPreferencesWidget::retranslateUi()
{
}

void AlertPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

/*! Creates a new preferences page with a given parent. */
AlertPreferencesPage::AlertPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("AlertPreferencesPage");
}

AlertPreferencesPage::~AlertPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString AlertPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString AlertPreferencesPage::displayName() const
{
    return tkTr(Trans::Constants::ALERTS);
}

/*! Returns the (translated) category of the preferences page. */
QString AlertPreferencesPage::category() const
{
    return tkTr(Trans::Constants::ALERTS);
}

int AlertPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ALERTS;
}


/*! Resets the whole preferences page to the default settings of the settings data model. */
void AlertPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void AlertPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AlertPreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void AlertPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(Webcam::Constants::FOO_SETTING_KEY, Webcam::Constants::FOO_SETTING_VALUE);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

/*! Creates the settings page */
QWidget *AlertPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AlertPreferencesWidget(parent);
    return m_Widget;
}


