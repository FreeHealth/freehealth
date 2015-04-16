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
#ifndef AGENDA_USERCALENDAR_H
#define AGENDA_USERCALENDAR_H

#include <agendaplugin/agenda_exporter.h>
#include <calendar/modelanditem/calendar_people.h>

#include <QVariant>
#include <QVector>
#include <QPair>
#include <QTime>
#include <QStandardItemModel>

/**
 * \file usercalendar.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 02 Aug 2011
*/

namespace Agenda {

class AbstractCalendarModel;
class DayAvailabilityModel;
class DayAvailability;

class AGENDA_EXPORT UserCalendar : public Calendar::CalendarPeople
{
    friend class Agenda::DayAvailabilityModel;
public:
    enum DataRepresentation {
        Uid = 0,
        UserOwnerUid,
        UserOwnerFullName,
        Label,
        Description,
        Type,
        Status,
        IsDefault,
        IsPrivate,
        Password,
        LocationUid,
        AbsPathIcon,
        DefaultDuration,
        SortId,
        IsDelegated,
        UserData = 10000
    };

    UserCalendar();
    virtual ~UserCalendar();

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    virtual bool isModified() const;
    virtual void setModified(const bool state);

    virtual QString xmlOptions() const;

    bool hasAvailability() const;
    QVector<DayAvailability> availabilities(const int day = -1) const;
    void addAvailabilities(const DayAvailability &av);
    void setAvailabilities(const QList<DayAvailability> &availabilities);
    void clearAvailabilities();
    void removeAvailabilitiesForWeekDay(const int weekday);
    void removeAvailabilitiesTimeRange(const int weekday, const QTime &from, const QTime &to);


    bool canBeAvailable(const QDateTime &date) const;
    bool canBeAvailable(const QDateTime &date, const int durationInMinutes) const;

    AbstractCalendarModel *model() {return m_model;}

    // Calendar::CalendarPeople interface
    void addPeople(const Calendar::People &people)  {setModified(true); Calendar::CalendarPeople::addPeople(people);}
    void setPeopleName(const int people, const QString &uid, const QString &name) {setModified(true); Calendar::CalendarPeople::setPeopleName(people, uid, name);}
    void removePeople(const QString &uid) {setModified(true); Calendar::CalendarPeople::removePeople(uid);}
    void clearPeople(const int people)    {setModified(true); Calendar::CalendarPeople::clearPeople(people);}

    // getters
    QString uid() const {return data(Uid).toString();}
    int sortId() const {return data(SortId).toInt();}
    bool isDefault() const {return data(IsDefault).toBool();}
    bool isDelegated() const {return data(IsDelegated).toBool();}

    static bool lessThan(const UserCalendar *cal1, const UserCalendar *cal2);

protected:
    void setModel(AbstractCalendarModel *model) {m_model=model;}

private:
    QHash<int, QVariant> m_data;
    bool m_modified;
    QList<DayAvailability> m_availabilities;
    AbstractCalendarModel *m_model;
};

}  // End namespace Agenda


#endif // AGENDA_USERCALENDAR_H
