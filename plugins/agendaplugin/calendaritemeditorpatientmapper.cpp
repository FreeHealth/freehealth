/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "calendaritemeditorpatientmapper.h"

//#include <patientbaseplugin/patientlineeditcompletersearch.h>

#include <calendar/calendar_item.h>

#include "ui_calendaritemeditorpatientmapper.h"

#include <QDebug>

using namespace Agenda;
using namespace Internal;

CalendarItemEditorPatientMapperWidget::CalendarItemEditorPatientMapperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::CalendarItemEditorPatientMapper)
{
    ui->setupUi(this);
    m_StringListModel = new QStringListModel(this);
    ui->selectedPatientView->setModel(m_StringListModel);
    connect(ui->searchPatient, SIGNAL(selectedPatient(QString,QString)), this, SLOT(onPatientSelected(QString,QString)));
}

CalendarItemEditorPatientMapperWidget::~CalendarItemEditorPatientMapperWidget()
{
    delete ui;
}

void CalendarItemEditorPatientMapperWidget::setCalendarItem(const Calendar::CalendarItem &item)
{
    // get peoples
    m_SelectedPatientUids = item.peopleUids(Calendar::CalendarItem::PeopleAttendee);
    m_SelectedPatientsNames = item.peopleNames(Calendar::CalendarItem::PeopleAttendee);
    m_StringListModel->setStringList(m_SelectedPatientsNames);
}

void CalendarItemEditorPatientMapperWidget::clear()
{
    ui->searchPatient->clear();
    m_SelectedPatientUids.clear();
    m_SelectedPatientsNames.clear();
}

void CalendarItemEditorPatientMapperWidget::onPatientSelected(const QString &name, const QString &uid)
{
    QStringList list = m_StringListModel->stringList();
    if (!list.contains(name)) {
        list.append(name);
        m_SelectedPatientUids.append(uid);
        m_SelectedPatientsNames.append(name);
        m_StringListModel->setStringList(list);
        ui->searchPatient->clear();
    }
}

CalendarItemEditorPatientMapper::CalendarItemEditorPatientMapper(QObject *parent) :
    Calendar::ICalendarItemDataWidget(parent)
{
}

CalendarItemEditorPatientMapper::~CalendarItemEditorPatientMapper()
{
}

int CalendarItemEditorPatientMapper::insertionPlace() const
{
    return Calendar::ICalendarItemDataWidget::Beginning;
}

QWidget *CalendarItemEditorPatientMapper::createWidget(QWidget *parent)
{
    if (m_Widget) {
        return m_Widget;
    }
    m_Widget = new Internal::CalendarItemEditorPatientMapperWidget(parent);
    return m_Widget;
}

bool CalendarItemEditorPatientMapper::setCalendarItem(const Calendar::CalendarItem &item)
{
    if (m_Widget)
        m_Widget->setCalendarItem(item);
}

bool CalendarItemEditorPatientMapper::clear()
{
    if (m_Widget) {
        m_Widget->clear();
    }
}

bool CalendarItemEditorPatientMapper::submitChangesToCalendarItem(Calendar::CalendarItem &item)
{
    if (!m_Widget) {
        return false;
    }
    for(int i = 0; i < m_Widget->selectedPatientUids().count(); ++i) {
        item.addPeople(Calendar::CalendarItem::PeopleAttendee, m_Widget->selectedPatientsNames().at(i), m_Widget->selectedPatientUids().at(i));
    }
    return true;
}
