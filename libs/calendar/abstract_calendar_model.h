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
#ifndef ABSTRACT_CALENDAR_MODEL_H
#define ABSTRACT_CALENDAR_MODEL_H

#include <calendar/calendar_exporter.h>
#include <calendar/calendar_item.h>
#include <calendar/usercalendar.h>

#include <QObject>
QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

/**
 * \file abstract_calendar_model.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 11 Jul 2011
*/

namespace Calendar {

class CALENDAR_EXPORT AbstractCalendarModel : public QObject
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Uid  = 0,
        Label,
        Description,
        Type,
        Status,
        IsPrivate,
        Password,
        IsBusy,
        IsAGroupEvent,
        DateStart,
        DateEnd,
        Location,
        LocationUid,
        IconPath,
        CreatedDate,
        UserData = 10000
    };

    enum PeopleType {
        PeopleAttendee = 0,
        PeopleOwner,
        PeopleUser
    };

    enum ItemStatus {  // synchronized with common.h -> availableStatus() stringlist
        Waiting = 0,
        Approved,
        Arrived,
        Changed,
        Cancelled,
        Missed
    };

    AbstractCalendarModel(QObject *parent = 0);

    // Management of Items
    virtual int count() const = 0;

    virtual Calendar::CalendarItem getItemByUid(const QString &uid) const = 0;
    virtual QList<Calendar::CalendarItem> getItemsBetween(const QDate &from, const QDate &to) const = 0;

    virtual CalendarItem insertItem(const QDateTime &begin, const QDateTime &end) = 0;
    virtual Calendar::CalendarItem addCalendarItem(const Calendar::CalendarItem &item) = 0;
    virtual void removeItem(const QString &uid) = 0;

    // Item data
    virtual QVariant data(const Calendar::CalendarItem &item, int dataRef, int role = Qt::DisplayRole) const;
    virtual bool setData(const Calendar::CalendarItem &item, int dataRef, const QVariant &value, int role = Qt::EditRole);

    virtual void addPeople(const Calendar::CalendarItem &item, const PeopleType people, const QString &name, const QString &uid = QString::null);
    virtual QStringList peopleNames(const Calendar::CalendarItem &item, const PeopleType people = PeopleAttendee, bool skipEmpty = false) const;

    void stopEvents();
    void resumeEvents();


    // Management of Calendars
    virtual Calendar::UserCalendar calendar(const Calendar::CalendarItem &item) const = 0;
    virtual Calendar::UserCalendar addUserCalendar(const Calendar::UserCalendar &userCalendar) = 0;
    virtual bool updateUserCalendar(const Calendar::UserCalendar &calendar) = 0;
    virtual Calendar::UserCalendar defaultUserCalendar() const = 0;

    virtual QAbstractItemModel *userCalendarComboModel(QObject *parent) const = 0;
    virtual int defaultUserCalendarComboModelIndex() const = 0;
    virtual Calendar::UserCalendar calendarFromComboModelIndex(const int index) const = 0;

public Q_SLOTS:
    virtual void clearAll() {}
    virtual bool submitAll() {return false;}
    virtual bool submit(const Calendar::CalendarItem &) {return false;}

Q_SIGNALS:
    void dataChanged(const Calendar::CalendarItem &begin);
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

    void setItemIsMine(Calendar::CalendarItem *item) const;
    void setCalendarIsMine(Calendar::UserCalendar *item) const;
};

}  // End namespace Calendar

 #endif  // ABSTRACT_CALENDAR_MODEL_H
