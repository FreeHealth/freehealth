/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ABSTRACT_CALENDAR_MODEL_H
#define ABSTRACT_CALENDAR_MODEL_H

#include <calendar/calendar_exporter.h>
#include <calendar/modelanditem/calendar_item.h>
#include <calendar/modelanditem/calendar_people.h>

#include <QObject>
QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

/**
 * \file abstract_calendar_model.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 11 Aug 2011
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
    virtual QList<Calendar::CalendarItem> getItemsBetween(const QDate &from, const QDate &to) = 0;

    virtual CalendarItem insertItem(const QDateTime &begin, const QDateTime &end) = 0;
    virtual CalendarItem addCalendarItem(const Calendar::CalendarItem &item) = 0;
    virtual void removeItem(const QString &uid) = 0;
    virtual bool moveItem(const Calendar::CalendarItem &from, Calendar::CalendarItem &to) = 0;

    // Item data
    virtual QVariant data(const Calendar::CalendarItem &item, int dataRef, int role = Qt::DisplayRole) const;
    virtual bool setData(const Calendar::CalendarItem &item, int dataRef, const QVariant &value, int role = Qt::EditRole);

    virtual bool setPeopleList(const Calendar::CalendarItem &item, const QList<Calendar::People> &peoples);
    virtual bool addPeople(const Calendar::CalendarItem &item, const People &people);
    virtual bool removePeople(const Calendar::CalendarItem &item, const int peopleType, const QString &uid);
    virtual QList<Calendar::People> peopleList(const Calendar::CalendarItem &item);

    void stopEvents();
    void resumeEvents();

public Q_SLOTS:
    virtual void clearAll() {}
    virtual bool submitAll() {return false;}
    virtual bool submit(const Calendar::CalendarItem &) {return false;}
    virtual bool revert(const Calendar::CalendarItem &) {return false;}

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
};

}  // End namespace Calendar

 #endif  // ABSTRACT_CALENDAR_MODEL_H
