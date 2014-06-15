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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "agendapreferencespage.h"
#include "constants.h"

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

#include <QDebug>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

AgendaPreferencesPage::AgendaPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("AgendaPreferencesPage");
}

AgendaPreferencesPage::~AgendaPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

QString AgendaPreferencesPage::id() const
{
    return objectName();
}

QString AgendaPreferencesPage::displayName() const
{
    return tkTr(Trans::Constants::AGENDA);
}

QString AgendaPreferencesPage::category() const
{
    return tkTr(Trans::Constants::AGENDA);
}

QString AgendaPreferencesPage::title() const
{
    return tr("Agenda preferences");
}

int AgendaPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_AGENDA;
}


void AgendaPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void AgendaPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AgendaPreferencesPage::finish()
{
    delete m_Widget;
}

void AgendaPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(Constants::S_SELECTOR_FIELDSTOSHOW, PatientSelector::Default);
//    defaultvalues.insert(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, true);
//    defaultvalues.insert(Constants::S_SELECTOR_USEGENDERCOLORS, true);
//    defaultvalues.insert(Constants::S_PATIENTBARCOLOR, Qt::white);

//    foreach(const QString &k, defaultvalues.keys()) {
//        if (settings()->value(k) == QVariant())
//            settings()->setValue(k, defaultvalues.value(k));
//    }
//    settings()->sync();
}

QWidget *AgendaPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AgendaPreferencesWidget(parent);
    return m_Widget;
}



AgendaPreferencesWidget::AgendaPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDataToUi();
}

void AgendaPreferencesWidget::setDataToUi()
{
//    showVirtualPatients->setChecked(settings()->value(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS).toBool());
//    genderColor->setChecked(settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool());
//    patientBarColor->setColor(QColor(settings()->value(Constants::S_PATIENTBARCOLOR).toString()));
}

void AgendaPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
//    Core::ISettings *s;
//    if (!sets)
//        s = settings();
//    else
//        s = sets;

//    s->setValue(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, showVirtualPatients->isChecked());
//    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, genderColor->isChecked());
//    s->setValue(Constants::S_PATIENTBARCOLOR, patientBarColor->color());
}

void AgendaPreferencesWidget::writeDefaultSettings(Core::ISettings *)
{
    //    qWarning() << "---------> writedefaults";
//    Utils::Log::addMessage("AgendaPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
//    s->setValue(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, true);
//    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, true);
//    s->setValue(Constants::S_PATIENTBARCOLOR, Qt::white);
//    s->sync();
}

void AgendaPreferencesWidget::changeEvent(QEvent *e)
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

