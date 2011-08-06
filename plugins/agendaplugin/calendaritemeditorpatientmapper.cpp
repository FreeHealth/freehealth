/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <calendar/calendar_item.h>
#include <calendar/calendar_people.h>
#include <calendar/abstract_calendar_model.h>
#include <translationutils/constanttranslations.h>

#include "ui_calendaritemeditorpatientmapper.h"

#include <QDebug>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

CalendarItemEditorPatientMapperWidget::CalendarItemEditorPatientMapperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::CalendarItemEditorPatientMapper)
{
    ui->setupUi(this);
    connect(ui->searchPatient, SIGNAL(selectedPatient(QString,QString)), this, SLOT(onPatientSelected(QString,QString)));
}

CalendarItemEditorPatientMapperWidget::~CalendarItemEditorPatientMapperWidget()
{
    delete ui;
}

void CalendarItemEditorPatientMapperWidget::setCalendarItem(const Calendar::CalendarItem &item)
{
    clear();

    Calendar::AbstractCalendarModel *model = item.model();
    if (!model)
        return;

    /** \todo code here : get peoples from the model */
//    m_SelectedPatientUids = item.peopleUids(Calendar::CalendarItem::PeopleAttendee);
//    m_SelectedPatientsNames = item.peopleNames(Calendar::CalendarItem::PeopleAttendee);
//    for(int i = 0; i < m_SelectedPatientUids.count(); ++i) {
//        addPatientRow(m_SelectedPatientsNames.at(i), m_SelectedPatientUids.at(i));
//    }
}

void CalendarItemEditorPatientMapperWidget::clear()
{
    foreach(const QString &uid, m_PatientWidgets.keys()) {
        ui->groupGridLayout->removeWidget(m_PatientWidgets.value(uid));
    }
    qDeleteAll(m_PatientWidgets);
    m_PatientWidgets.clear();
    ui->searchPatient->clear();
    m_Selected.clear();
}

void CalendarItemEditorPatientMapperWidget::addPatientRow(const QString &name, const QString &uid)
{
    QWidget *w = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(w);
    w->setLayout(layout);
    layout->setMargin(0);
    // Remove button
    QToolButton *b = new QToolButton(this);
    b->setIconSize(QSize(16, 16));
    b->setMinimumSize(QSize(20,20));
    b->setMaximumSize(QSize(20,20));
    QAction *a = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONCLOSEDARK));
    a->setToolTip(QString("%1 %2").arg(tkTr(Trans::Constants::REMOVE_TEXT)).arg(tkTr(Trans::Constants::PATIENT)));
    a->setData(uid);
    b->addAction(a);
    b->setDefaultAction(a);
    connect(b, SIGNAL(triggered(QAction*)), this, SLOT(removePatient(QAction*)));
    // Patient name
    QLabel *l = new QLabel(this);
    l->setText(name);
    QFont bold;
    bold.setBold(true);
    l->setFont(bold);
    // Add to layout
    layout->addWidget(b);
    layout->addWidget(l);
    ui->groupGridLayout->addWidget(w);
    m_PatientWidgets.insert(uid, w);
}

void CalendarItemEditorPatientMapperWidget::removePatient(QAction *action)
{
    QString uid = action->data().toString();
    ui->groupGridLayout->removeWidget(m_PatientWidgets.value(uid));
    delete m_PatientWidgets.value(uid);
    m_PatientWidgets.remove(uid);
    m_Selected.removeAll(Calendar::People(Calendar::People::PeopleAttendee, "", uid));
}

void CalendarItemEditorPatientMapperWidget::onPatientSelected(const QString &name, const QString &uid)
{
    if (!m_PatientWidgets.contains(uid)) {
        addPatientRow(name, uid);
        m_Selected.append(Calendar::People(Calendar::People::PeopleAttendee, name, uid));
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
    return true;
}

bool CalendarItemEditorPatientMapper::clear()
{
    if (m_Widget) {
        m_Widget->clear();
    }
    return true;
}

bool CalendarItemEditorPatientMapper::submitChangesToCalendarItem(const Calendar::CalendarItem &item)
{
    if (!m_Widget) {
        return false;
    }

    Calendar::AbstractCalendarModel *model = item.model();
    if (!model)
        return false;

    for(int i = 0; i < m_Widget->selected().count(); ++i) {
        model->addPeople(item, m_Widget->selected().at(i));
    }

    return true;
}
