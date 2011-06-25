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
#include "calendaritemmodel.h"
#include "agendabase.h"
#include "constants.h"

#include <calendar/calendar_item.h>
#include <calendar/usercalendar.h>

using namespace Agenda;
using namespace Internal;

//static inline Core::IUser *user() {return Core::ICore::instance()->user();}
//static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}

CalendarItemModel::CalendarItemModel(QObject *parent)
{
    // TEST
    // get all items from database
    CalendarEventQuery query;
    query.setDateRangeForCurrentWeek();
    QList<Calendar::CalendarItem *> items = base()->getCalendarEvents(query);
    for(int i = 0; i < items.count(); ++i) {
        Calendar::CalendarItem *item = items.at(i);
        m_sortedByBeginList.insert(getInsertionIndex(true, item->beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), item);
        m_sortedByEndList.insert(getInsertionIndex(false, item->ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), item);
    }
    // END TEST
}

CalendarItemModel::~CalendarItemModel()
{
    // pointers are duplicated between m_sortedByBeginList and m_sortedByEndList so delete them once
    qDeleteAll(m_sortedByBeginList);
}

Calendar::CalendarItem CalendarItemModel::getItemByUid(const QString &uid) const
{
    Calendar::CalendarItem *item = getItemPointerByUid(uid);
    return item ? *item : Calendar::CalendarItem();
}

QList<Calendar::CalendarItem> CalendarItemModel::getItemsBetween(const QDate &from, const QDate &to) const
{
    Q_ASSERT_X(from <= to, "CalendarItemModel::getItemsBetween", "<from> is strictly greater than <to>");

    QList<Calendar::CalendarItem> list;
    QMap<Calendar::CalendarItem*, bool> added;

    int pivot = searchForIntersectedItem(m_sortedByBeginList, from, to, 0, m_sortedByBeginList.count() - 1);

    if (pivot == -1)
            return list;

    list << *m_sortedByBeginList[pivot];
    added.insert(m_sortedByBeginList[pivot], true);

    // search backward...
    for (int i = pivot - 1; i >= 0; i--) {
            if (m_sortedByBeginList[i]->intersects(from, to))
                    break;
            list << *m_sortedByBeginList[i];
            added.insert(m_sortedByBeginList[i], true);
    }

    // ... and forward
    for (int i = pivot + 1; i < m_sortedByBeginList.count(); i++) {
            if (m_sortedByBeginList[i]->intersects(from, to))
                    break;
            list << *m_sortedByBeginList[i];
            added.insert(m_sortedByBeginList[i], true);
    }

    // now, recompute the pivot and search in sorted by ending
    pivot = m_sortedByEndList.indexOf(m_sortedByBeginList[pivot]);

    // search backward...
    for (int i = pivot - 1; i >= 0; i--) {
            if (added[m_sortedByEndList[i]])
                    continue;
            if (m_sortedByEndList[i]->intersects(from, to))
                    break;
            list << *m_sortedByEndList[i];
    }

    // ... and forward
    for (int i = pivot + 1; i < m_sortedByEndList.count(); i++) {
            if (added[m_sortedByEndList[i]])
                    continue;
            if (m_sortedByEndList[i]->intersects(from, to))
                    break;
            list << *m_sortedByEndList[i];
    }


    return list;
}

int CalendarItemModel::count() const
{
    return m_sortedByBeginList.count();
}

const Calendar::CalendarItem &CalendarItemModel::insertItem(const QDateTime &beginning, const QDateTime &ending)
{
    if (m_propagateEvents)
            beginInsertItem();

    // create the item once but insert it in two lists
    Calendar::CalendarItem *item = new Calendar::CalendarItem(createUid(), beginning, ending);

    m_sortedByBeginList.insert(getInsertionIndex(true, beginning, m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), item);
    m_sortedByEndList.insert(getInsertionIndex(false, ending, m_sortedByEndList, 0, m_sortedByEndList.count() - 1), item);

    if (m_propagateEvents)
            endInsertItem(*item);

    return *item;
}

Calendar::CalendarItem CalendarItemModel::addCalendarItem(const Calendar::CalendarItem &item)
{
    // already in list ? -> update item
    Calendar::CalendarItem *oldItem = getItemPointerByUid(item.uid());
    if (oldItem) {
        if (updateCalendarItem(item))
            return item;
    }
    // create new item
    beginInsertItem();

    // create the item once but insert it in two lists
    Calendar::CalendarItem *pItem = new Calendar::CalendarItem(item);
    pItem->setData(Calendar::CalendarItem::Uid, createUid());

    m_sortedByBeginList.insert(getInsertionIndex(true, item.beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), pItem);
    m_sortedByEndList.insert(getInsertionIndex(false, item.ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), pItem);

    endInsertItem(*pItem);
    return *pItem;
}

bool CalendarItemModel::updateCalendarItem(const Calendar::CalendarItem &item)
{
    setItemByUid(item.uid(), item);
    return true;
}

void CalendarItemModel::setItemByUid(const QString &uid, const Calendar::CalendarItem &item)
{
    // remove the old item
    Calendar::CalendarItem *oldItem = getItemPointerByUid(uid);
    if (!oldItem)
            return;

    beginModifyItem();

    m_sortedByBeginList.removeAt(m_sortedByBeginList.indexOf(oldItem));
    m_sortedByEndList.removeAt(m_sortedByEndList.indexOf(oldItem));

    // create the item once but insert it in two lists
    Calendar::CalendarItem *pItem = new Calendar::CalendarItem(item);

    m_sortedByBeginList.insert(getInsertionIndex(true, item.beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), pItem);
    m_sortedByEndList.insert(getInsertionIndex(false, item.ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), pItem);

    endModifyItem(*oldItem, *pItem);

    delete oldItem;
}

void CalendarItemModel::removeItem(const QString &uid)
{
    // remove the old item
    Calendar::CalendarItem *oldItem = getItemPointerByUid(uid);
    if (!oldItem)
            return;

    beginRemoveItem();

    m_sortedByBeginList.removeAt(m_sortedByBeginList.indexOf(oldItem));
    m_sortedByEndList.removeAt(m_sortedByEndList.indexOf(oldItem));

    endRemoveItem(*oldItem);

    delete oldItem;
}

void CalendarItemModel::stopEvents()
{
    Calendar::AbstractCalendarModel::stopEvents();
}

void CalendarItemModel::resumeEvents()
{
    Calendar::AbstractCalendarModel::resumeEvents();
}

void CalendarItemModel::clearAll()
{
    qDeleteAll(m_sortedByBeginList);
    m_sortedByBeginList.clear();
    m_sortedByEndList.clear();
    if (m_propagateEvents)
            emit reset();
}

void CalendarItemModel::beginInsertItem()
{
    Calendar::AbstractCalendarModel::beginInsertItem();
}

void CalendarItemModel::endInsertItem(const Calendar::CalendarItem &newItem)
{
    Calendar::AbstractCalendarModel::endInsertItem(newItem);
}

void CalendarItemModel::beginModifyItem()
{
    Calendar::AbstractCalendarModel::beginModifyItem();
}

void CalendarItemModel::endModifyItem(const Calendar::CalendarItem &oldItem, const Calendar::CalendarItem &newItem)
{
    Calendar::AbstractCalendarModel::endModifyItem(oldItem, newItem);
}

void CalendarItemModel::beginRemoveItem()
{
    Calendar::AbstractCalendarModel::beginRemoveItem();
}

void CalendarItemModel::endRemoveItem(const Calendar::CalendarItem &removedItem)
{
    Calendar::AbstractCalendarModel::endRemoveItem(removedItem);
}


////// PRIVATE PART
int CalendarItemModel::searchForIntersectedItem(const QList<Calendar::CalendarItem*> &list, const QDate &from, const QDate &to, int first, int last) const
{
    if (last == -1) // 0 items
        return -1;

    if (first == last) { // only one item, left or right?
        if (list[first]->intersects(from, to))
            return -1;
        return first;
    }

    // at least two items
    int middle = first + (last - first) / 2;
    int v = list[middle]->intersects(from, to);
    if (v > 0)
        return searchForIntersectedItem(list, from, to, first, middle); // if an intersecting item really exists, it will be in the left part
    else if (v < 0)
        return searchForIntersectedItem(list, from, to, middle + 1, last); // if an intersecting item really exists, it will be in the right part

    return middle; // intersection => found!
}

int CalendarItemModel::getInsertionIndex(bool begin, const QDateTime &dateTime, const QList<Calendar::CalendarItem*> &list, int first, int last) const
{
    if (last == - 1) // 0 items
        return 0;

    if (first == last) { // only one item, left or right?
        const QDateTime &curDateTime = begin ? list[first]->beginning() : list[first]->ending();
        if (dateTime < curDateTime)
            return first;
        else
            return first + 1;
    }

    // at least two items
    int middle = first + (last - first) / 2;
    const QDateTime &curDateTime = begin ? list[middle]->beginning() : list[middle]->ending();
    if (dateTime < curDateTime)
        return getInsertionIndex(begin, dateTime, list, first, middle); // insertion index is in left part
    return getInsertionIndex(begin, dateTime, list, middle + 1, last); // insertion index is in right part
}

Calendar::CalendarItem *CalendarItemModel::getItemPointerByUid(const QString &uid) const
{
    // TODO: optimize it
    foreach (Calendar::CalendarItem *item, m_sortedByBeginList)
        if (item->uid() == uid)
            return item;
    return 0;
}

QString CalendarItemModel::createUid() const
{
    // at first, get the date
    QString nowStr = QDateTime::currentDateTime().toString("yyyyMMddThhmmsszz");
    int index = 0;
    QString propal;
    do {
        if (!index)
            propal = nowStr;
        else
            propal = nowStr + QString("-%1").arg(index);
        index++;
    } while (getItemByUid(propal).isValid());
    return propal;
}
