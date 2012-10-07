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
#ifndef USERCALENDARMODEL_H
#define USERCALENDARMODEL_H

#include <calendar/modelanditem/calendar_people.h>

#include <QAbstractTableModel>
#include <QStandardItemModel>

/**
 * \file usercalendarmodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 18 Nov 2011
*/

namespace Agenda {
class AgendaCore;
class DayAvailabilityModel;
class DayAvailability;
class UserCalendar;

namespace Internal {
class UserCalendarModelPrivate;
} // End namespace Internal

class UserCalendarModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class Agenda::AgendaCore;

protected:
    explicit UserCalendarModel(const QString &userUid = QString::null, QObject *parent = 0);

public:
    enum DataRepresentation {
        Label = 0,
        ExtraLabel, // with user delegation
        Description,
        Type,
        Status,
        IsDefault,
        IsPrivate,
        Password,
        LocationUid,
        DefaultDuration,
        Uid,
        ColumnCount
    };

    ~UserCalendarModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    UserCalendar *userCalendarAt(const int row) const;
    UserCalendar *defaultUserCalendar() const;
    QModelIndex defaultUserCalendarModelIndex() const;
    void updateUserCalendarChanged(const int row);

    DayAvailabilityModel *availabilityModel(const QModelIndex &index, QObject *parent = 0) const;
    DayAvailabilityModel *availabilityModel(const int index, QObject *parent = 0) const;

    void setPeopleList(const int row, const QList<Calendar::People> &peoples);
    void addPeople(const int row, const Calendar::People &people);
    void removePeople(const int row, const QString &uid);

public Q_SLOTS:
    bool submit();
    void revert();

private:
    Internal::UserCalendarModelPrivate *d;
};


namespace Internal {
class DayAvailabilityModelPrivate;
}

class DayAvailabilityModel : public QStandardItemModel
{
    Q_OBJECT
public:
    DayAvailabilityModel(QObject *parent = 0);
    ~DayAvailabilityModel();

    void clearAvailabilities();

    void setUserCalendar(UserCalendar *calendar);
    void addAvailability(const DayAvailability &availability);
    void removeAvailability(const QModelIndex &index);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

public Q_SLOTS:
    bool submit();

private:
    Internal::DayAvailabilityModelPrivate *d;
};


} // End namespace Agenda

#endif // USERCALENDARMODEL_H
