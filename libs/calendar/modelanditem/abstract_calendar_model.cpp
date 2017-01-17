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
   \class Calendar::AbstractCalendarModel
   Model used by the Calendar::CalendarWidget to catch any Calendar::CalendarItem.
*/

/**
  \fn CalendarItem Calendar::AbstractCalendarModel::getItemByUid(const QString &uid) const
  Return an item by an uid
*/

/**
  \fn QList<CalendarItem> Calendar::AbstractCalendarModel::getItemsBetween(const QDate &from, const QDate &to)
  Returns all calendar items between two days
*/

/**
  \fn int Calendar::AbstractCalendarModel::count() const
  Returns the total items count
*/

/**
  \fn const CalendarItem &Calendar::AbstractCalendarModel::insertItem(const QDateTime &begin, const QDateTime &end)
  \brief insert an item with begin and end dates
  The base class function does nothing.
  \return the item if the insertion occured, or invalid item if failure
*/

/**
  \fn void Calendar::AbstractCalendarModel::removeItem(const QString &uid)
  Remove a calendar item in function of a uid
*/

/**
  \fn void Calendar::AbstractCalendarModel::clearAll()
  Clear all items. Default implementation does nothing.
*/

/**
  \fn void Calendar::AbstractCalendarModel::stopEvents()
  Cancel all future events about insertion, modification, deletion, etc
  Can be used for massive items insertion loops
*/


/**
  \fn void Calendar::AbstractCalendarModel::resumeEvents();
  Resume all events behavior about insertion, modification, deletion, etc
*/

/**
  \fn void Calendar::AbstractCalendarModel::itemInserted(const CalendarItem &newItem)
  Signal.
*/

/**
  \fn void Calendar::AbstractCalendarModel::itemModified(const CalendarItem &oldItem, const CalendarItem &newItem)
  Signal.
*/

/**
  \fn void Calendar::AbstractCalendarModel::itemRemoved(const CalendarItem &removedItem)
  Signal.
*/

/**
  \fn void Calendar::AbstractCalendarModel::reset()
  Signal
*/

/**
  \fn void Calendar::AbstractCalendarModel::beginInsertItem()
  This function must be called before any item insertion
*/

/**
  \fn void Calendar::AbstractCalendarModel::endInsertItem(const CalendarItem &newItem)
  This function must be called after any item insertion
*/

/**
  \fn void Calendar::AbstractCalendarModel::beginModifyItem()
  This function must be called before any item modification
*/

/**
  \fn void Calendar::AbstractCalendarModel::endModifyItem(const CalendarItem &oldItem, const CalendarItem &newItem);
  this function must be called after any item modification
*/

/**
  \fn void Calendar::AbstractCalendarModel::beginRemoveItem()
  This function must be called before any item deletion
*/

/**
  \fn void Calendar::AbstractCalendarModel::endRemoveItem(const CalendarItem &removedItem)
  This function must be called after any item deletion
*/


#include "abstract_calendar_model.h"

using namespace Calendar;

AbstractCalendarModel::AbstractCalendarModel(QObject *parent) :
    QObject(parent), m_propagateEvents(true) {
}


QVariant AbstractCalendarModel::data(const Calendar::CalendarItem &item, const int dataRef, int role) const
{
    Q_UNUSED(item);
    Q_UNUSED(dataRef);
    Q_UNUSED(role);
    return QVariant();
}

bool AbstractCalendarModel::setData(const Calendar::CalendarItem &item, const int dataRef, const QVariant &value, int role)
{
    Q_UNUSED(item);
    Q_UNUSED(dataRef);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return true;
}

bool AbstractCalendarModel::setPeopleList(const Calendar::CalendarItem &item, const QList<Calendar::People> &peoples)
{
    bool ok = true;
    for(int i=0; i < peoples.count(); ++i) {
        if (!addPeople(item, peoples.at(i)))
            ok=false;
    }
    return ok;
}

bool AbstractCalendarModel::addPeople(const Calendar::CalendarItem &item, const People &people)
{
    Q_UNUSED(item);
    Q_UNUSED(people);
    return false;
}

bool AbstractCalendarModel::removePeople(const Calendar::CalendarItem &item, const int peopleType, const QString &uid)
{
    Q_UNUSED(item);
    Q_UNUSED(peopleType);
    Q_UNUSED(uid);
    return false;
}

QList<Calendar::People> AbstractCalendarModel::peopleList(const Calendar::CalendarItem &item)
{
    Q_UNUSED(item);
    return QList<Calendar::People>();
}


void AbstractCalendarModel::beginInsertItem() {
    // does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endInsertItem(const CalendarItem &newItem) {
    if (m_propagateEvents)
        Q_EMIT itemInserted(newItem);
}

void AbstractCalendarModel::beginModifyItem() {
    // does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endModifyItem(const CalendarItem &oldItem, const CalendarItem &newItem) {
    if (m_propagateEvents)
        Q_EMIT itemModified(oldItem, newItem);
}

void AbstractCalendarModel::beginRemoveItem() {
    // does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endRemoveItem(const CalendarItem &removedItem) {
    if (m_propagateEvents)
        Q_EMIT itemRemoved(removedItem);
}

void AbstractCalendarModel::setItemIsMine(Calendar::CalendarItem *item) const {
    item->setModel(const_cast<AbstractCalendarModel*>(this));
}

//void AbstractCalendarModel::setCalendarIsMine(Calendar::UserCalendar *cal) const {
//    cal->setModel((AbstractCalendarModel*)this);
//}

//void AbstractCalendarModel::setItemByUid(const QString &, const CalendarItem &) {}

void AbstractCalendarModel::stopEvents() {
    m_propagateEvents = false;
}

void AbstractCalendarModel::resumeEvents() {
    m_propagateEvents = true;
    Q_EMIT reset();
}

