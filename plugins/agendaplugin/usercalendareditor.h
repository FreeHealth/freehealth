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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_USERCALENDAREDITOR_H
#define AGENDA_USERCALENDAREDITOR_H

#include <agendaplugin/agenda_exporter.h>

#include <QWidget>
#include <QDataWidgetMapper>

/**
 * \file usercalendareditor.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 18 Nov 2011
*/

namespace Calendar {
class AbstractCalendarModel;
}

namespace Agenda {
class UserCalendar;
class UserCalendarModel;
class DayAvailabilityModel;

namespace Ui {
class UserCalendarEditorWidget;
}

class AGENDA_EXPORT UserCalendarEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserCalendarEditorWidget(QWidget *parent = 0);
    ~UserCalendarEditorWidget();

    void clear();

    void setUserCalendarModel(UserCalendarModel *model);

public Q_SLOTS:
    void setCurrentIndex(const QModelIndex &index);
    void addAvailability();
    void removeAvailabilities();
    void modifyAvailability(const QModelIndex &index);
    void clearAvailabilities();
    void submit();
    void revert();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::UserCalendarEditorWidget *ui;
    UserCalendarModel *m_UserCalendarModel;
    DayAvailabilityModel *m_AvailabilityModel;
    QDataWidgetMapper *m_Mapper;
};

}  // End namespace Agenda


#endif // AGENDA_USERCALENDAREDITOR_H
