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
#include "agendaeventsmodel.h"
#include "agendabase.h"

#include <utils/widgets/calendar/basic_calendar_model.h>

using namespace Agenda;
using namespace Internal;

static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}


namespace Agenda {
namespace Internal {
    class AgendaEventsModelPrivate
    {
    public:
        AgendaEventsModelPrivate(AgendaEventsModel *parent) : q(parent)
        {}

        ~AgendaEventsModelPrivate()
        {}

        void getFromDatabase(const QDateTime &start, const QDateTime &end)
        {
            CalendarEventQuery query;
            query.setDateRange(start, end);
            m_Events = base()->getCalendarEvents(query);
        }

    public:
        QList<ICalendarEvent*> m_Events;

    private:
        AgendaEventsModel *q;
    };
}
}

AgendaEventsModel::AgendaEventsModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::AgendaEventsModelPrivate(this))
{
}

void AgendaEventsModel::refresh()
{
}

void AgendaEventsModel::filterDateRange(const QDateTime &start, const QDateTime &end)
{
}

void AgendaEventsModel::filterCalendar(const int calendarId)
{
}

int AgendaEventsModel::rowCount(const QModelIndex &parent) const
{
    return d->m_Events.count();
}

int AgendaEventsModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

Qt::ItemFlags AgendaEventsModel::flags(const QModelIndex &index) const
{
}

QVariant AgendaEventsModel::data(const QModelIndex &item, int role) const
{
}

bool AgendaEventsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
}

Calendar::AbstractCalendarModel *AgendaEventsModel::toAbstractCalendarModel()
{
}

bool AgendaEventsModel::submitAll()
{
}

bool AgendaEventsModel::submitIndex(const QModelIndex &item)
{
}

bool AgendaEventsModel::revertAll()
{
}

bool AgendaEventsModel::revertIndex(const QModelIndex &item)
{
}

