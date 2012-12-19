#ifndef DAYAVAILABILITY_H
#define DAYAVAILABILITY_H

#include "agenda_exporter.h"

#include <QTime>
#include <QVector>

namespace Agenda {

struct AGENDA_EXPORT TimeRange {
    TimeRange() : id(-1) {}

    // the id is used for database accesses and should be modified
    int id;
    QTime from, to;
};

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
    TimeRange timeRange(const int index) const;
    void removeTimeRangeAt(const int index);

private:
    int m_id;
    int m_WeekDay;
    bool m_isAvailable;
    QVector<TimeRange> timeRanges;
};

} // end namespace Agenda
#endif // DAYAVAILABILITY_H
