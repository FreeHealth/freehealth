/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CALENDARPEOPLE_H
#define CALENDARPEOPLE_H

#include <calendar/calendar_exporter.h>

#include <QString>
#include <QList>
#include <QAbstractTableModel>

/**
 * \file calendar_people.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 04 Aug 2011
*/

namespace Calendar {

struct CALENDAR_EXPORT People {
    // Keep this enum sync with Calendar::AbstractCalendarModel::PeopleType
    enum TypeOfPeople {
        PeopleAttendee = 0,
        PeopleOwner,
        PeopleUser,
        PeopleUserDelegate,
        PeopleCount
    };

    People(const TypeOfPeople _type, const QString &_name, const QString &_uid) :
            uid(_uid), name(_name), type(_type) {}
    People() : type(PeopleAttendee) {}

    bool operator==(const People &other) const
    {
        if (type==other.type)
            return (uid==other.uid);
        return false;
    }

    QString uid, name;
    int type;
};

class CalendarPeopleModel;

class CALENDAR_EXPORT CalendarPeople
{
    friend class Calendar::CalendarPeopleModel;
public:
    CalendarPeople();
    virtual ~CalendarPeople();

    // People
    virtual void setPeopleList(const QList<Calendar::People> &peoples);
    virtual void addPeople(const Calendar::People &people);
    virtual void insertPeople(const int index, const Calendar::People &people);
    virtual void setPeopleName(const int people, const QString &uid, const QString &name);
    virtual void removePeople(const QString &uid);
    virtual void clearPeople(const int people);

    virtual int peopleCount(const int type = -1) const;
    virtual QStringList peopleNames(const int people, bool skipEmpty = false) const;
    virtual QStringList peopleUids(const int people, bool skipEmpty = false) const;
    virtual bool peopleNamesPopulated(const int peopleType) const;
    virtual QList<Calendar::People> peopleList() const {return m_People;}

protected:
    QList<Calendar::People> m_People;
};


class CALENDAR_EXPORT CalendarPeopleModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation { PeopleTypeName = 0, FullName , Uid, EmptyColumn };

    CalendarPeopleModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void clear();

    QVariant data(const QModelIndex &idx, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void addPeople(const Calendar::People &people);
    void removePeople(const QString &uid);
    void setPeopleList(const QList<Calendar::People> &list);
    QList<Calendar::People> peopleList() const;
    bool contains(Calendar::People &person) const;

private:
    QString typeToString(const int type) const;

    QList<Calendar::People> m_People;
};

}  // End namespace Calendar


#endif // CALENDARPEOPLE_H
