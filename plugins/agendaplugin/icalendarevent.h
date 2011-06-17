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
 ***************************************************************************/
#ifndef ICALENDAREVENT
#define ICALENDAREVENT

#include <agendaplugin/agenda_exporter.h>

#include <QVariant>
#include <QHash>
#include <QStringList>

/**
 * \file icalendarevent.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 06 June 2011
*/

namespace Agenda {
namespace Internal {
class AgendaBase;
}

class AGENDA_EXPORT ICalendarEvent
{
    friend class Agenda::Internal::AgendaBase;

public:
    enum DataRepresentation {
        Label = 0,
        FullContent,
        TypeId,
        StatusId,
        IsPrivate,
        Password,
        IsBusy,
        IsAGroupEvent,
        TextualSite,
        UserUid,
        PatientUid,
        PatientFullName,
        DateStart,
        DateEnd,
        SiteUid,
        ThemedIcon,
        DbOnly_CalId,
        DbOnly_ComId,
        DbOnly_EvId,
        DbOnly_CycEvId,
        DbOnly_CatId,
        DbOnly_IsValid,
        DbOnly_XmlViewOptions,
        DbOnly_XmlOptions,
    };

    ICalendarEvent();
    virtual ~ICalendarEvent() {}

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    virtual void addPatient(const QString &patientUid);
    virtual QStringList patients() const;
    virtual void removePatient(const QString &patientUid);

    virtual void addUser(const QString &userUid);
    virtual QStringList users() const;
    virtual void removeUser(const QString &userUid);

    virtual bool isModified() const;
    virtual void setModified(const bool state);

    virtual bool isCycling() const {return false;}

protected:
    virtual void setDatabaseValue(const int ref, const QVariant &value);
    virtual int calendarId() const;
    virtual int commonId() const;
    virtual int eventId() const;
    virtual int cyclingEventId() const;
    virtual int categoryId() const;

private:
    QHash<int, QVariant> m_Datas;
    bool m_Modified;
    QStringList m_Patients, m_Users;
};


class AGENDA_EXPORT ICalendarCyclingEvent : public ICalendarEvent
{
public:
    enum DataRepresentation {
        RepeatTime = 1000,
        RepeatScheme,
        RepeatInterval
    };

    ICalendarCyclingEvent() : ICalendarEvent() {}
    ~ICalendarCyclingEvent() {}

    virtual bool isCycling() const {return true;}
};

}  // End namespace Agenda

#endif //  ICALENDAREVENT
