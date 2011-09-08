/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "calendaritemmodel.h"
#include "agendabase.h"
#include "constants.h"
#include "appointement.h"
#include <agendaplugin/usercalendar.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>

#include <patientbaseplugin/patientmodel.h>

#include <usermanagerplugin/usermodel.h>

#include <calendar/calendar_item.h>

#include <utils/log.h>

#include <QStandardItemModel>
#include <QStandardItem>

using namespace Agenda;
using namespace Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
//static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}

CalendarItemModel::CalendarItemModel(const QVariant &calendarUid, QObject *parent) :
    Calendar::AbstractCalendarModel(parent),
    m_CalendarUid(calendarUid)
{
    setObjectName("CalendarItemModel");
//    userChanged();
}

CalendarItemModel::~CalendarItemModel()
{
    // pointers are duplicated between m_sortedByBeginList and m_sortedByEndList so delete them once
    qDeleteAll(m_sortedByBeginList);
}

Calendar::CalendarItem CalendarItemModel::getItemByUid(const QString &uid) const
{
    Appointement *item = getItemPointerByUid(uid.toInt());

    if (!item) {
        Calendar::CalendarItem cal;
        cal.setData(Uid, createUid());
        setItemIsMine(&cal);
        return cal;
    }
    return toCalendarItem(item);
}

/** Return the filtred items between specified dates \e from and \e to. This function uses the default Agenda::UserCalendar or the first available. */
QList<Calendar::CalendarItem> CalendarItemModel::getItemsBetween(const QDate &from, const QDate &to) const
{
    Q_ASSERT_X(from <= to, "CalendarItemModel::getItemsBetween", "<from> is strictly greater than <to>");

    getItemFromDatabase(from, to, -1);

    QList<Calendar::CalendarItem> list;
    QMap<Appointement *, bool> added;

    int pivot = searchForIntersectedItem(m_sortedByBeginList, from, to, 0, m_sortedByBeginList.count() - 1);

    if (pivot == -1)
            return list;

    list << toCalendarItem(m_sortedByBeginList[pivot]);
    added.insert(m_sortedByBeginList[pivot], true);

    // search backward...
    for (int i = pivot - 1; i >= 0; i--) {
            if (m_sortedByBeginList[i]->intersects(from, to))
                    break;
            list << toCalendarItem(m_sortedByBeginList[i]);
            added.insert(m_sortedByBeginList[i], true);
    }

    // ... and forward
    for (int i = pivot + 1; i < m_sortedByBeginList.count(); i++) {
            if (m_sortedByBeginList[i]->intersects(from, to))
                    break;
            list << toCalendarItem(m_sortedByBeginList[i]);
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
            list << toCalendarItem(m_sortedByEndList[i]);
    }

    // ... and forward
    for (int i = pivot + 1; i < m_sortedByEndList.count(); i++) {
            if (added[m_sortedByEndList[i]])
                    continue;
            if (m_sortedByEndList[i]->intersects(from, to))
                    break;
            list << toCalendarItem(m_sortedByEndList[i]);
    }

    return list;
}

int CalendarItemModel::count() const
{
    return m_sortedByBeginList.count();
}

Calendar::CalendarItem CalendarItemModel::insertItem(const QDateTime &beginning, const QDateTime &ending)
{
    if (m_propagateEvents)
        beginInsertItem();

    // create the item once but insert it in two lists
    Appointement *item = new Appointement;
    item->setModelUid(createUid());
    item->setData(DateStart, beginning);
    item->setData(DateEnd, ending);
    item->setData(Constants::Db_IsValid, 1);
    item->setData(Constants::Db_CalId, m_CalendarUid);

    m_sortedByBeginList.insert(getInsertionIndex(true, beginning, m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), item);
    m_sortedByEndList.insert(getInsertionIndex(false, ending, m_sortedByEndList, 0, m_sortedByEndList.count() - 1), item);

    if (m_propagateEvents)
        endInsertItem(toCalendarItem(item));

    return toCalendarItem(item);
}

Calendar::CalendarItem CalendarItemModel::addCalendarItem(const Calendar::CalendarItem &item)
{
    // already in list ? -> update item
    Appointement *oldItem = getItemPointerByUid(item.uid().toInt());
    if (oldItem) {
        LOG_ERROR("Item already present");
//        if (updatetoCalendarItem(item))
//            return item;
    }
    // create new item
    beginInsertItem();

    // create the item once but insert it in two lists
    Appointement *pItem = new Appointement;
    pItem->setModelUid(createUid());
    pItem->setData(DateStart, item.beginning());
    pItem->setData(DateEnd, item.ending());
    pItem->setData(Constants::Db_IsValid, 1);
    pItem->setData(Constants::Db_CalId, m_CalendarUid);

    m_sortedByBeginList.insert(getInsertionIndex(true, item.beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), pItem);
    m_sortedByEndList.insert(getInsertionIndex(false, item.ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), pItem);

    endInsertItem(toCalendarItem(pItem));
    return toCalendarItem(pItem);
}

Calendar::CalendarItem CalendarItemModel::toCalendarItem(Appointement *item) const
{
    Calendar::CalendarItem c(QString::number(item->modelUid()), item->beginning(), item->ending());
    setItemIsMine(&c);
    return c;
}

void CalendarItemModel::setItemByUid(const QString &uid, const Calendar::CalendarItem &item)
{
    // remove the old item
    Appointement *oldItem = getItemPointerByUid(uid.toInt());
    if (!oldItem)
            return;

    beginModifyItem();

    m_sortedByBeginList.removeAt(m_sortedByBeginList.indexOf(oldItem));
    m_sortedByEndList.removeAt(m_sortedByEndList.indexOf(oldItem));

    // create the item once but insert it in two lists
    Appointement *pItem = new Appointement;
    pItem->setData(DateStart, item.beginning());
    pItem->setData(DateEnd, item.ending());
    pItem->setData(Constants::Db_IsValid, 1);
    pItem->setData(Constants::Db_CalId, m_CalendarUid);
    pItem->setModelUid(uid.toInt());

    m_sortedByBeginList.insert(getInsertionIndex(true, item.beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), pItem);
    m_sortedByEndList.insert(getInsertionIndex(false, item.ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), pItem);

    endModifyItem(toCalendarItem(oldItem), toCalendarItem(pItem));

    delete oldItem;
}

/** Remove a Calendar::CalendarItem according to its \e uid */
void CalendarItemModel::removeItem(const QString &uid)
{
    // remove the old item
    Appointement *oldItem = getItemPointerByUid(uid.toInt());
    if (!oldItem)
            return;
    // remove from database
    oldItem->setData(Constants::Db_IsValid, 0);
    base()->saveCalendarEvent(oldItem);

    // remove from model
    beginRemoveItem();
    m_sortedByBeginList.removeAt(m_sortedByBeginList.indexOf(oldItem));
    m_sortedByEndList.removeAt(m_sortedByEndList.indexOf(oldItem));
    endRemoveItem(toCalendarItem(oldItem));

    delete oldItem;
}

bool CalendarItemModel::moveItem(const Calendar::CalendarItem &from, Calendar::CalendarItem &to)
{
//    qWarning() << Q_FUNC_INFO;
//    qWarning() << from << to;
    if (!from.isValid())
        return false;

    Appointement *item = getItemPointerByUid(from.uid().toInt());
    if (!item) {
        return false;
    }

    Q_EMIT itemRemoved(from);
    item->setData(DateStart, to.beginning());
    item->setData(DateEnd, to.ending());
    base()->saveCalendarEvent(item);
    Q_EMIT itemInserted(toCalendarItem(item));
    return true;
}

QVariant CalendarItemModel::data(const Calendar::CalendarItem &item, int dataRef, int role) const
{
    if (!item.isValid())
        return QVariant();

    const Appointement *pItem = getItemPointerByUid(item.uid().toInt());

    if (!pItem)
        return QVariant();

    if (role==Qt::DisplayRole) {
        switch (dataRef) {
        case Uid: return item.uid();
        case DateStart: return pItem->beginning();
        case DateEnd: return pItem->ending();
        case CreatedDate: return item.created();
        default: return pItem->data(dataRef);
        }
    } else if (role==Qt::BackgroundRole && dataRef==Status) {
        int status = pItem->data(Status).toInt();
        /** \todo create preferences for color */
        switch (status) {
        case Waiting: return QColor(0,0,200); break;
        case Approved: return QColor(0,200,0); break;
        case Arrived: return QColor(0,200,200); break;
        case Changed: return QColor(0,200,100); break;
        case Cancelled: return QColor(200,100,0); break;
        case Missed: return QColor(200,0,0); break;
        }
    } else if (role==Qt::ToolTipRole) {
        getPeopleNames((Appointement *)pItem);
        QStringList names = pItem->peopleNames(Calendar::People::PeopleAttendee);
        if (names.count()) {
            QString html;
            html += QString("<b>%1</b><br />%2").arg(names.join("<br />")).arg(pItem->data(Label).toString());
            return html;
        }
        if (!pItem->data(Label).toString().isEmpty())
            return pItem->data(Label);
        return tr("Empty");
    }
    return QVariant();
}

bool CalendarItemModel::setData(const Calendar::CalendarItem &item, int dataRef, const QVariant &value, int role)
{
    if (!item.isValid())
        return false;

    if (dataRef==Uid)
        return false;

    Appointement *pItem = getItemPointerByUid(item.uid().toInt());

    if (!pItem)
        return false;

    if (pItem->data(dataRef) == value)
        return true;


    if (role==Qt::EditRole) {
        pItem->setData(dataRef, value);
        if (dataRef==DateStart || dataRef==DateEnd) {
            Q_EMIT itemModified(item, toCalendarItem(pItem));
        }
        Q_EMIT dataChanged(item);
        return true;
    }

    return false;
}

void CalendarItemModel::stopEvents()
{
    Calendar::AbstractCalendarModel::stopEvents();
}

void CalendarItemModel::resumeEvents()
{
    Calendar::AbstractCalendarModel::resumeEvents();
}

bool CalendarItemModel::setPeopleList(const Calendar::CalendarItem &item, const QList<Calendar::People> &peoples)
{
    if (!item.isValid())
        return false;

    Appointement *pItem = getItemPointerByUid(item.uid().toInt());
    pItem->setPeopleList(peoples);
    return true;
}

bool CalendarItemModel::addPeople(const Calendar::CalendarItem &item, const Calendar::People &people)
{
    if (!item.isValid())
        return false;

    Appointement *pItem = getItemPointerByUid(item.uid().toInt());
    pItem->addPeople(people);
    return true;
}

QList<Calendar::People> CalendarItemModel::peopleList(const Calendar::CalendarItem &item)
{
    if (!item.isValid())
        return QList<Calendar::People>();
    Appointement *pItem = getItemPointerByUid(item.uid().toInt());
    getPeopleNames(pItem);
    return pItem->peopleList();
}

/** Defines the UserCalendar to use as event filter according to its index in the userCalendarComboModel(). */
bool CalendarItemModel::filterUserCalendarEvents(const QVariant &calendarUid)
{
    m_CalendarUid = calendarUid;
    clearAll();
    reset();
    return true;
}

/** Clear the model and reset. All Calendar::CalendarItem will be deleted without beeing saved into database, even if they are modified. */
void CalendarItemModel::clearAll()
{
    qDeleteAll(m_sortedByBeginList);
    m_sortedByBeginList.clear();
    m_sortedByEndList.clear();
    m_RetrievedDates.clear();
    if (m_propagateEvents)
            Q_EMIT reset();
}

bool CalendarItemModel::submitAll()
{
    return base()->saveCalendarEvents(m_sortedByBeginList);
}

bool CalendarItemModel::submit(const Calendar::CalendarItem &item)
{
    if (!item.isValid())
        return false;

    Appointement *pItem = getItemPointerByUid(item.uid().toInt());

    if (!pItem)
        return false;

    return base()->saveCalendarEvent(pItem);
}

bool CalendarItemModel::revert(const Calendar::CalendarItem &item)
{
    if (!item.isValid())
        return false;

    Appointement *pItem = getItemPointerByUid(item.uid().toInt());

    if (!pItem)
        return false;

    CalendarEventQuery query;
    query.setAppointementId(pItem->data(Constants::Db_EvId));
    QList<Appointement *> a = base()->getCalendarEvents(query);

    if (a.count()==1) {
        beginModifyItem();
        m_sortedByBeginList.removeAt(m_sortedByBeginList.indexOf(pItem));
        m_sortedByEndList.removeAt(m_sortedByEndList.indexOf(pItem));

        Appointement *newItem = a.at(0);
        m_sortedByBeginList.insert(getInsertionIndex(true, newItem->beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), newItem);
        m_sortedByEndList.insert(getInsertionIndex(false, newItem->ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), newItem);

        endModifyItem(toCalendarItem(pItem), toCalendarItem(newItem));

        delete pItem;
        return true;
    }
    return false;
}


////// PRIVATE PART
int CalendarItemModel::searchForIntersectedItem(const QList<Appointement *> &list, const QDate &from, const QDate &to, int first, int last) const
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

int CalendarItemModel::getInsertionIndex(bool begin, const QDateTime &dateTime, const QList<Appointement *> &list, int first, int last) const
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

Appointement *CalendarItemModel::getItemPointerByUid(const int uid) const
{
    // TODO: optimize it
    foreach (Appointement *item, m_sortedByBeginList)
        if (item->modelUid() == uid)
            return item;
    return 0;
}

/** Create an uid only used by this model. The uid is not connected to the database but only to Calendar::CalendarItem */
int CalendarItemModel::createUid() const
{
    static int handle = 0;
    ++handle;
    return handle;
//    // at first, get the date
//    QString nowStr = QDateTime::currentDateTime().toString("yyyyMMddThhmmsszz");
//    int index = 0;
//    QString propal;
//    do {
//        if (!index)
//            propal = nowStr;
//        else
//            propal = nowStr + QString("-%1").arg(index);
//        index++;
//    } while (getItemByUid(propal).isValid());
//    return propal;
}

//void CalendarItemModel::userChanged()
//{
//    clearAll();

//    // get all UserCalendars
//    qDeleteAll(m_UserCalendar);
//    m_UserCalendar.clear();
//    m_UserCalendar = base()->getUserCalendars(user()->uuid());
//    m_UserCalendarFilterIndex = this->defaultUserCalendarComboModelIndex();

//    for(int i = 0; i < m_UserCalendar.count(); ++i) {
//        setCalendarIsMine(m_UserCalendar.at(i));
//    }

    // TEST
//    // Find default user calendar
//    Agenda::UserCalendar *cal = 0;
//    for(int i=0; i < m_UserCalendar.count(); ++i) {
//        if (m_UserCalendar.at(i)->data(Agenda::UserCalendar::IsDefault).toBool()) {
//            cal = m_UserCalendar.at(i);
//        }
//    }
//    if (!cal) {
//        LOG_ERROR("No default");
//        return;
//    }

//    // get all items from database
//    CalendarEventQuery query;
//    query.setDateRangeForCurrentYear();
//    query.setCalendarId(cal->data(Constants::Db_CalId).toInt());
//    QList<Calendar::CalendarItem *> items = base()->getCalendarEvents(query);
//    for(int i = 0; i < items.count(); ++i) {
//        Calendar::CalendarItem *item = items.at(i);
//        setItemIsMine(item);
//        m_sortedByBeginList.insert(getInsertionIndex(true, item->beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), item);
//        m_sortedByEndList.insert(getInsertionIndex(false, item->ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), item);
//    }
    // END TEST
//}

void CalendarItemModel::getItemFromDatabase(const QDate &from, const QDate &to, const int calendarId) const
{
    /** \todo code here: manage calendarId ? */
    // appointements are already available ?
    QDate testDate = from;
    bool getFullRange = true;
    QVector<QDate> getDates;
    while (true) {
        if (m_RetrievedDates.contains(testDate)) {
            getFullRange = false;
        } else {
            getDates << testDate;
        }
        if (testDate == to)
            break;
        testDate = testDate.addDays(1);
    }

    // nothing to retrieve ?
    if (!getFullRange && getDates.isEmpty()) {
        return;
    }

    // get from database needed appointements
    QList<Appointement *> items;

    CalendarEventQuery query;
//    if (m_UserCalendarFilterIndex>0 && m_UserCalendarFilterIndex<m_UserCalendar.count()) {
//        query.setCalendarId(m_UserCalendar.at(m_UserCalendarFilterIndex)->data(Constants::Db_CalId).toInt());
//    } else {
//        query.setCalendarId(defaultUserCalendar().data(Constants::Db_CalId).toInt());
//    }
    if (!m_CalendarUid.isNull()) {
        query.setCalendarId(m_CalendarUid.toInt());
    }

    if (getFullRange) {
        query.setDateRange(from, to);
        items = base()->getCalendarEvents(query);
        m_RetrievedDates << getDates;
    } else {
        for(int i=0; i < getDates.count(); ++i) {
            query.setDateRangeForDay(getDates.at(i));
            items << base()->getCalendarEvents(query);
        }
    }

    for(int i = 0; i < items.count(); ++i) {
        Appointement *item = items.at(i);
        item->setModelUid(createUid());
//        setItemIsMine(item);
        m_sortedByBeginList.insert(getInsertionIndex(true, item->beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), item);
        m_sortedByEndList.insert(getInsertionIndex(false, item->ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), item);
    }
}

void CalendarItemModel::getPeopleNames(Appointement *appointement) const
{
    for(int i = 0; i < Calendar::People::PeopleCount; ++i) {
        if (!appointement->peopleNamesPopulated(i)) {
            // get names
            switch (i) {
            case Calendar::People::PeopleUser:
            case Calendar::People::PeopleUserDelegate:
            case Calendar::People::PeopleOwner:
            {
                QHash<QString, QString> names = UserPlugin::UserModel::getUserNames(appointement->peopleUids(i, false));
                foreach(const QString &uid, names) {
                    appointement->setPeopleName(i, uid, names.value(uid));
                }
                break;
            }
            case Calendar::People::PeopleAttendee:
            {
                QHash<QString, QString> names = Patients::PatientModel::patientName(appointement->peopleUids(i, false));
                foreach(const QString &uid, names.keys()) {
                    appointement->setPeopleName(i, uid, names.value(uid));
                }
                break;
            }
            }
        }
    }
}
