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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "usercalendar.h"

#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>

#include <QDate>
#include <QStandardItem>

#include <QDebug>

using namespace Agenda;
using namespace Trans::ConstantTranslations;

DayAvailability::DayAvailability() :
        m_id(-1),
        m_WeekDay(-1),
        m_isAvailable(true)
{}

void DayAvailability::addTimeRange(const QTime &from, const QTime &to)
{
    TimeRange range;
    if (from < to) {
        range.from = from;
        range.to = to;
    } else {
        range.from = to;
        range.to = from;
    }
    timeRanges.append(range);
}

void DayAvailability::addTimeRange(const TimeRange &tr)
{
    timeRanges.append(tr);
}

TimeRange DayAvailability::timeRange(const int index) const
{
    if (index < timeRanges.count())
        return timeRanges.at(index);
    return TimeRange();
}
void DayAvailability::removeTimeRangeAt(const int index)
{
    if (index > timeRanges.count())
        return;
    timeRanges.remove(index);
}

UserCalendar::UserCalendar() :
        m_modified(false)
{
}

UserCalendar::~UserCalendar()
{
}

bool UserCalendar::isValid() const
{
    // TODO: code here
    return true;
}

bool UserCalendar::isNull() const
{
    // TODO: code here
    return false;
}

QVariant UserCalendar::data(const int ref) const
{
    return m_data.value(ref);
}

bool UserCalendar::setData(const int ref, const QVariant &value)
{
    m_data.insert(ref, value);
    m_modified = true;
    return true;
}

bool UserCalendar::isModified() const
{
    return m_modified;
}

void UserCalendar::setModified(const bool state)
{
    m_modified = state;
}

QString UserCalendar::xmlOptions() const
{
    return QString();
}

bool UserCalendar::hasAvailability() const
{
    return (m_availabilities.count() > 0);
}

QVector<DayAvailability> UserCalendar::availabilities(const int day) const
{
    if (day == -1)
        return m_availabilities.toVector();
    QVector<DayAvailability> toReturn;
    for(int i = 0; i < m_availabilities.count(); ++i) {
        if (m_availabilities.at(i).weekDay() == day) {
            toReturn << m_availabilities.at(i);
        }
    }
    return toReturn;
}

void UserCalendar::addAvailabilities(const DayAvailability &av)
{
    m_availabilities.append(av);
    m_modified = true;
}

void UserCalendar::setAvailabilities(const QList<DayAvailability> &availabilities)
{
    m_modified = true;
    m_availabilities = availabilities;
}

void UserCalendar::clearAvailabilities()
{
    m_availabilities.clear();
    m_modified = true;
}

void UserCalendar::removeAvailabilitiesForWeekDay(const int weekday)
{
    for(int i = m_availabilities.count() - 1; i >= 0; --i) {
        if (m_availabilities.at(i).weekDay() == weekday) {
            m_availabilities.removeAt(i);
            m_modified = true;
        }
    }
}

void UserCalendar::removeAvailabilitiesTimeRange(const int weekday, const QTime &from, const QTime &to)
{
    for(int i = m_availabilities.count() - 1; i >= 0; --i) {
        DayAvailability &av = m_availabilities[i];
        if (av.weekDay() == weekday) {
            for(int j = av.timeRangeCount() - 1; j >= 0; --j) {
                if (av.timeRange(j).from == from && av.timeRange(j).to == to) {
                    av.removeTimeRangeAt(j);
                    m_modified = true;
                }
            }
        }
    }
}

/** Return true if the calendar availabilities allow this date. */
bool UserCalendar::canBeAvailable(const QDateTime &date) const
{
    int day = date.date().dayOfWeek();
    if (day == -1)
        return false;
    QTime time = date.time();
    for(int i = 0; i < m_availabilities.count(); ++i) {
        if (m_availabilities.at(i).weekDay()==day) {
            for(int j = 0; j < m_availabilities.at(i).timeRangeCount(); ++j) {
                TimeRange range = m_availabilities.at(i).timeRange(j);
                // date included in one timeRange ? --> return true
                if (time >= range.from && time <= range.to) {
                    return true;
                }
            }
        }
    }
    return false;
}

/** Return true if the calendar availabilities allow this date for a duration of \e durationInMinutes. */
bool UserCalendar::canBeAvailable(const QDateTime &start, const int durationInMinutes) const
{
    int day = start.date().dayOfWeek();
    if (day == -1)
        return false;
    QDateTime end = start.addSecs(durationInMinutes*60);

    // in the same day only
    if (start.date().dayOfWeek() != end.date().dayOfWeek())
        return false;

    const QTime &startTime = start.time();
    const QTime &endTime = end.time();
    for(int i = 0; i < m_availabilities.count(); ++i) {
        if (m_availabilities.at(i).weekDay()==day) {
            for(int j = 0; j < m_availabilities.at(i).timeRangeCount(); ++j) {
                TimeRange range = m_availabilities.at(i).timeRange(j);
                // start and end are included in one unique timeRange ? --> return true
                if ((startTime >= range.from && startTime <= range.to) &&
                    (endTime >= range.from && endTime <= range.to)) {
                    return true;
                }
            }
        }
    }
    return false;
}

QDebug operator<<(QDebug dbg, const Agenda::DayAvailability &c)
{
    QStringList t;
    for(int i = 0; i < c.timeRangeCount(); ++i) {
        const TimeRange &tr = c.timeRange(i);
        t << QString("%1-%2").arg(tr.from.toString()).arg(tr.to.toString());
    }
    dbg.nospace() << "DayAvailability("
                  << QDate::shortDayName(c.weekDay())
                  << t.join("; ")
                  << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Agenda::DayAvailability *c)
{
    if (!c) {
        dbg.nospace() << "DayAvailability(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}
