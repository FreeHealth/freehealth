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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       Christian A. Reiter                                               *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "dayavailability.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QDebug>

using namespace Agenda;
using namespace Trans::ConstantTranslations;

DayAvailability::DayAvailability() :
        m_id(-1),
        m_WeekDay(-1)
        //, m_isAvailable(true)
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

TimeRange DayAvailability::timeRangeAt(const int index) const
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

void DayAvailability::removeTimeRanges(const TimeRange &timeRange)
{
    foreach(const TimeRange &tr, timeRanges) {
        if(timeRange == tr)
            timeRanges.remove(timeRanges.indexOf(tr));
    }
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

    void resetModel()
    {
        q->clear();
        QFont bold;
        bold.setBold(true);
        // Create one item for each day of week
        for(int day = Qt::Monday; day <= Qt::Sunday; ++day) {
            QStandardItem *dayItem = new QStandardItem(QDate::longDayName(day));
            dayItem->setFont(bold);
            dayItem->setData(day, WeekDayRole);
            // Add availabilities to items
            const QVector<DayAvailability> &avail = m_UserCalendar->availabilities(day);
            for(int availabilityId = 0; availabilityId < avail.count(); ++availabilityId) {
                for(int timeRangeId = 0; timeRangeId < avail.at(availabilityId).timeRangeCount(); ++timeRangeId) {
                    TimeRange range = avail.at(availabilityId).timeRangeAt(timeRangeId);
                    QStandardItem *time = new QStandardItem(tkTr(Trans::Constants::FROM_1_TO_2).arg(range.from.toString()).arg(range.to.toString()));
                    time->setData(day, WeekDayRole);
                    time->setData(range.from, HourFromRole);
                    time->setData(range.to, HourToRole);
                    time->setData(timeRangeId, TimeRangeIdRole);
                    time->setData(availabilityId, AvailIdRole);
                    time->setToolTip(time->text());
                    dayItem->appendRow(time);
                }
            }
            if (dayItem->rowCount() > 0) {
                dayItem->sortChildren(0);
            }
            q->invisibleRootItem()->appendRow(dayItem);
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
{
    setHeaderData(1, Qt::Horizontal, "test");
}

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
    beginResetModel();
    d->m_UserCalendar = calendar;
    d->resetModel();
    endResetModel();
}

void DayAvailabilityModel::addAvailability(const DayAvailability &availability)
{
    Q_ASSERT(d->m_UserCalendar);
    Q_ASSERT(IN_RANGE(availability.weekDay(), Qt::Monday, Qt::Sunday));

    d->m_UserCalendar->addAvailabilities(availability);

    // find the day item
    QStandardItem *dayItem = 0;
    for(int i = 0; i < invisibleRootItem()->rowCount(); ++i) {
        if (invisibleRootItem()->child(i)->data(WeekDayRole).toInt() == availability.weekDay()) {
            dayItem = invisibleRootItem()->child(i);
            break;
        }
    }

    // at this point there MUST be a valid dayItem. If not, something really bad happened.
    Q_ASSERT(dayItem);

    // delete the placeholder item, if exists
    if (dayItem->hasChildren() && dayItem->child(0)->data(AvailIdRole).toInt() == -1)
        dayItem->removeRow(0);

    // Insert the TimeRanges
    for(int i = 0; i < availability.timeRangeCount(); ++i) {
        TimeRange range = availability.timeRangeAt(i);
        QStandardItem *timeItem = new QStandardItem(tkTr(Trans::Constants::FROM_1_TO_2).arg(range.from.toString()).arg(range.to.toString()));
        timeItem->setData(availability.weekDay(), WeekDayRole);
        timeItem->setData(range.from, HourFromRole);
        timeItem->setData(range.to, HourToRole);
        timeItem->setToolTip(timeItem->text());
        dayItem->appendRow(timeItem);
    }
    dayItem->sortChildren(0);

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

bool DayAvailabilityModel::submit()
{
    // FIXME: Data are not correctly submitted?
    LOG_ERROR("submit() is not coded.");
    return false;
}

QDebug operator<<(QDebug dbg, const Agenda::DayAvailability &c)
{
    QStringList t;
    for(int i = 0; i < c.timeRangeCount(); ++i) {
        const TimeRange &tr = c.timeRangeAt(i);
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
