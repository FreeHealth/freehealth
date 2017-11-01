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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "calendaritemeditorusercalendarmapper.h"
#include "calendaritemmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <calendar/calendar_item.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include "ui_calendaritemeditorusercalendarmapper.h"

#include <QDebug>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

CalendarItemEditorUserCalendarMapperWidget::CalendarItemEditorUserCalendarMapperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::CalendarItemEditorUserCalendarMapper),
    m_Model(0)
{
    ui->setupUi(this);
}

CalendarItemEditorUserCalendarMapperWidget::~CalendarItemEditorUserCalendarMapperWidget()
{
    delete ui;
}

void CalendarItemEditorUserCalendarMapperWidget::setCalendarItem(const Calendar::CalendarItem &item)
{
    clear();
    if (!item.model()) {
        LOG_ERROR("No item model");
        return;
    }
    m_Model = qobject_cast<CalendarItemModel*>(item.model());
}

void CalendarItemEditorUserCalendarMapperWidget::clear()
{
    // set combo index to the default usercalendar
    ui->agendasCombo->setCurrentIndex();
}

void CalendarItemEditorUserCalendarMapperWidget::onAgendaSelected(const int index)
{
}

CalendarItemEditorUserCalendarMapper::CalendarItemEditorUserCalendarMapper(QObject *parent) :
    Calendar::ICalendarItemDataWidget(parent)
{
}

CalendarItemEditorUserCalendarMapper::~CalendarItemEditorUserCalendarMapper()
{
}

int CalendarItemEditorUserCalendarMapper::insertionPlace() const
{
    return Calendar::ICalendarItemDataWidget::BeforeDateTime;
}

QWidget *CalendarItemEditorUserCalendarMapper::createWidget(QWidget *parent)
{
    if (m_Widget) {
        return m_Widget;
    }
    m_Widget = new Internal::CalendarItemEditorUserCalendarMapperWidget(parent);
    return m_Widget;
}

bool CalendarItemEditorUserCalendarMapper::setCalendarItem(const Calendar::CalendarItem &item)
{
    if (m_Widget)
        m_Widget->setCalendarItem(item);
    return true;
}

bool CalendarItemEditorUserCalendarMapper::clear()
{
    if (m_Widget) {
        m_Widget->clear();
    }
    return true;
}

bool CalendarItemEditorUserCalendarMapper::submitChangesToCalendarItem(Calendar::CalendarItem &item)
{
    if (!m_Widget) {
        return false;
    }
//    for(int i = 0; i < m_Widget->selectedPatientUids().count(); ++i) {
//        item.addPeople(Calendar::CalendarItem::PeopleAttendee, m_Widget->selectedPatientsNames().at(i), m_Widget->selectedPatientUids().at(i));
//    }
    return true;
}
