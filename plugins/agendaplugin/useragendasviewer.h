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
#ifndef USERAGENDASVIEWER_H
#define USERAGENDASVIEWER_H

#include <QWidget>

namespace Calendar {
    class UserCalendar;
}

namespace Agenda {

namespace Internal {
class UserAgendasViewerPrivate;

class UserAgendasViewer : public QWidget
{
    Q_OBJECT

public:
    enum AgendaOwner {
        OwnAgendas = 0,
        DelegatedAgendas
    };

    explicit UserAgendasViewer(QWidget *parent = 0);
    ~UserAgendasViewer();

private Q_SLOTS:
    void newEventAtAvailabity(QAction *action);
    void recalculateAvailabilitiesWithDurationIndex(const int index);
    void on_availableAgendasCombo_activated(const int index);
    void userChanged();

protected:
    void changeEvent(QEvent *e);

private:
    UserAgendasViewerPrivate *d;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // USERAGENDASVIEWER_H
