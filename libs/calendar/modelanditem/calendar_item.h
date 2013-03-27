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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <calendar/calendar_exporter.h>
#include <calendar/common.h>

#include <QVariant>
#include <QHash>
#include <QStringList>
#include <QVector>
#include <QDateTime>

#include <QDebug>

/**
 * \file calendar_item.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.0
 * \date 04 Aug 2011
*/

namespace Calendar {
    class AbstractCalendarModel;

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
    friend class Calendar::AbstractCalendarModel;

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

    CalendarItem();
    CalendarItem(const QDateTime &beginning, const QDateTime &ending);
    CalendarItem(const QString &uid, const QDateTime &beginning, const QDateTime &ending);

    virtual ~CalendarItem() {}

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    virtual bool isCycling() const {return false;}

    int intersects(const QDate &firstDay, const QDate &lastDay) const;

    bool overlap(const CalendarItem &item) const;

    QString uid() const {return m_uid;}

    QString title() const { return data(Label).toString(); }
    void setTitle(const QString &value) {setData(Label, value);}

    QDateTime beginning() const { return m_beginning; }
    void setBeginning(const QDateTime &value);

    DateType beginningType() const { return m_beginningType; }
    void setBeginningType(DateType value);

    QDateTime ending() const { return m_ending; }
    void setEnding(const QDateTime &value);

    DateType endingType() const { return m_endingType; }
    void setEndingType(DateType value);

    QString description() const { return data(Description).toString(); }
    int status() const {return data(Status).toInt();}

    // Put this in a private part ?
    QDateTime created() const { return m_created; }
    void setCreated(const QDateTime &value) {m_created=value;}

    void setDaily(bool value);

    bool operator==(const CalendarItem &other) const;

    // AbstractCalendarModel
    Calendar::AbstractCalendarModel *model() const {return m_Model;}

protected:
    void setModel(Calendar::AbstractCalendarModel *model) {m_Model=model;}

private:
    QString m_uid;
    QDateTime m_beginning;
    QDateTime m_ending;
    QDateTime m_created;
    DateType m_beginningType;
    DateType m_endingType;
    AbstractCalendarModel *m_Model;
};

bool calendarItemLessThan(const CalendarItem &item1, const CalendarItem &item2);

}  // End namespace Calendar

CALENDAR_EXPORT QDebug operator<<(QDebug dbg, const Calendar::CalendarItem &c);
CALENDAR_EXPORT QDebug operator<<(QDebug dbg, const Calendar::CalendarItem *c);


#endif
