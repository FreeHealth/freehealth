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
 *       Christian A. Reiter                                               *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_DAYAVAILABILITY_H
#define AGENDA_DAYAVAILABILITY_H

#include "agenda_exporter.h"
#include "usercalendar.h"

#include <QTime>
#include <QVector>
#include <QStandardItemModel>

namespace Agenda {

enum DayAvailabilityRoles {
    WeekDayRole = Qt::UserRole + 1,
    HourFromRole,
    HourToRole,
    TimeRangeIdRole,
    AvailIdRole
};

struct AGENDA_EXPORT TimeRange {
    TimeRange() : id(-1) {}
    inline bool operator== (const TimeRange &other) const { return (id == other.id) && (from == other.from) && (to == other.to);}
    // the id is used for database accesses and should be modified
    int id;
    QTime from, to;
};

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////      DayAvailability     ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
class AGENDA_EXPORT DayAvailability
{
public:
    DayAvailability();

    void setId(const int id) {m_id = id;}
    int id() const {return m_id;}

    void clearTimeRange() {timeRanges.clear();}

    void setWeekDay(const int weekDay)
    {
        Q_ASSERT((weekDay >= Qt::Monday && weekDay <= Qt::Sunday) || weekDay == -1);
        m_WeekDay = weekDay;
    }
    int weekDay() const {return m_WeekDay;}

    void addTimeRange(const QTime &from, const QTime &to);
    void addTimeRange(const TimeRange &tr);
    void setTimeRanges(const QVector<TimeRange> &ranges) {timeRanges = ranges;}

    int timeRangeCount() const {return timeRanges.count();}
    TimeRange timeRangeAt(const int index) const;
    void removeTimeRangeAt(const int index);
    void removeTimeRanges(const TimeRange &timeRangeAt);

private:
    int m_id;
    int m_WeekDay; // == Qt::DayOfWeek
    bool m_isAvailable;
    QVector<TimeRange> timeRanges;
};


///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////      DayAvailabilityModel     ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
namespace Internal {
class DayAvailabilityModelPrivate;
}

class DayAvailabilityModel : public QStandardItemModel
{
    Q_OBJECT
public:
    DayAvailabilityModel(QObject *parent = 0);
    ~DayAvailabilityModel();

    void clearAvailabilities();

    void setUserCalendar(UserCalendar *calendar);
    void addAvailability(const DayAvailability &availability);
    void removeAvailability(const QModelIndex &index);

public Q_SLOTS:
    bool submit();

private:
    Internal::DayAvailabilityModelPrivate *d;
};

} // end namespace Agenda

QDebug operator<<(QDebug dbg, const Agenda::DayAvailability &c);
QDebug operator<<(QDebug dbg, const Agenda::DayAvailability *c);

#endif // AGENDA_DAYAVAILABILITY_H
