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
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Calendar::CalendarItem
  Represents a event in an agenda (calendar).
*/

#include "common.h"
#include "calendar_item.h"

using namespace Calendar;

/** Constructs an invalid item */
CalendarItem::CalendarItem() :
        m_Modified(false)
{}

CalendarItem::CalendarItem(const QDateTime &beginning, const QDateTime &ending)
{
    m_Datas.insert(DateStart, beginning);
    m_Datas.insert(DateEnd, ending);
    m_Datas.insert(CreatedDate, QDateTime::currentDateTime());
    m_beginningType = Date_DateTime;
    m_endingType = Date_DateTime;
    m_Modified = false;
}

CalendarItem::CalendarItem(const QString &uid, const QDateTime &beginning, const QDateTime &ending)
{
    m_Datas.insert(Uid, uid);
    m_Datas.insert(DateStart, beginning);
    m_Datas.insert(DateEnd, ending);
    m_Datas.insert(CreatedDate, QDateTime::currentDateTime());
    m_beginningType = Date_DateTime;
    m_endingType = Date_DateTime;
    m_Modified = false;
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
    /** \todo code here */
    return m_Datas.value(DateStart).toDateTime().isValid();
}

bool CalendarItem::isNull() const
{
    /** \todo code here */
    return false;
}

bool CalendarItem::isModified() const
{
    return m_Modified;
}

void CalendarItem::setModified(const bool state)
{
    m_Modified=state;
}

QVariant CalendarItem::data(const int ref) const
{
    return m_Datas.value(ref, QVariant());
}

bool CalendarItem::setData(const int ref, const QVariant &value)
{
    m_Modified = true;
    m_Datas.insert(ref, value);
    return true;
}

void CalendarItem::setBeginningType(DateType value)
{
    if (m_beginningType==value)
        return;
    m_beginningType = value;
    m_Modified = true;
}

void CalendarItem::setEndingType(DateType value)
{
    if (m_endingType==value)
        return;
    m_endingType = value;
    m_Modified = true;
}

void CalendarItem::addPeople(const PeopleType people, const QString &name, const QString &uid)
{
    m_People.append(Internal::PeopleStructPrivate(people, name, uid));
}

QStringList CalendarItem::peopleNames(const PeopleType people) const
{
    QStringList toReturn;
    for(int i = 0; i < m_People.count(); ++i) {
        toReturn << m_People.at(i).name;
    }
    return toReturn;
}

QStringList CalendarItem::peopleUids(const PeopleType people) const
{
    QStringList toReturn;
    for(int i = 0; i < m_People.count(); ++i) {
        toReturn << m_People.at(i).uid;
    }
    return toReturn;
}

void CalendarItem::removePeople(const QString &uid)
{
    for(int i = 0; i < m_People.count(); ++i) {
        if (m_People.at(i).uid==uid) {
            m_People.remove(i);
            break;
        }
    }
}

void CalendarItem::clearPeople(const PeopleType people)
{
    for(int i = 0; i < m_People.count(); ++i) {
        if (m_People.at(i).type==people) {
            m_People.remove(i);
        }
    }
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

/** a sort function for calendar items */
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
