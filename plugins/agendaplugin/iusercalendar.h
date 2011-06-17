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
#ifndef IUSERCALENDAR_H
#define IUSERCALENDAR_H

#include <agendaplugin/agenda_exporter.h>

#include <QVariant>

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

protected:
    virtual void setDatabaseValue(const int ref, const QVariant &value);
    virtual int calendarId() const;
    virtual QString userOwnerUid() const;
    virtual int categoryId() const;
    virtual QString xmlOptions() const;

private:
    QHash<int, QVariant> m_Datas;
    bool m_Modified;
};

}  // End namespace Agenda

#endif // IUSERCALENDAR_H
