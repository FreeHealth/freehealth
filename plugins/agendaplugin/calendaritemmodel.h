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
#ifndef AGENDA_CALENDARITEMMODEL_H
#define AGENDA_CALENDARITEMMODEL_H

#include <agendaplugin/agenda_exporter.h>
#include <calendar/modelanditem/abstract_calendar_model.h>

/**
 * \file calendaritemmodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 11 Aug 2011
*/

namespace Agenda {
class UserCalendar;
class AgendaCore;

namespace Internal {
class Appointment;
}

class AGENDA_EXPORT CalendarItemModel : public Calendar::AbstractCalendarModel
{
    Q_OBJECT
    friend class Agenda::AgendaCore;

protected:
    CalendarItemModel(const QVariant &calendarUid, QObject *parent);

public:
    ~CalendarItemModel();

    Calendar::CalendarItem getItemByUid(const QString &uid) const;
    QList<Calendar::CalendarItem> getItemsBetween(const QDate &from, const QDate &to);

    int count() const;

    Calendar::CalendarItem insertItem(const QDateTime &begin, const QDateTime &end);
    Calendar::CalendarItem addCalendarItem(const Calendar::CalendarItem &item);
    bool updateCalendarItem(const Calendar::CalendarItem &item);
    void removeItem(const QString &uid);
    bool moveItem(const Calendar::CalendarItem &from, Calendar::CalendarItem &to);

    QVariant data(const Calendar::CalendarItem &item, int dataRef, int role = Qt::DisplayRole) const;
    bool setData(const Calendar::CalendarItem &item, int dataRef, const QVariant &value, int role = Qt::EditRole);

    void stopEvents();
    void resumeEvents();

    virtual bool setPeopleList(const Calendar::CalendarItem &item, const QList<Calendar::People> &peoples);
    virtual bool addPeople(const Calendar::CalendarItem &item, const Calendar::People &people);
    QList<Calendar::People> peopleList(const Calendar::CalendarItem &item);

    // Filters
    bool filterUserCalendarEvents(const QVariant &calendarUid);

public Q_SLOTS:
    void clearAll();
    bool submitAll();
    bool submit(const Calendar::CalendarItem &item);
    bool revert(const Calendar::CalendarItem &item);


private:
    Calendar::CalendarItem toCalendarItem(Internal::Appointment *item) const;
    void setItemByUid(const QString &uid, const Calendar::CalendarItem &item);
    int searchForIntersectedItem(const QList<Internal::Appointment *> &list, const QDate &from, const QDate &to, int first, int last) const;
    int getInsertionIndex(bool begin, const QDateTime &dateTime, const QList<Internal::Appointment*> &list, int first, int last) const;
    Internal::Appointment *getItemPointerByUid(const int uid) const;
    int createUid() const;
    void getItemFromDatabase(const QDate &from, const QDate &to, const int calendarId) const;
    void getPeopleNames(Internal::Appointment *appointment) const;

private:
    mutable QList<Internal::Appointment *> m_sortedByBeginList;
    mutable QList<Internal::Appointment *> m_sortedByEndList;
    mutable QVector<QDate> m_RetrievedDates;
    QVariant m_CalendarUid;
};

}  // End namespace Agenda

#endif // AGENDA_CALENDARITEMMODEL_H
