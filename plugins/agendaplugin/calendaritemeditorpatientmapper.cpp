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

#include <patientbaseplugin/patientbasecompleter.h>

#include "ui_calendaritemeditorpatientmapper.h"

using namespace Agenda;
using namespace Internal;

CalendarItemEditorPatientMapperWidget::CalendarItemEditorPatientMapperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::CalendarItemEditorPatientMapper)
{
    ui->setupUi(this);

    Patients::PatientBaseCompleter *c = new Patients::PatientBaseCompleter(this);
    ui->searchPatient->setValidator(c->validator());
    ui->searchPatient->setCompleter(c);
}

CalendarItemEditorPatientMapperWidget::~CalendarItemEditorPatientMapperWidget()
{
    delete ui;
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
    return new Internal::CalendarItemEditorPatientMapperWidget(parent);
}

bool CalendarItemEditorPatientMapper::setCalendarItem(const Calendar::CalendarItem &item)
{}

bool CalendarItemEditorPatientMapper::clear()
{}

bool CalendarItemEditorPatientMapper::submitChangesToCalendarItem(Calendar::CalendarItem &item)
{}
