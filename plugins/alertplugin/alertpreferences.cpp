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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertpreferences.h"
#include "ui_alertpreferences.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

using namespace Alert;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

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

void AlertPreferencesWidget::setDatasToUi()
{
}

void AlertPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;
}

void AlertPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("AlertPreferencesWidget"));
}

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

QString AlertPreferencesPage::id() const
{
    return objectName();
}

QString AlertPreferencesPage::name() const
{
    return tkTr(Trans::Constants::ALERTS);
}

QString AlertPreferencesPage::category() const
{
    return tkTr(Trans::Constants::ALERTS);
}

void AlertPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void AlertPreferencesPage::applyChanges()
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

void AlertPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *AlertPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AlertPreferencesWidget(parent);
    return m_Widget;
}


