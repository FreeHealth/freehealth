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

#include <QObject>

#include "calendar_item.h"

namespace Calendar {

class CALENDAR_EXPORT AbstractCalendarModel : public QObject
{
    Q_OBJECT
public:
    AbstractCalendarModel(QObject *parent = 0);

    virtual int count() const = 0;

    virtual CalendarItem getItemByUid(const QString &uid) const = 0;
    virtual QList<CalendarItem> getItemsBetween(const QDate &from, const QDate &to) const = 0;

    virtual const CalendarItem &insertItem(const QDateTime &begin, const QDateTime &end);
    virtual void setItemByUid(const QString &uid, const CalendarItem &item);
    virtual void removeItem(const QString &uid) = 0;

    void stopEvents();
    void resumeEvents();

public Q_SLOTS:
    virtual void clearAll() {}

Q_SIGNALS:
    void itemInserted(const CalendarItem &newItem);
    void itemModified(const CalendarItem &oldItem, const CalendarItem &newItem);
    void itemRemoved(const CalendarItem &removedItem);
    void reset();

protected:
    bool m_propagateEvents;

    void beginInsertItem();
    void endInsertItem(const CalendarItem &newItem);
    void beginModifyItem();
    void endModifyItem(const CalendarItem &oldItem, const CalendarItem &newItem);
    void beginRemoveItem();
    void endRemoveItem(const CalendarItem &removedItem);
};

}  // End namespace Calendar

 #endif  // ABSTRACT_CALENDAR_MODEL_H
