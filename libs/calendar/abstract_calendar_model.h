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
#ifndef ABSTRACT_CALENDAR_MODEL_H
#define ABSTRACT_CALENDAR_MODEL_H

#include <calendar/calendar_exporter.h>
#include <calendar/calendar_item.h>
#include <calendar/usercalendar.h>

#include <QObject>


namespace Calendar {

class CALENDAR_EXPORT AbstractCalendarModel : public QObject
{
    Q_OBJECT
public:
    AbstractCalendarModel(QObject *parent = 0);

    // Management of Items
    virtual int count() const = 0;

    virtual Calendar::CalendarItem getItemByUid(const QString &uid) const = 0;
    virtual QList<Calendar::CalendarItem> getItemsBetween(const QDate &from, const QDate &to) const = 0;

//    virtual void setItemByUid(const QString &uid, const Calendar::CalendarItem &item);

    virtual const CalendarItem &insertItem(const QDateTime &begin, const QDateTime &end) = 0;
    virtual Calendar::CalendarItem addCalendarItem(const Calendar::CalendarItem &item) = 0;
    virtual bool updateCalendarItem(const Calendar::CalendarItem &item) = 0;

    virtual void removeItem(const QString &uid) = 0;

    void stopEvents();
    void resumeEvents();

    // Management of Calendars
    virtual Calendar::UserCalendar calendar(const Calendar::CalendarItem &item) const = 0;
    virtual bool updateUserCalendar(const Calendar::UserCalendar &calendar) = 0;

public Q_SLOTS:
    virtual void clearAll() {}

Q_SIGNALS:
    void itemInserted(const Calendar::CalendarItem &newItem);
    void itemModified(const Calendar::CalendarItem &oldItem, const Calendar::CalendarItem &newItem);
    void itemRemoved(const Calendar::CalendarItem &removedItem);
    void reset();

protected:
    bool m_propagateEvents;

    void beginInsertItem();
    void endInsertItem(const Calendar::CalendarItem &newItem);
    void beginModifyItem();
    void endModifyItem(const Calendar::CalendarItem &oldItem, const Calendar::CalendarItem &newItem);
    void beginRemoveItem();
    void endRemoveItem(const Calendar::CalendarItem &removedItem);
};

}  // End namespace Calendar

 #endif  // ABSTRACT_CALENDAR_MODEL_H
