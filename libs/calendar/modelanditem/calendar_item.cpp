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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Calendar::CalendarItem
  The Calendar::CalendarItem is used to locate an event in the
  Calendar::AbstractCalendarModel. It acts the same way as QModelIndex.
*/

#include "calendar_item.h"
#include "abstract_calendar_model.h"
#include <calendar/common.h>

#include <QDebug>

using namespace Calendar;

/** Constructs an invalid item */
CalendarItem::CalendarItem() :
        m_Model(0)
{}

/** Constructs an item starting at \e beginning and ending at \e ending. */
CalendarItem::CalendarItem(const QDateTime &beginning, const QDateTime &ending) :
        m_Model(0)
{
    m_beginning = beginning;
    m_ending = ending;
    m_created = QDateTime::currentDateTime();
    m_beginningType = Date_DateTime;
    m_endingType = Date_DateTime;
}

/** Constructs an item starting at \e beginning and ending at \e ending with the specified \e uid. */
CalendarItem::CalendarItem(const QString &uid, const QDateTime &beginning, const QDateTime &ending) :
        m_Model(0)
{
    m_uid = uid;
    m_beginning = beginning;
    m_ending = ending;
    m_created = QDateTime::currentDateTime();
    m_beginningType = Date_DateTime;
    m_endingType = Date_DateTime;
}

/** if true, beginning and ending date types are set to Date_Date. If false, beginning and ending date types are set to Date_DateTime */
void CalendarItem::setDaily(bool value) {
	DateType dateType = value ? Date_Date : Date_DateTime;

	if (m_beginningType == dateType && m_endingType == dateType)
		return;

	m_beginningType = dateType;
	m_endingType = dateType;
}

bool CalendarItem::isValid() const
{
    // TODO: code here
    return m_beginning.isValid() && (m_Model);
}

bool CalendarItem::isNull() const
{
    // TODO: code here
    return false;
}

/** Wrapper to the Calendar::AbstractCalendarModel::data() */
QVariant CalendarItem::data(const int ref) const
{
    if (m_Model) {
        // from Calendar ref to Model ref
        int refModel = -1;
        switch (ref) {
        case Uid : return m_uid;
        case Label: refModel = AbstractCalendarModel::Label; break;
        case Description: refModel = AbstractCalendarModel::Description; break;
        case Type: refModel = AbstractCalendarModel::Type; break;
        case Status: refModel = AbstractCalendarModel::Status; break;
        case IsPrivate: refModel = AbstractCalendarModel::IsPrivate; break;
        case Password: refModel = AbstractCalendarModel::Password; break;
        case IsBusy: refModel = AbstractCalendarModel::IsBusy; break;
        case IsAGroupEvent: refModel = AbstractCalendarModel::IsAGroupEvent; break;
        case DateStart: return m_beginning;
        case DateEnd: return m_ending;
        case Location: refModel = AbstractCalendarModel::Location; break;
        case LocationUid: refModel = AbstractCalendarModel::LocationUid; break;
        case IconPath: refModel = AbstractCalendarModel::IconPath; break;
        case CreatedDate: return m_created;
        }
        if (refModel!=-1)
            return m_Model->data(*this, refModel);
    }
    return QVariant();
}

/** Wrapper to the Calendar::AbstractCalendarModel::setData() */
bool CalendarItem::setData(const int ref, const QVariant &value)
{
    if (m_Model) {
        switch (ref) {
        case DateStart: setBeginning(value.toDateTime()); break;
        case DateEnd:  setEnding(value.toDateTime()); break;
        case CreatedDate: m_created = value.toDateTime(); break;
        }
        return m_Model->setData(*this, ref, value);
    } else {
        qWarning() << "CalendarItem does not have model";
    }
    return false;
}

void CalendarItem::setBeginningType(DateType value)
{
    if (m_beginningType==value)
        return;
    m_beginningType = value;
}

void CalendarItem::setEndingType(DateType value)
{
    if (m_endingType==value)
        return;
    m_endingType = value;
}

/** compute an intersection value with a day range
             * returns:
             * -1 if item is entirely before first day
             * 0 if item intersects [firstDay, lastDay]
             * 1 if item is entirely after lastDay
*/
int CalendarItem::intersects(const QDate &firstDay, const QDate &lastDay) const
{
    return intersectsDays(beginning(), ending(), firstDay, lastDay);
}

/** compute an overlap value with another item
             * returns:
             * false if items do not overlap
             * true if items overlap
*/
bool CalendarItem::overlap(const CalendarItem &item) const
{
    return ending() > item.beginning() && beginning() < item.ending();
}

/** Define the beginning date. The date will be passed to the item's model */
void CalendarItem::setBeginning(const QDateTime &value)
{
    if (!value.isValid())
        return;
    if (m_beginning==value)
        return;
    m_beginning=value;
    // inform the model
    if (m_Model) {
        m_Model->setData(*this, AbstractCalendarModel::DateStart, value);
    }
}

/** Define the ending date. The date will be passed to the item's model */
void CalendarItem::setEnding(const QDateTime &value)
{
    if (!value.isValid())
        return;
    if (m_ending==value)
        return;
    m_ending=value;
    // inform the model
    if (m_Model) {
        m_Model->setData(*this, AbstractCalendarModel::DateEnd, value);
    }
}

/** Test Calendar::CalendarItem equality */
bool CalendarItem::operator==(const CalendarItem &other) const
{
    // TODO: match on uid should be enough assuming each CalendarItem have a different uid.
    return (other.uid()==uid() &&
            other.beginning()==m_beginning &&
            other.ending() == m_ending);
}

/** A sort function for calendar items */
bool Calendar::calendarItemLessThan(const CalendarItem &item1, const CalendarItem &item2)
{
    // at first compare with begin dates. If they're equals, compare by end dates.
    // at first, compare the beginnings
    if (item1.beginning() < item2.beginning())
        return true;
    else if (item1.beginning() > item2.beginning())
        return false;
    // beginnings are the same => compare the endings
    else if (item1.ending() > item2.ending())
        return true;
    else if (item1.ending() < item2.ending())
        return false;
    // beginnings and endings are the same, compare the creation date time
    else if (item1.created() < item2.created())
        return true;
    else if (item1.created() > item2.created())
        return false;
    // create date time are the same => finally compare uid
    else
        return item1.uid() > item2.uid();
}

QDebug operator<<(QDebug dbg, const Calendar::CalendarItem &c)
{
    dbg.nospace() << "CalendarItem("
                  << c.beginning().toString(QLocale().dateTimeFormat(QLocale::ShortFormat))
                  << ", "
                  << c.ending().toString(QLocale().dateTimeFormat(QLocale::ShortFormat))
                  << ", " << c.model() << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Calendar::CalendarItem *c)
{
    if (!c) {
        dbg.nospace() << "CalendarItem(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

