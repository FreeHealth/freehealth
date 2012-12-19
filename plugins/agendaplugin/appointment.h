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
#ifndef AGENDA_APPOINTMENT_H
#define AGENDA_APPOINTMENT_H

#include <agendaplugin/constants.h>
#include <calendar/modelanditem/calendar_people.h>

#include <QVariant>
#include <QHash>
#include <QStringList>
#include <QVector>
#include <QDateTime>

/**
 * \file appointment.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 18 Sept 2011
*/

namespace Agenda {
namespace Internal {

class Appointment : public Calendar::CalendarPeople
{
public:
    // Data representation is the same as the Calendar::AbstractCalendarModel
    Appointment();

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual bool isModified() const {return m_Modified;}
    virtual void setModified(const bool state) {m_Modified=state;}

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    virtual bool isCycling() const {return false;}

    QDateTime beginning() const;
    QDateTime ending() const;
    int intersects(const QDate &firstDay, const QDate &lastDay) const;

    // Calendar::CalendarPeople interface
    void addPeople(const Calendar::People &people)  {setModified(true); Calendar::CalendarPeople::addPeople(people);}
    void setPeopleName(const int people, const QString &uid, const QString &name) {setModified(true); Calendar::CalendarPeople::setPeopleName(people, uid, name);}
    void removePeople(const QString &uid) {setModified(true); Calendar::CalendarPeople::removePeople(uid);}
    void clearPeople(const int people)    {setModified(true); Calendar::CalendarPeople::clearPeople(people);}

    // Private getters
    int calendarId() const {return data(Constants::Db_CalId).toInt();}
    int commonId() const {return data(Constants::Db_ComId).toInt();}
    int eventId() const {return data(Constants::Db_EvId).toInt();}
    int cyclingEventId() const {return data(Constants::Db_CyclingEvId).toInt();}
    int categoryId() const {return data(Constants::Db_CatId).toInt();}

    void setModelUid(const int uid) {m_uid = uid;}
    int modelUid() const {return m_uid;}

    static bool dateLessThan(const Appointment *item1, const Appointment *item2);

private:
    QHash<int, QVariant> m_Data;
    bool m_Modified;
    int m_uid;
};

}  // End namespace Internal
}  // End namespace Agenda

QDebug operator<<(QDebug dbg, const Agenda::Internal::Appointment &c);
QDebug operator<<(QDebug dbg, const Agenda::Internal::Appointment *c);

#endif // AGENDA_APPOINTMENT_H
