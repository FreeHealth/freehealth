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
#ifndef COMMON_H
#define COMMON_H

#include <calendar/calendar_exporter.h>

#include <QPair>

/**
 * \file common.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 12 Aug 2011
*/

class QDate;
class QDateTime;
class QTime;

namespace Calendar {
    enum ViewType {
        View_Day,
        View_Week,
        View_Month
    };

    enum DateType {
        Date_DateTime,
        Date_Date
    };

    /** Return the translated available status for the CalendarItem. Recall each QEvent::LanguageChange. */
    CALENDAR_EXPORT QStringList availableStatus();

    /** returns the first date of the view in function of a random date */
    CALENDAR_EXPORT QDate getFirstDateByRandomDate(ViewType viewType, const QDate &randomDate);

    /** returns the bounding days interval for a month. WARNING: days are not necessarily inside the month. */
    CALENDAR_EXPORT QPair<QDate, QDate> getBoundingMonthDaysInterval(int year, int month);

    /** returns the bounding days interval for a month. WARNING: days are not necessarily inside the month.
     * Same function as the previous one but with a QDate instead of a year+month.
     */
    CALENDAR_EXPORT QPair<QDate, QDate> getBoundingMonthDaysInterval(const QDate &date);

    /** compute an intersection value (int) between a range of QDateTime and a day range
     * @param beginning The lower bound of the QDateTime range
     * @param ending The upper bound of the QDateTime range
     * @param firstDay The lower bound of the day range
     * @param lastDay The upper bound of the QDateTime range
     * @return -1 if the range is entirely before first day
     * @return 0 if the range intersects [firstDay, lastDay]
     * @return 1 if the range is entirely after lastDay
     */
    CALENDAR_EXPORT int intersectsDays(const QDateTime &beginning, const QDateTime &ending,
                                           const QDate &firstDay, const QDate &lastDay);

    /** returns the first and last day intersecting a given range
     * Take account of 00:00AM for ending datetime
     * \param beginning A QDateTime representing the lower bound of a QDateTime range
     * \param ending A QDateTime representing the upper bound of a QDateTime range
     * \return {QPair<QDate,QDate>} The first and last day intersecting the given range
     */
    CALENDAR_EXPORT QPair<QDate, QDate> getIntersectDayRange(const QDateTime &beginning, const QDateTime &ending);

    /** returns the top and the height of an item, taking account of a minimum item height */
    CALENDAR_EXPORT QPair<int, int> getItemTopAndHeight(const QTime &begin, const QTime &end, int hourHeight, int minimumItemHeight);

    /** Return the mime-type of the calendar lib (used in drag/drop of appointments inside the calendar view) */
    CALENDAR_EXPORT QString calendarMimeType();
}

#endif
