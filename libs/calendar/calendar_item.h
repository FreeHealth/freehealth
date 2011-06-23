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
#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <calendar/calendar_exporter.h>

#include <QVariant>
#include <QHash>
#include <QStringList>
#include <QVector>
#include <QDateTime>

#include "common.h"

namespace Calendar {
namespace Internal {

struct CALENDAR_EXPORT PeopleStructPrivate {
    PeopleStructPrivate(const int _type, const QString &_name, const QString &_uid) :
            uid(_uid), name(_name), type(_type) {}
    PeopleStructPrivate() {}

    QString uid, name;
    int type;
};
}  // End namespace Internal

class CALENDAR_EXPORT CalendarItem
{
public:
    enum DataRepresentation {
        Uid  = 0,
        Label,
        Description,
        Type,
        Status,
        IsPrivate,
        Password,
        IsBusy,
        IsAGroupEvent,
        DateStart,
        DateEnd,
        Location,
        LocationUid,
        IconPath,
        CreatedDate,
        UserData = 10000
    };

    enum PeopleType {
        PeopleAttendee = 0,
        PeopleOwner,
        PeopleUser
    };


    CalendarItem();
    CalendarItem(const QString &uid, const QDateTime &beginning, const QDateTime &ending);

    virtual ~CalendarItem() {}

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual bool isModified() const;
    virtual void setModified(const bool state);

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    // People
    virtual void addPeople(const PeopleType people, const QString &name, const QString &uid = QString::null);
    virtual QStringList peopleNames(const PeopleType people = PeopleAttendee) const;
    virtual QStringList peopleUids(const PeopleType people = PeopleAttendee) const;
    virtual void removePeople(const QString &uid);
    virtual void clearPeople(const PeopleType people = PeopleAttendee);

    virtual bool isCycling() const {return false;}

    int intersects(const QDate &firstDay, const QDate &lastDay) const;

    bool overlap(const CalendarItem &item) const;

    QString uid() const { return data(Uid).toString(); }

    QString title() const { return data(Label).toString(); }
    void setTitle(const QString &value) {setData(Label, value);}

    QDateTime beginning() const { return data(DateStart).toDateTime(); }
    void setBeginning(const QDateTime &value) {setData(DateStart, value);}

    DateType beginningType() const { return m_beginningType; }
    void setBeginningType(DateType value);

    QDateTime ending() const { return data(DateEnd).toDateTime(); }
    void setEnding(const QDateTime &value) {setData(DateEnd, value);}

    DateType endingType() const { return m_endingType; }
    void setEndingType(DateType value);

    QString description() const { return data(Description).toString(); }
    void setDescription(const QString &value) {setData(Description, value);}

    // Put this in a private part ?
    QDateTime created() const { return data(CreatedDate).toDateTime(); }
    void setCreated(const QDateTime &value) {setData(CreatedDate,value);}

    void setDaily(bool value);

private:
    QHash<int, QVariant> m_Datas;
    QVector<Internal::PeopleStructPrivate> m_People;
    DateType m_beginningType;
    DateType m_endingType;
    bool m_Modified;
};

bool calendarItemLessThan(const CalendarItem &item1, const CalendarItem &item2);


}  // End namespace Calendar

#endif
