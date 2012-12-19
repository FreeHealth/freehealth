#include "dayavailability.h"

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

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

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////      DayAvailabilityModel     ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
namespace Agenda {
namespace Internal {

class DayAvailabilityModelPrivate
{
public:

    DayAvailabilityModelPrivate(DayAvailabilityModel *parent) :
        m_UserCalendar(0),
        q(parent)
    {}

    void refreshModel()
    {
        q->clear();
        QFont bold;
        bold.setBold(true);
        // Create one item foreach week of day
        QVector<QStandardItem *> days;
        for(int i = Qt::Monday; i <= Qt::Sunday; ++i) {
            QStandardItem *day = new QStandardItem(QDate::longDayName(i));
            day->setFont(bold);
            day->setData(i, WeekDayRole),
            days << day;
            // Add availabilities to items
            const QVector<DayAvailability> &avail = m_UserCalendar->availabilities(i);
            for(int j = 0; j < avail.count(); ++j) {
                for(int k = 0; k < avail.at(j).timeRangeCount(); ++k) {
                    TimeRange range = avail.at(j).timeRange(k);
                    QStandardItem *time = new QStandardItem(tkTr(Trans::Constants::FROM_1_TO_2).arg(range.from.toString()).arg(range.to.toString()));
                    time->setData(range.from, HourFromRole);
                    time->setData(range.to, HourToRole);
                    time->setData(k, TimeRangeIdRole);
                    time->setData(j, AvailIdRole);
                    time->setToolTip(time->text());
                    day->appendRow(time);
                }
            }
            if (day->rowCount()) {
                day->sortChildren(0);
                q->invisibleRootItem()->appendRow(day);
            }
        }
        if (!q->invisibleRootItem()->rowCount()) {
            QStandardItem *item = new QStandardItem(tkTr(Trans::Constants::NO_AVAILABILITY));
            q->invisibleRootItem()->appendRow(item);
        }
    }

public:
    UserCalendar *m_UserCalendar;

private:
    DayAvailabilityModel *q;
};
}
}

DayAvailabilityModel::DayAvailabilityModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::DayAvailabilityModelPrivate(this))
{}

DayAvailabilityModel::~DayAvailabilityModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void DayAvailabilityModel::clearAvailabilities()
{
    if (d->m_UserCalendar) {
        d->m_UserCalendar->setAvailabilities(QList<DayAvailability>());
        clear();
    }
}

void DayAvailabilityModel::setUserCalendar(UserCalendar *calendar)
{
    d->m_UserCalendar = calendar;
    d->refreshModel();
    reset();
}

void DayAvailabilityModel::addAvailability(const DayAvailability &availability)
{
    Q_ASSERT(d->m_UserCalendar);
    d->m_UserCalendar->addAvailabilities(availability);

    // find the day item
    QStandardItem *dayItem = 0;
    for(int i = 0; i < invisibleRootItem()->rowCount(); ++i) {
        if (invisibleRootItem()->child(i)->data(WeekDayRole).toInt()==availability.weekDay()) {
            dayItem = invisibleRootItem()->child(i);
            break;
        }
    }

    if (!dayItem) {
        // Create the day item
        QFont bold;
        bold.setBold(true);
        dayItem = new QStandardItem(QDate::longDayName(availability.weekDay()));
        dayItem->setData(availability.weekDay(), WeekDayRole);
        dayItem->setFont(bold);

        // Find where to insert the item to keep the sort order clean without resetting the model
        int after = -1;
        // BUG!!! child(0) is not there always
        if (invisibleRootItem()->child(0)->data(Qt::DisplayRole).toString() == tkTr(Trans::Constants::NO_AVAILABILITY)) {
            clear();
        } else {
            for(int i = 0; i < invisibleRootItem()->rowCount(); ++i) {
                if (invisibleRootItem()->child(i)->data(WeekDayRole).toInt() < availability.weekDay()) {
                    after = i;
                } else {
                    break;
                }
            }
            ++after;
        }

        // Add the time ranges
        for(int i = 0; i < availability.timeRangeCount(); ++i) {
            TimeRange range = availability.timeRange(i);
            QStandardItem *time = new QStandardItem(tkTr(Trans::Constants::FROM_1_TO_2).arg(range.from.toString()).arg(range.to.toString()));
            time->setData(range.from, HourFromRole);
            time->setToolTip(time->text());
            dayItem->appendRow(time);
        }

        // Insert the Day item to the root item
        insertRow(after, dayItem);
    } else {
        // Insert the TimeRanges
        for(int i = 0; i < availability.timeRangeCount(); ++i) {
            TimeRange range = availability.timeRange(i);
            QStandardItem *time = new QStandardItem(tkTr(Trans::Constants::FROM_1_TO_2).arg(range.from.toString()).arg(range.to.toString()));
            time->setData(range.from, HourFromRole);
            time->setToolTip(time->text());
            dayItem->appendRow(time);
        }
        dayItem->sortChildren(0);
    }
}

void DayAvailabilityModel::removeAvailability(const QModelIndex &index)
{
    if (d->m_UserCalendar) {
        if (index.parent()==QModelIndex()) {
            // delete all availabilities for a specific weekDay
            QStandardItem *item = itemFromIndex(index);
            int weekDay = item->data(WeekDayRole).toInt();
            d->m_UserCalendar->removeAvailabilitiesForWeekDay(weekDay);
            invisibleRootItem()->removeRow(index.row());
        } else {
            // delete a specific time range
            QStandardItem *parent = itemFromIndex(index.parent());
            int weekDay = parent->data(WeekDayRole).toInt();
            const QTime &from = itemFromIndex(index)->data(HourFromRole).toTime();
            const QTime &to = itemFromIndex(index)->data(HourToRole).toTime();
            d->m_UserCalendar->removeAvailabilitiesTimeRange(weekDay, from, to);
            parent->removeRow(index.row());
        }
    }
}

bool DayAvailabilityModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // TODO: code here
    qWarning() << Q_FUNC_INFO << row << count << parent;
    return true;
}

bool DayAvailabilityModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // TODO: code here
    qWarning() << Q_FUNC_INFO << row << count << parent;
    return true;
}

bool DayAvailabilityModel::submit()
{
    // TODO: code here
    return true;
}
