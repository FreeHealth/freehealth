/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef APPOINTEMENT_H
#define APPOINTEMENT_H

#include <agendaplugin/constants.h>

#include <QVariant>
#include <QHash>
#include <QStringList>
#include <QVector>
#include <QDateTime>

namespace Agenda {
namespace Internal {

struct PeopleStructPrivate {
    PeopleStructPrivate(const int _type, const QString &_name, const QString &_uid) :
            uid(_uid), name(_name), type(_type) {}
    PeopleStructPrivate() {}

    QString uid, name;
    int type;
};

class Appointement
{
public:
    // Data representation is the same as the Calendar::AbstractCalendarModel
    Appointement();

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual bool isModified() const {return m_Modified;}
    virtual void setModified(const bool state) {m_Modified=state;}

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    // People
    virtual void addPeople(const int people, const QString &name, const QString &uid = QString::null);
    virtual void setPeopleName(const int people, const QString &uid, const QString &name);
    virtual QStringList peopleNames(const int people, bool skipEmpty = false) const;
    virtual QStringList peopleUids(const int people, bool skipEmpty = false) const;
    virtual void removePeople(const QString &uid);
    virtual void clearPeople(const int people);

    virtual bool isCycling() const {return false;}

    QDateTime beginning() const;
    QDateTime ending() const;
    int intersects(const QDate &firstDay, const QDate &lastDay) const;

    int calendarId() const {return data(Constants::Db_CalId).toInt();}
    int commonId() const {return data(Constants::Db_ComId).toInt();}
    int eventId() const {return data(Constants::Db_EvId).toInt();}
    int cyclingEventId() const {return data(Constants::Db_CyclingEvId).toInt();}
    int categoryId() const {return data(Constants::Db_CatId).toInt();}

    void setModelUid(const int uid) {m_uid = uid;}
    int modelUid() const {return m_uid;}

    static bool dateLessThan(const Appointement *item1, const Appointement *item2);

private:
    QHash<int, QVariant> m_Datas;
    QVector<Internal::PeopleStructPrivate> m_People;
    bool m_Modified;
    int m_uid;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // APPOINTEMENT_H
