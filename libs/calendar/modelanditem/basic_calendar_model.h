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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASIC_CALENDAR_MODEL_H
#define BASIC_CALENDAR_MODEL_H

#include <QPair>

#include <calendar/modelanditem/calendar_item.h>
#include <calendar/modelanditem/abstract_calendar_model.h>

/**
 * \file basic_calendar_model.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 11 Jul 2011
*/

namespace Calendar {
/**
 * This model offers an optimized version of <getItemsBetween()> based on dichotomy method and double lists.
*/
class BasicCalendarModel : public AbstractCalendarModel
{
    Q_OBJECT
public:
    BasicCalendarModel(QObject *parent = 0);
    virtual ~BasicCalendarModel();

    QList<CalendarItem> getItemsBetween(const QDate &from, const QDate &to);

    int count() const { return m_sortedByBeginList.count(); }

    CalendarItem insertItem(const QDateTime &beginning, const QDateTime &ending);
    Calendar::CalendarItem addCalendarItem(const Calendar::CalendarItem &item);
    void removeItem(const QString &uid);
    bool moveItem(const Calendar::CalendarItem &from, Calendar::CalendarItem &to){Q_UNUSED(from); Q_UNUSED(to); return false;}

    CalendarItem getItemByUid(const QString &uid) const;

    void clearAll();

private:
    QList<CalendarItem*> m_sortedByBeginList;
    QList<CalendarItem*> m_sortedByEndList;

    void setItemByUid(const QString &uid, const CalendarItem &item);

    // returns an insertion index for a datetime in <list> from <first> to <last> (dichotomy method)
    int getInsertionIndex(bool begin, const QDateTime &dateTime, const QList<CalendarItem*> &list, int first, int last) const;

    // search for an intersected item, the first found item is enough
    int searchForIntersectedItem(const QList<CalendarItem*> &list, const QDate &from, const QDate &to, int first, int last) const;

    // create a uniq uid
    QString createUid() const;

    CalendarItem *getItemPointerByUid(const QString &uid) const;
};
}

#endif
