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
#ifndef USERCALENDARWIDGET_H
#define USERCALENDARWIDGET_H

#include <calendar/calendar_exporter.h>
#include <calendar/usercalendar.h>

#include <QWidget>

namespace Calendar {
class AbstractCalendarModel;
class UserCalendar;

namespace Ui {
    class UserCalendarEditorWidget;
}

class CALENDAR_EXPORT UserCalendarEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserCalendarEditorWidget(QWidget *parent = 0);
    ~UserCalendarEditorWidget();

    void setModel(AbstractCalendarModel *model);
    void setUserCalendar(const Calendar::UserCalendar &calendar);

public Q_SLOTS:
    void submit();
    UserCalendar userCalendar() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::UserCalendarEditorWidget *ui;
    UserCalendar m_Calendar;
    DayAvailabilityModel *m_AvailabilityModel;
};

}  // End namespace Calendar


#endif // USERCALENDARWIDGET_H
