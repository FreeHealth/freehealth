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
#ifndef AGENDABASE_H
#define AGENDABASE_H

#include <utils/database.h>

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QList>


/**
 * \file agendabase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 17 Jun 2011
*/

namespace Agenda {
class ICalendarEvent;
class ICalendarCyclingEvent;
class IUserCalendar;

namespace Internal {

class CalendarEventQuery
{
public:
    CalendarEventQuery();
    ~CalendarEventQuery();

    void setUserFilter(const QString &userUid);
    void setPatientFilter(const QStringList &limitToPatientUids);

    void setDateRange(const QDateTime &start, const QDateTime &end) {m_DateStart=start;m_DateEnd=end;}
    void setDateRangeForToday();
    void setDateRangeForTomorrow();
    void setDateRangeForYesterday();
    void setDateRangeForCurrentWeek();
    void setDateStart(const QDateTime &dt) {m_DateStart = dt;}
    void setDateEnd(const QDateTime &dt) {m_DateEnd = dt;}

    void setExtractionLimit(const int max) {m_Limit = max;}
    void setStartAtItem(const int start) {m_StartItem = start;}
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
    int m_Limit, m_StartItem;
    QDateTime m_DateStart, m_DateEnd;
    bool m_UseCurrentUser;
    QList<int> m_CalIds;
    QStringList m_Users, m_Patients;
};

class AgendaBase :  public QObject, public Utils::Database
{
    Q_OBJECT

protected:
    AgendaBase(QObject *parent = 0);

public:
    // Constructor
    static AgendaBase *instance();
    virtual ~AgendaBase() {}

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

public:
    QList<Agenda::IUserCalendar *> getUserCalendars(const QString &userUuid = QString::null);
    bool saveUserCalendar(IUserCalendar *calendar);

    QList<ICalendarEvent *> getCalendarEvents(const CalendarEventQuery &query);
    bool saveCalendarEvents(const QList<Agenda::ICalendarEvent *> &events);
    bool saveCalendarEvent(Agenda::ICalendarEvent *event);

private:
    bool updateCyclingEvent(Agenda::ICalendarCyclingEvent *event);
    bool updateNonCyclingEvent(Agenda::ICalendarEvent *event);
    bool saveCommonEvent(Agenda::ICalendarEvent *event);
    bool saveCyclingEvent(Agenda::ICalendarCyclingEvent *event);
    bool saveNonCyclingEvent(Agenda::ICalendarEvent *event);

private:
    static bool m_initialized;
    static AgendaBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace UserPlugin

#endif // AGENDABASE_H
