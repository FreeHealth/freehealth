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
#include "icalendarevent.h"

using namespace Agenda;

ICalendarEvent::ICalendarEvent() :
        m_Modified(false)
{
    m_Datas.insert(DbOnly_CalId, -1);
    m_Datas.insert(DbOnly_ComId, -1);
    m_Datas.insert(DbOnly_EvId, -1);
    m_Datas.insert(DbOnly_CycEvId, -1);
    m_Datas.insert(DbOnly_CatId, -1);
    m_Datas.insert(DbOnly_IsValid, false);
}

bool ICalendarEvent::isValid() const
{
    /** \todo code here */
    return true;
}

bool ICalendarEvent::isNull() const
{
    /** \todo code here */
    return false;
}

QVariant ICalendarEvent::data(const int ref) const
{
    return m_Datas.value(ref, QVariant());
}

bool ICalendarEvent::setData(const int ref, const QVariant &value)
{
    /** \todo block DbOnly_ refs */
    m_Modified = true;
    m_Datas.insert(ref, value);
    return true;
}

bool ICalendarEvent::isModified() const
{
    return m_Modified;
}

void ICalendarEvent::setModified(const bool state)
{
    m_Modified=state;
}

void ICalendarEvent::addPatient(const QString &patientUid)
{
    m_Patients << patientUid;
}

QStringList ICalendarEvent::patients() const
{
    return m_Patients;
}

void ICalendarEvent::removePatient(const QString &patientUid)
{
    m_Patients.removeAll(patientUid);
}

void ICalendarEvent::addUser(const QString &userUid)
{
    m_Users << userUid;
}

QStringList ICalendarEvent::users() const
{
    return m_Users;
}

void ICalendarEvent::removeUser(const QString &userUid)
{
    m_Users.removeAll(userUid);
}

void ICalendarEvent::setDatabaseValue(const int ref, const QVariant &value)
{
    m_Modified = true;
    m_Datas.insert(ref, value);
}

int ICalendarEvent::calendarId() const
{
    return m_Datas.value(DbOnly_CalId).toInt();
}

int ICalendarEvent::commonId() const
{
    return m_Datas.value(DbOnly_ComId).toInt();
}

int ICalendarEvent::eventId() const
{
    return m_Datas.value(DbOnly_EvId).toInt();
}

int ICalendarEvent::cyclingEventId() const
{
    return m_Datas.value(DbOnly_CycEvId).toInt();
}

int ICalendarEvent::categoryId() const
{
    return m_Datas.value(DbOnly_CatId).toInt();
}
