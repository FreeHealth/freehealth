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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef USERCALENDAR_H
#define USERCALENDAR_H

#include <calendar/calendar_exporter.h>

#include <QVariant>
#include <QVector>
#include <QPair>
#include <QTime>
#include <QStandardItemModel>

namespace Calendar {

class AbstractCalendarModel;

struct CALENDAR_EXPORT TimeRange {
    TimeRange() : id(-1) {}

    // the id is used for database accesses and should be modified
    int id;
    QTime from, to;
};

class CALENDAR_EXPORT DayAvailability
{
public:
    DayAvailability();

    void setId(const int id) {m_id= id;}
    int id() const {return m_id;}

    void clearTimeRange() {timeRanges.clear();}

    void setWeekDay(const int _weekDays) {m_WeekDay=_weekDays;}
    int weekDay() const {return m_WeekDay;}

    void addTimeRange(const QTime &from, const QTime &to);
    void addTimeRange(const TimeRange &tr);
    void setTimeRanges(const QVector<TimeRange> &ranges) {timeRanges=ranges;}

    int timeRangeCount() const {return timeRanges.count();}
    TimeRange timeRange(const int index) const;

private:
    int m_id;
    int m_WeekDay;
    bool m_isAvailable;
    QVector<TimeRange> timeRanges;
};

class DayAvailabilityModel;

class CALENDAR_EXPORT UserCalendar
{
    friend class Calendar::AbstractCalendarModel;
    friend class Calendar::DayAvailabilityModel;
public:
    enum DataRepresentation {
        Uid = 0,
        UserOwnerUid,
        UserName,
        Label,
        Description,
        Type,
        Status,
        IsDefault,
        IsPrivate,
        Password,
        LocationUid,
        AbsPathIcon,
        DefaultDuration,
        UserData = 10000
    };

    UserCalendar();
    virtual ~UserCalendar();

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    virtual bool isModified() const;
    virtual void setModified(const bool state);

    virtual QString xmlOptions() const;

    bool hasAvailability() const;
    QVector<DayAvailability> availabilities(const int day = -1) const;
    void addAvailabilities(const DayAvailability &av);
    void setAvailabilities(const QList<DayAvailability> &availabilities);
    bool canBeAvailable(const QDateTime &date) const;
    Calendar::DayAvailabilityModel *availabilitiesModel(QObject *parent) const;

    AbstractCalendarModel *model() {return m_model;}

protected:
    void setModel(AbstractCalendarModel *model) {m_model=model;}

private:
    QHash<int, QVariant> m_Datas;
    bool m_Modified;
    QList<DayAvailability> m_Availabilities;
    AbstractCalendarModel *m_model;
};


namespace Internal {
class DayAvailabilityModelPrivate;
}

class CALENDAR_EXPORT DayAvailabilityModel : public QStandardItemModel
{
public:
    DayAvailabilityModel(QObject *parent = 0);
    ~DayAvailabilityModel();

    void setUserCalendar(const UserCalendar &calendar);
    UserCalendar userCalendar() const;

    void addAvailability(const DayAvailability &availability);

private:
    Internal::DayAvailabilityModelPrivate *d;
};


}  // End namespace Calendar

#endif // USERCALENDAR_H
