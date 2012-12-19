#include "dayavailability.h"

#include <utils/global.h>

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
    if (IN_RANGE(index, 0, timeRanges.count()))
        return timeRanges.at(index);
    return TimeRange();
}
void DayAvailability::removeTimeRangeAt(const int index)
{
    if (index > timeRanges.count())
        return;
    timeRanges.remove(index);
}
