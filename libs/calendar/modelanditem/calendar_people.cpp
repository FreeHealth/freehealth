/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \struct Calendar::People
  Simple structure to store a person refence. \sa Calendar::CalendarPeople
*/

/**
  \class Calendar::CalendarPeople
  Simple structure to manage people associated with any type of agenda classes (Appointment, UserCalendar...).\n
  The type of people is referenced in Calendar::AbstractItemModel::PeopleType.\n
  Inherite this class to add people management functionalities.
*/

#include "calendar_people.h"

#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>
#include <translationutils/trans_user.h>

#include <QStringList>

#include <QDebug>

using namespace Calendar;
using namespace Trans::ConstantTranslations;

CalendarPeople::CalendarPeople()
{}

CalendarPeople::~CalendarPeople()
{}

void CalendarPeople::setPeopleList(const QList<Calendar::People> &peoples)
{
    m_People.clear();
    m_People = peoples;
}

void CalendarPeople::addPeople(const People &people)
{
    m_People.append(people);
}

void CalendarPeople::insertPeople(const int index, const Calendar::People &people)
{
    m_People.insert(index, people);
}

void CalendarPeople::setPeopleName(const int people, const QString &uid, const QString &name)
{
    for(int i = 0; i < m_People.count(); ++i) {
        const People &p = m_People.at(i);
        if (p.type==people && p.uid==uid) {
            m_People[i].name = name;
        }
    }
}

QStringList CalendarPeople::peopleNames(const int people, bool skipEmpty) const
{
    QStringList toReturn;
    for(int i = 0; i < m_People.count(); ++i) {
        const People &p = m_People.at(i);
        if (p.type == people) {
            if (skipEmpty) {
                if (p.name.isEmpty())
                    continue;
            }
            toReturn << p.name;
        }
    }
    return toReturn;
}

QStringList CalendarPeople::peopleUids(const int people, bool skipEmpty) const
{
    QStringList toReturn;
    for(int i = 0; i < m_People.count(); ++i) {
        if (m_People.at(i).type == people) {
            if (skipEmpty) {
                if (m_People.at(i).uid.isEmpty())
                    continue;
            }
            toReturn << m_People.at(i).uid;
        }
    }
    return toReturn;
}

bool CalendarPeople::peopleNamesPopulated(const int peopleType) const
{
    for(int i = 0; i < m_People.count(); ++i) {
        const People &p = m_People.at(i);
        if (p.type == peopleType) {
            if (!p.uid.isEmpty() && p.name.isEmpty()) {
                return false;
            }
        }
    }
    return true;
}

//void CalendarPeople::removePeople(const int index)
//{
//    if (index > 0 && index < m_People.count())
//        m_People.removeAt(index);
//}

void CalendarPeople::removePeople(const QString &uid)
{
    for(int i = 0; i < m_People.count(); ++i) {
        if (m_People.at(i).uid==uid) {
            m_People.removeAt(i);
            break;
        }
    }
}

/** Remove all type of people, if you want to clear all people, set \e people arg to -1 */
void CalendarPeople::clearPeople(const int people)
{
    if (people==-1) {
        m_People.clear();
        return;
    }
    for(int i = m_People.count(); i > -1; --i) {
        if (m_People.at(i).type==people) {
            m_People.removeAt(i);
        }
    }
}

/** Return the number of total people according to their type. \e type = -1 --> all types. */
int CalendarPeople::peopleCount(const int type) const
{
    if (type==-1)
        return m_People.count();
    int total = 0;
    for(int i = 0; i < m_People.count(); ++i) {
        if (m_People.at(i).type==type) {
            ++total;
        }
    }
    return total;
}

CalendarPeopleModel::CalendarPeopleModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int CalendarPeopleModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return m_People.count();
}

int CalendarPeopleModel::columnCount(const QModelIndex &) const
{
    return 4;
}

void CalendarPeopleModel::clear()
{
    m_People.clear();
    reset();
}

QVariant CalendarPeopleModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid())
        return QVariant();

    if (idx.column()==EmptyColumn)
        return QVariant();

    if (idx.column()==FullName) {
        if (role==Qt::DisplayRole || role==Qt::EditRole || role==Qt::ToolTipRole) {
            return m_People.at(idx.row()).name;
        }
    } else if (idx.column()==Uid && role==Qt::DisplayRole) {
        return m_People.at(idx.row()).uid;
    } else if (idx.column()==PeopleTypeName && role==Qt::DisplayRole) {
        return typeToString(m_People.at(idx.row()).type);
    }

    return QVariant();
}

bool CalendarPeopleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (index.column()==FullName && role==Qt::EditRole) {
            m_People[index.row()].name = value.toString();
            Q_EMIT dataChanged(index, index);
            return true;
    } else if (index.column()==Uid && role==Qt::EditRole) {
        m_People[index.row()].uid = value.toString();
        Q_EMIT dataChanged(index, index);
        return true;
    }
    return false;
}

bool CalendarPeopleModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count);
    for(int i = 0; i < count; ++i) {
        Calendar::People p;
        m_People.insert(row+i, p);
    }
    endInsertRows();
    return true;
}

bool CalendarPeopleModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count);
    for(int i = 0; i < count; ++i) {
        if (row < m_People.count()) {
            m_People.removeAt(row);
        }
    }
    endRemoveRows();
    return true;
}

void CalendarPeopleModel::addPeople(const Calendar::People &people)
{
    m_People.append(people);
    reset();
}

void CalendarPeopleModel::removePeople(const QString &uid)
{
    for(int i = m_People.count(); i > -1 ; --i) {
        if (m_People.at(i).uid==uid)
            m_People.removeAt(i);
    }
}

void CalendarPeopleModel::setPeopleList(const QList<Calendar::People> &list)
{
    m_People = list;
    reset();
}

QList<Calendar::People> CalendarPeopleModel::peopleList() const
{
    return m_People;
}

bool CalendarPeopleModel::contains(People &person) const
{
    return m_People.contains(person);
}

QString CalendarPeopleModel::typeToString(const int type) const
{
    switch (type) {
    case Calendar::People::PeopleAttendee: return tkTr(Trans::Constants::ATTENDEE);
    case Calendar::People::PeopleOwner: return tkTr(Trans::Constants::OWNER);
    case Calendar::People::PeopleUser: return tkTr(Trans::Constants::USER);
    case Calendar::People::PeopleUserDelegate: return tkTr(Trans::Constants::USER_DELEGATES);
    }
    return QString();
}
