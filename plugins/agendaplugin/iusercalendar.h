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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IUSERCALENDAR_H
#define IUSERCALENDAR_H

#include <agendaplugin/agenda_exporter.h>

#include <QVariant>
QT_BEGIN_NAMESPACE
class QStandardItem;
QT_END_NAMESPACE

namespace Agenda {
namespace Internal {
class AgendaBase;
}

class AGENDA_EXPORT IUserCalendar
{
    friend class Agenda::Internal::AgendaBase;

public:
    enum DataRepresentation {
        UserOwnerUid = 0,
        Label,
        FullContent,
        TypeId,
        StatusId,
        IsDefault,
        IsPrivate,
        Password,
        ThemedIcon,
        DbOnly_CalId,
        DbOnly_CatId,
        DbOnly_UserCalId,
        DbOnly_IsValid
    };

    IUserCalendar();
    virtual ~IUserCalendar();

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    virtual bool isModified() const;
    virtual void setModified(const bool state);

    virtual QStandardItem *toStandardItem() const;
    virtual int calendarId() const;

protected:
    virtual void setDatabaseValue(const int ref, const QVariant &value);
    virtual QString userOwnerUid() const;
    virtual int categoryId() const;
    virtual QString xmlOptions() const;

private:
    QHash<int, QVariant> m_Datas;
    bool m_Modified;
};

}  // End namespace Agenda

#endif // IUSERCALENDAR_H
