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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "common.h"
#include "modelanditem/calendar_item.h"

#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>

#include <QDate>

using namespace Trans::ConstantTranslations;

namespace Calendar {

QStringList availableStatus() {
    return QStringList()
            << tkTr(Trans::Constants::WAITING)
            << tkTr(Trans::Constants::APPROVED)
            << tkTr(Trans::Constants::ARRIVED)
            << tkTr(Trans::Constants::CHANGED)
            << tkTr(Trans::Constants::CANCELLED)
            << tkTr(Trans::Constants::MISSED)
               ;
}

QDate getFirstDateByRandomDate(ViewType viewType, const QDate &randomDate)
{
    if (randomDate.isNull())
        return randomDate;
    switch (viewType) {
    case View_Day:
        return randomDate;
    case View_Week:
        return randomDate.addDays(-randomDate.dayOfWeek() + 1);
    case View_Month:
        return randomDate.addDays(- randomDate.day() + 1);
    default: // should never happend
        return QDate();
    }
}

QPair<QDate, QDate> getBoundingMonthDaysInterval(int year, int month) {
    QDate firstDayOfMonth(year, month, 1);

    // get the very first day of the grid (not necessarily the first day of the month)
    QDate veryFirstDay = firstDayOfMonth.addDays(-firstDayOfMonth.dayOfWeek() + 1);

    // get the very last day of the grid (not necessarily the last day of the month)
    QDate veryLastDay = firstDayOfMonth.addDays(firstDayOfMonth.daysInMonth() - 1);
    veryLastDay = veryLastDay.addDays(7 - veryLastDay.dayOfWeek());

    return QPair<QDate,QDate>(veryFirstDay, veryLastDay);
}

QPair<QDate, QDate> getBoundingMonthDaysInterval(const QDate &date) {
    return getBoundingMonthDaysInterval(date.year(), date.month());
}

int intersectsDays(const QDateTime &beginning, const QDateTime &ending,
                   const QDate &firstDay, const QDate &lastDay) {
    if (ending.date() < firstDay ||
            (ending.date() == firstDay && ending.time() == QTime(0, 0)))
        return -1;
    if (beginning.date() > lastDay)
        return 1;

    return 0;
}

QPair<QDate, QDate> getIntersectDayRange(const QDateTime &beginning, const QDateTime &ending) {
    QDate first = beginning.date();
    QDate last = ending.date();
    QDate firstDate, lastDate;
    for (QDate date = first; date <= last; date = date.addDays(1)) {
        if (!intersectsDays(beginning, ending, date, date)) {
            if (!firstDate.isValid())
                firstDate = date;
            lastDate = date;
        } else
            break;
    }
    return QPair<QDate,QDate>(firstDate, lastDate);
}

QPair<int, int> getItemTopAndHeight(const QTime &begin, const QTime &end, int hourHeight, int minimumItemHeight) {
    int seconds = end < begin ? begin.secsTo(QTime(23, 59)) + 1 : begin.secsTo(end);
    int top = (QTime(0, 0).secsTo(begin) * hourHeight) / 3600;
    int height = (seconds * hourHeight) / 3600;

    if (height < minimumItemHeight)
        height = minimumItemHeight;

    // vertical lines
    return QPair<int, int>(top, height);
}

QString calendarMimeType()
{
    return "freemedforms-calendar/calendar-item";
}

} // end namespace Calendar
