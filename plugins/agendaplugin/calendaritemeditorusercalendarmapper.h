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
#ifndef CALENDARITEMEDITORUSERCALENDARMAPPER_H
#define CALENDARITEMEDITORUSERCALENDARMAPPER_H

#include <calendar/icalendaritemdatawidget.h>

#include <QWidget>
#include <QHash>
#include <QPointer>

/**
 * \file calendaritemeditorusercalendarmapper.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 27 Jul 2011
*/


namespace Agenda {
class CalendarItemModel;

namespace Internal {
namespace Ui {
class CalendarItemEditorUserCalendarMapper;
}

class CalendarItemEditorUserCalendarMapperWidget : public QWidget
{
    Q_OBJECT
public:
    CalendarItemEditorUserCalendarMapperWidget(QWidget *parent);
    ~CalendarItemEditorUserCalendarMapperWidget();

    void clear();
    void setCalendarItem(const Calendar::CalendarItem &item);

private Q_SLOTS:
    void onAgendaSelected(const int index);

private:
    Internal::Ui::CalendarItemEditorUserCalendarMapper *ui;
    Agenda::CalendarItemModel *m_Model;
};

}  // End namespace Internal

class CalendarItemEditorUserCalendarMapper : public Calendar::ICalendarItemDataWidget
{
    Q_OBJECT
public:
    explicit CalendarItemEditorUserCalendarMapper(QObject *parent = 0);
    ~CalendarItemEditorUserCalendarMapper();

    int insertionPlace() const;
    QWidget *createWidget(QWidget *parent = 0);
    bool setCalendarItem(const Calendar::CalendarItem &item);

    bool clear();
    bool submitChangesToCalendarItem(Calendar::CalendarItem &item);

private:
    QPointer<Internal::CalendarItemEditorUserCalendarMapperWidget> m_Widget;
};


}  // End namespace Agenda

#endif // CALENDARITEMEDITORUSERCALENDARMAPPER_H
