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
#ifndef AGENDAEVENTSMODEL_H
#define AGENDAEVENTSMODEL_H

#include <QAbstractTableModel>

namespace Calendar {
class AbstractCalendarModel;
}

namespace Agenda {
namespace Internal {
class AgendaEventsModelPrivate;
}

class AgendaEventsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        DateStart,
        DateEnd,
        ColumnCount
    };

    explicit AgendaEventsModel(QObject *parent = 0);
    virtual void refresh();

    void filterDateRange(const QDateTime &start, const QDateTime &end);
    void filterCalendar(const int calendarId);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Calendar::AbstractCalendarModel *toAbstractCalendarModel();

public Q_SLOTS:
    virtual bool submitAll();
    virtual bool submitIndex(const QModelIndex &item);
    virtual bool revertAll();
    virtual bool revertIndex(const QModelIndex &item);

private:
    Internal::AgendaEventsModelPrivate *d;
};

}  // End namespace Agenda

#endif // AGENDAEVENTSMODEL_H
