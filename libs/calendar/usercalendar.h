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
#ifndef USERCALENDAR_H
#define USERCALENDAR_H

#include <calendar/calendar_exporter.h>

#include <QVariant>
QT_BEGIN_NAMESPACE
class QStandardItem;
QT_END_NAMESPACE

namespace Calendar {

class CALENDAR_EXPORT UserCalendar
{
public:
    enum DataRepresentation {
        Uid = 0,
        UserOwnerUid,
        Label,
        Description,
        Type,
        Status,
        IsDefault,
        IsPrivate,
        Password,
        LocationUid,
        AbsPathIcon,
        UserData = 10000
    };

    UserCalendar();
    virtual ~UserCalendar();

    virtual bool isValid() const;
    virtual bool isNull() const;

    virtual QVariant data(const int ref) const;
    virtual bool setData(const int ref, const QVariant &value);

    virtual bool isModified() const;
    virtual void setModified(const bool state);

    virtual QStandardItem *toStandardItem() const;

    virtual QString xmlOptions() const;

private:
    QHash<int, QVariant> m_Datas;
    bool m_Modified;
};

}  // End namespace Calendar

#endif // USERCALENDAR_H
