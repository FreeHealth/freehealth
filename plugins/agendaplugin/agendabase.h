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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_AGENDABASE_H
#define AGENDA_AGENDABASE_H

#include <utils/database.h>
#include <agendaplugin/usercalendar.h>

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QList>

/**
 * \file agendabase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date 28 Jun 2012
*/

namespace Agenda {
class AgendaCore;
namespace Internal {
class Appointment;
class NextAvailabiliyManager;

class CalendarEventQuery
{
public:
    CalendarEventQuery();
    ~CalendarEventQuery();

    /** Retreive a specific appointment, all other params are ignored */
    void setAppointmentId(const QVariant &id) {m_AppointmentId=id;}
    QVariant appointmentId() const {return m_AppointmentId;}

    void setUserFilter(const QString &userUid);
    void setPatientFilter(const QStringList &limitToPatientUids);

    void setDateRange(const QDateTime &start, const QDateTime &end) {m_DateStart=start;m_DateEnd=end;}
    void setDateRange(const QDate &start, const QDate &end);
    void setDateRangeForDay(const QDate &day);
    void setDateRangeForToday();
    void setDateRangeForTomorrow();
    void setDateRangeForYesterday();
    void setDateRangeForCurrentWeek();
    void setDateRangeForCurrentMonth();
    void setDateRangeForCurrentYear();
    void setDateStart(const QDateTime &dt) {m_DateStart = dt;}
    void setDateEnd(const QDateTime &dt) {m_DateEnd = dt;}

    void setExtractionLimit(const int max) {m_Limit = max;}
    void setStartAtItem(const int start) {m_StartItem = start;}

    /** Use the current user default calendar */
    void setUseCurrentUser(const bool use) {m_UseCurrentUser = use; m_Users.clear();}

    void setCalendarIds(const QList<int> &ids) {m_CalIds = ids;}
    void setCalendarId(const int id) {m_CalIds = QList<int>() << id;}

    QDateTime dateStart() const {return m_DateStart;}
    QDateTime dateEnd() const {return m_DateEnd;}
    bool hasDateRange() const;

    int limit() const {return m_Limit;}
    int startItem() const {return m_StartItem;}
    QStringList usersUuid() const {return m_Users;}
    bool useCurrentUser() const {return m_UseCurrentUser;}

    QList<int> calendarIds() const {return m_CalIds;}
    int calendarId() const {if (m_CalIds.count()>=1) return m_CalIds.at(0); return -1;}

private:
    QVariant m_AppointmentId;
    int m_Limit, m_StartItem;
    QDateTime m_DateStart, m_DateEnd;
    bool m_UseCurrentUser;
    QList<int> m_CalIds;
    QStringList m_Users, m_Patients;
};

class AgendaBase :  public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Agenda::AgendaCore;

protected:
    AgendaBase(QObject *parent = 0);

public:
    enum RelatedEventFor {
        RelatedToCalendar = 0,
        RelatedToAppointment
    };

    virtual ~AgendaBase();

    // initialize
    bool initialize();
    bool isInitialized() const;
    bool createDatabase(const QString &connectionName, const QString &dbName,
                        const QString &pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString &login, const QString &pass,
                        const int port,
                        CreationOption createOption
                       );
    bool checkDatabaseVersion();

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

public:
    static Agenda::UserCalendar *createEmptyCalendar(const QString &userUid);
    bool hasCalendar(const QString &userUuid = QString::null);
    QList<Agenda::UserCalendar *> getUserCalendars(const QString &userUuid = QString::null);
    bool saveUserCalendar(Agenda::UserCalendar *calendar);

    QList<Appointment *> getCalendarEvents(const CalendarEventQuery &query);
    bool saveCalendarEvents(const QList<Appointment *> &events);
    bool saveCalendarEvent(Appointment *event);

    QList<QDateTime> nextAvailableTime(const QDateTime &startSearch, const int durationInMinutes, const Agenda::UserCalendar &calendar, const int numberOfDates);
    QDateTime nextAvailableTime(const QDateTime &startSearch, const int durationInMinutes, const Agenda::UserCalendar &calendar);

    bool getPatientNames(const QList<Appointment *> &items);
    bool getPatientNames(Appointment *item);

    UserCalendar *createVirtualUserCalendar(const QString &ownerUid, const QString &label, const QString &description,
                                   int defaultDurationInMinutes, int sortId,
                                   int type, int status, bool isDefault, bool isPrivate,
                                   const QString &password, const QString &iconPath,
                                   const QList<Calendar::People> &peoples);

private:
    bool saveCalendarAvailabilities(Agenda::UserCalendar *calendar);
    bool saveRelatedPeoples(RelatedEventFor relatedToCalendar, const int eventOrCalendarId, const Calendar::CalendarPeople *peopleClass);
    bool saveCommonEvent(Appointment *event);
    bool saveNonCyclingEvent(Appointment *event);
    bool getRelatedPeoples(RelatedEventFor relatedTo, const int eventOrCalendarId, Calendar::CalendarPeople *event);

private:
    bool m_initialized;
    NextAvailabiliyManager *m_Next;

};

}  // End namespace Internal
}  // End namespace UserPlugin

#endif // AGENDA_AGENDABASE_H
