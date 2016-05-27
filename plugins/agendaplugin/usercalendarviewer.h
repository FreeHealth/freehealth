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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_USERCALENDARVIEWER_H
#define AGENDA_USERCALENDARVIEWER_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QModelIndex;
class QDate;
QT_END_NAMESPACE

/**
 * \file usercalendarviewer.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 24 Sept 2011
*/

namespace Agenda {
class UserCalendar;

namespace Internal {
class UserCalendarViewerPrivate;
class AgendaMode;

class UserCalendarViewer : public QWidget
{
    Q_OBJECT
    friend class Agenda::Internal::AgendaMode;

public:
    enum AgendaOwner {
        OwnAgendas = 0,
        DelegatedAgendas
    };

    explicit UserCalendarViewer(QWidget *parent = 0);
    ~UserCalendarViewer();

protected:
    void recalculateComboAgendaIndex();

private Q_SLOTS:
    void newEvent();
    void newEventAtAvailabity(const QModelIndex &index);
    void refreshAvailabilities();
    void resetDefaultDuration();
    void quickDateSelection(QAction *a);
    void onStartDateChanged(const QDate &start);
    void recalculateAvailabilitiesWithDurationIndex(const int index);
    void on_availableAgendasCombo_activated(const int index);
    void userChanged();
    void updateCalendarData(const QModelIndex &top, const QModelIndex &bottom);
    void onSwitchToPatientClicked();
    void onEditAppointmentClicked();
    void onPrintAppointmentClicked();
    void onDeleteAppointmentClicked();

protected:
    void clear();
    bool event(QEvent *e);

private:
    UserCalendarViewerPrivate *d;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // AGENDA_USERCALENDARVIEWER_H
