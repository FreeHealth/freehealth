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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_AGENDACORE_H
#define AGENDA_AGENDACORE_H

#include <QObject>

/**
 * \file agendacore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 01 Aug 2011
*/

namespace Agenda {
class UserCalendarModel;
class CalendarItemModel;

namespace Internal {
class AgendaCorePrivate;
class AgendaBase;
class AgendaPlugin;
}  // End namespace Internal

class AgendaCore : public QObject
{
    Q_OBJECT
    friend class Agenda::Internal::AgendaPlugin;

public:
    static AgendaCore &instance();
    ~AgendaCore();

    UserCalendarModel *userCalendarModel(const QString &userUid = QString::null);
    CalendarItemModel *calendarItemModel(const QVariant &calendarUid);

    bool initializeDatabase();
    Internal::AgendaBase &agendaBase() const;

protected Q_SLOTS:
    void postCoreInitialization();

protected:
    explicit AgendaCore(QObject *parent = 0);
    void extensionsInitialized();

private:
    static AgendaCore *m_Instance;
    Internal::AgendaCorePrivate *d;
};

}  // End namespace Agenda

#endif // AGENDA_AGENDACORE_H
