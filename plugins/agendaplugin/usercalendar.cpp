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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "usercalendar.h"

#include <QDate>
#include <QStandardItem>

#include <QDebug>

using namespace Agenda;

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

UserCalendar::UserCalendar() :
        m_Modified(false)
{
}

UserCalendar::~UserCalendar()
{
}

bool UserCalendar::isValid() const
{
    /** \todo code here */
    return true;
}

bool UserCalendar::isNull() const
{
    /** \todo code here */
    return false;
}

QVariant UserCalendar::data(const int ref) const
{
    return m_Datas.value(ref);
}

bool UserCalendar::setData(const int ref, const QVariant &value)
{
    m_Datas.insert(ref, value);
    m_Modified = true;
    return true;
}

bool UserCalendar::isModified() const
{
    return m_Modified;
}

void UserCalendar::setModified(const bool state)
{
    m_Modified=state;
}

QString UserCalendar::xmlOptions() const
{
    return QString();
}

bool UserCalendar::hasAvailability() const
{
    return m_Availabilities.count();
}

QVector<DayAvailability> UserCalendar::availabilities(const int day) const
{
    if (day==-1)
        return m_Availabilities.toVector();
    QVector<DayAvailability> toReturn;
    for(int i = 0; i < m_Availabilities.count(); ++i) {
        if (m_Availabilities.at(i).weekDay()==day) {
            toReturn << m_Availabilities.at(i);
        }
    }
    return toReturn;
}

void UserCalendar::addAvailabilities(const DayAvailability &av)
{
    m_Availabilities.append(av);
}

void UserCalendar::setAvailabilities(const QList<DayAvailability> &availabilities)
{
    m_Availabilities = availabilities;
}

bool UserCalendar::canBeAvailable(const QDateTime &date) const
{
    int day = date.date().dayOfWeek();
    if (day==-1)
        return false;
    QTime time = date.time();
    for(int i = 0; i < m_Availabilities.count(); ++i) {
        if (m_Availabilities.at(i).weekDay()==day) {
            for(int j = 0; j < m_Availabilities.at(i).timeRangeCount(); ++j) {
                TimeRange range = m_Availabilities.at(i).timeRange(j);
                // date included in one timeRange ? --> return true
                if (time >= range.from && time <= range.to) {
                    return true;
                }
            }
        }
    }
    return false;
}

