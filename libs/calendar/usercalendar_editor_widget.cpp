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
#include "usercalendar_editor_widget.h"
#include "ui_usercalendar_editor_widget.h"

#include <QDebug>

using namespace Calendar;

UserCalendarEditorWidget::UserCalendarEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserCalendarEditorWidget),
    m_AvailabilityModel(0)
{
    ui->setupUi(this);
}

UserCalendarEditorWidget::~UserCalendarEditorWidget()
{
    delete ui;
    if (m_AvailabilityModel)
        delete m_AvailabilityModel;
    m_AvailabilityModel = 0;
}

void UserCalendarEditorWidget::setModel(AbstractCalendarModel *model)
{}

void UserCalendarEditorWidget::setUserCalendar(const Calendar::UserCalendar &calendar)
{
    // Availability
    if (m_AvailabilityModel) {
        delete m_AvailabilityModel;
        m_AvailabilityModel = 0;
    }

    m_AvailabilityModel = new DayAvailabilityModel(this);
    m_AvailabilityModel->setUserCalendar(calendar);
    ui->availabilityView->setModel(m_AvailabilityModel);

    // Description
    ui->calendarLabel->setText(calendar.data(UserCalendar::Label).toString());
    ui->defaultLocation->setText(calendar.data(UserCalendar::LocationUid).toString());
    ui->description->setText(calendar.data(UserCalendar::Description).toString());
//    ui->iconLabel->setPixmap();

    // Infos
    ui->defaultDuration->setValue(calendar.data(UserCalendar::DefaultDuration).toInt());
    ui->isDefaultCheck->setChecked(calendar.data(UserCalendar::IsDefault).toBool());
    ui->isPrivateCheck->setChecked(calendar.data(UserCalendar::IsPrivate).toBool());
    ui->password->setText(calendar.data(UserCalendar::Password).toString());

}

void UserCalendarEditorWidget::submit()
{
}

UserCalendar UserCalendarEditorWidget::userCalendar() const
{
}

void UserCalendarEditorWidget::changeEvent(QEvent *e)
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
