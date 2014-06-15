/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
/**
  \class Agenda::UserCalendarModel
  Model for the management of user calendars.\n
  Use the Agenda::AgendaCore::userCalendarModel() to create/get models.
*/

#include "usercalendarmodel.h"
#include "usercalendar.h"
#include "agendabase.h"
#include "agendacore.h"
#include "dayavailability.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>
#include <translationutils/trans_current.h>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
//static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Agenda::Internal::AgendaBase &base() {return Agenda::AgendaCore::instance().agendaBase();}

namespace Agenda {
namespace Internal {
class UserCalendarModelPrivate
{
public:
    void getUserCalendars()
    {
        qDeleteAll(m_UserCalendars);
        m_UserCalendars.clear();
        m_UserCalendars = base().getUserCalendars(m_UserUid);
    }

public:
    QString m_UserUid;
    QList<UserCalendar*> m_UserCalendars, m_RemovedCalendars;
};
} // End namespace Internal
} // End namespace Agenda


/** Use the Agenda::AgendaCore::userCalendarModel() to create new Agenda::UserCalendarModel \sa Agenda::AgendaCore. */
UserCalendarModel::UserCalendarModel(const QString &userUid, QObject *parent) :
    QAbstractTableModel(parent),
    d(new UserCalendarModelPrivate)
{
    if (userUid.isEmpty())
        d->m_UserUid = user()->uuid();
    else
        d->m_UserUid = userUid;
    d->getUserCalendars();
}

UserCalendarModel::~UserCalendarModel()
{
    if (d)
        delete d;
    d = 0;
}

int UserCalendarModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_UserCalendars.count();
}

int UserCalendarModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant UserCalendarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= d->m_UserCalendars.count())
        return QVariant();

    const UserCalendar *u = d->m_UserCalendars.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        switch (index.column()) {
        case Uid: return u->data(Constants::Db_CalId);
        case Label: return u->data(UserCalendar::Label);
        case ExtraLabel:
        {
            if (u->isDelegated()) {
                return QString("[%1] %2")
                        .arg(u->data(UserCalendar::UserOwnerFullName).toString())
                        .arg(u->data(UserCalendar::Label).toString());
            }
            //mark default calendar with an * if there are shown more than one
            if (u->isDefault() && d->m_UserCalendars.count() > 1) {
                return QString("%1 *")
                        .arg(u->data(UserCalendar::Label).toString());
            }
            return u->data(UserCalendar::Label);
        }
        case Description: return u->data(UserCalendar::Description);
        case Type: return u->data(UserCalendar::Type);
        case Status: return u->data(UserCalendar::Status);
        case IsDefault: return u->data(UserCalendar::IsDefault);
        case IsPrivate: return u->data(UserCalendar::IsPrivate);
        case Password: return u->data(UserCalendar::Password);
        case LocationUid: return u->data(UserCalendar::LocationUid);
        case DefaultDuration: return u->data(UserCalendar::DefaultDuration);
        }
        break;
    }
    case Qt::ToolTipRole:
    {
        switch (index.column()) {
        case Label: return u->data(UserCalendar::Label);
        }
        break;
    }
    case Qt::FontRole:
    {
        if (u->isDelegated()) {
            QFont italic;
            italic.setItalic(true);
            return italic;
        } else if (u->isDefault()) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
        break;
    }
    case Qt::DecorationRole:
    {
        if (!u->data(UserCalendar::AbsPathIcon).isNull()) {
            return theme()->icon(u->data(UserCalendar::AbsPathIcon).toString());
        }
        break;
    }
    default: return QVariant();
    } // End switch role

    return QVariant();
}

bool UserCalendarModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    WARN_FUNC << index << value << role;
    if (!index.isValid())
        return false;

    if (index.row() < 0 || index.row() >= d->m_UserCalendars.count())
        return false;

    UserCalendar *userCalendar = d->m_UserCalendars.at(index.row());

    if (role==Qt::EditRole) {
        switch (index.column()) {
        case Label: userCalendar->setData(UserCalendar::Label, value); break;
        case Description: userCalendar->setData(UserCalendar::Description, value); break;
        case Type: userCalendar->setData(UserCalendar::Type, value); break;
        case Status: userCalendar->setData(UserCalendar::Status, value); break;
        case IsDefault:
        {
            bool isDefault = value.toBool();
            if (isDefault) {
                // All calendars -> not default
                foreach(UserCalendar *calendar, d->m_UserCalendars)
                    calendar->setData(UserCalendar::IsDefault, false);
            }
            userCalendar->setData(UserCalendar::IsDefault, value);
            Q_EMIT defaultAgendaChanged(index);
            break;
        }
        case IsPrivate: userCalendar->setData(UserCalendar::IsPrivate, value); break;
        case Password: userCalendar->setData(UserCalendar::Password, value); break;
        case LocationUid: userCalendar->setData(UserCalendar::LocationUid, value); break;
        case DefaultDuration: userCalendar->setData(UserCalendar::DefaultDuration, value); break;
        default: return false;
        }
//        qWarning() << "DONE";
        Q_EMIT dataChanged(index, index);
        return true;
    }

    return false;
}

/** Creates a new Agenda::UserCalendar by inserting new model rows. */
bool UserCalendarModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count);
    bool newIsDefault = (rowCount() == 0); // check if there already is an Agenda
    for(int i = 0 ; i < count; ++i) {
        UserCalendar *u = base().createEmptyCalendar(d->m_UserUid);
        u->setData(UserCalendar::IsDefault, newIsDefault);
        base().saveUserCalendar(u);
        d->m_UserCalendars.insert(row+i, u);
    }
    endInsertRows();
    return true;
}

/** Removes Agenda::UserCalendar by removing model rows. */
bool UserCalendarModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count);
    for(int i = 0 ; i < count; ++i) {
        UserCalendar *u = d->m_UserCalendars.at(row);
        u->setData(Constants::Db_IsValid, 0);
        d->m_RemovedCalendars.append(u);
        d->m_UserCalendars.removeAt(row);
    }
    endRemoveRows();
    return true;
}

/** Returns the Agenda::UserCalendar at the index \e row. Pointer must not be deleted. */
UserCalendar *UserCalendarModel::userCalendarAt(const int row) const
{
    if (row < 0 || row >= d->m_UserCalendars.count())
        return 0;
    return d->m_UserCalendars.at(row);
}

/** Returns the default Agenda::UserCalendar or the first Agenda::UserCalendar or 0 if no Agenda::UserCalendar are available. The pointer must not be deleted. */
UserCalendar *UserCalendarModel::defaultUserCalendar() const
{
    for(int i = 0; i < d->m_UserCalendars.count(); ++i) {
        UserCalendar *u = d->m_UserCalendars.at(i);
        if (u->data(UserCalendar::IsDefault).toBool()) {
            return u;
        }
    }
    if (d->m_UserCalendars.count())
        return d->m_UserCalendars.at(0);
    return 0;
}

/** Returns the QModelIndex for the default Agenda::UserCalendar or the first Agenda::UserCalendar or 0 if no Agenda::UserCalendar are available. The pointer must not be deleted. */
QModelIndex UserCalendarModel::defaultUserCalendarModelIndex() const
{
    for(int i = 0; i < d->m_UserCalendars.count(); ++i) {
        UserCalendar *u = d->m_UserCalendars.at(i);
        if (u->data(UserCalendar::IsDefault).toBool()) {
            return index(i, 0);
        }
    }
    if (d->m_UserCalendars.count())
        return index(0, 0);
    return QModelIndex();
}

/** Updates models if UserCalendar was modified outside of the model. The calendar will be saved to the database. */
void UserCalendarModel::updateUserCalendarChanged(const int row)
{
    Q_UNUSED(row);
    beginResetModel();
    // TODO: update availabities model
    endResetModel();
}

/** Returns the Agenda::DayAvailabilityModel corresponding to the calendar at the specified \e index. You have to delete the returned object. */
DayAvailabilityModel *UserCalendarModel::availabilityModel(const QModelIndex &index, QObject *parent) const
{
    if (!index.isValid())
        return 0;
    return availabilityModel(index.row(), parent);
}

/** Returns the Agenda::DayAvailabilityModel corresponding to the calendar at the specified \e index. You have to delete the returned object. */
DayAvailabilityModel *UserCalendarModel::availabilityModel(const int index, QObject *parent) const
{
    if (index < 0 || index >= d->m_UserCalendars.count())
        return 0;

    UserCalendar *u = d->m_UserCalendars.at(index);

    DayAvailabilityModel *model = new DayAvailabilityModel(parent);
    model->setUserCalendar(u);
    return model;
}

/** Sets the full list of user delegates to UserCalendars. */
void UserCalendarModel::setPeopleList(const int row, const QList<Calendar::People> &peoples)
{
    if (row < 0 || row >= d->m_UserCalendars.count())
        return;
    UserCalendar *u = d->m_UserCalendars.at(row);
    u->setPeopleList(peoples);
}

/** Adds user delegates to UserCalendars. The only allowed type of Calendar::People is Calendar::People::PeopleUserDelegate. */
void UserCalendarModel::addPeople(const int row, const Calendar::People &people)
{
    if (row < 0 || row >= d->m_UserCalendars.count())
        return;
    UserCalendar *u = d->m_UserCalendars.at(row);
    u->addPeople(people);
}

/** Removes one UserDelegate to the UserCalendar. \sa  addPeople() */
void UserCalendarModel::removePeople(const int row, const QString &uid)
{
    if (row < 0 || row >= d->m_UserCalendars.count())
        return;
    UserCalendar *u = d->m_UserCalendars.at(row);
    u->removePeople(uid);
}

/** Submits all modifications to the database. */
bool UserCalendarModel::submit()
{
    bool ok = true;
    for(int i = 0; i < d->m_UserCalendars.count(); ++i) {
        if (!base().saveUserCalendar(d->m_UserCalendars.at(i)))
                ok = false;
    }
    for(int i = 0; i < d->m_RemovedCalendars.count(); ++i) {
        if (!base().saveUserCalendar(d->m_RemovedCalendars.at(i)))
                ok = false;
    }
    return ok;
}

/** Reverts all modifications. */
void UserCalendarModel::revert()
{
    beginResetModel();
    d->getUserCalendars();
    endResetModel();
}
