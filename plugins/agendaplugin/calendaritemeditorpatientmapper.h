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
#ifndef AGENDA_CALENDARITEMEDITORPATIENTMAPPER_H
#define AGENDA_CALENDARITEMEDITORPATIENTMAPPER_H

#include <calendar/icalendaritemdatawidget.h>
#include <calendar/modelanditem/calendar_people.h>

#include <QWidget>
#include <QHash>
#include <QPointer>

/**
 * \file calendaritemeditorpatientmapper.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 09 Jul 2011
*/

namespace Calendar {
class AbstractCalendarModel;
}

namespace Patients {
class PatientBaseCompleter;
}

namespace Agenda {
namespace Internal {
namespace Ui {
    class CalendarItemEditorPatientMapper;
}

class CalendarItemEditorPatientMapperWidget : public QWidget
{
    Q_OBJECT
public:
    CalendarItemEditorPatientMapperWidget(QWidget *parent);
    ~CalendarItemEditorPatientMapperWidget();

    void clear();
    void setCalendarItemModel(Calendar::AbstractCalendarModel *model);
    void setCalendarItem(const Calendar::CalendarItem &item);

public Q_SLOTS:
    void addCurrentPatient();
//    QList<Calendar::People> selected() const {return m_Selected;}
    bool submitToItem(const Calendar::CalendarItem &item);

private:
    void addPatientRow(const QString &name, const QString &uid);

private Q_SLOTS:
    void removePatient(QAction *action);
    void onPatientSelected(const QString &name, const QString &uid);
    void onPatientCreated(const QString &uid);
    void onCurrentPatientChanged();
    void handlePressed(const QModelIndex &index);
    void handleClicked(const QModelIndex &index);

protected:
    void changeEvent(QEvent *e);

private:
    Internal::Ui::CalendarItemEditorPatientMapper *ui;
//    QList<Calendar::People> m_Selected;
    Patients::PatientBaseCompleter *m_Completer;
    Calendar::CalendarPeopleModel *m_PeopleModel;
    Calendar::AbstractCalendarModel *m_ItemModel;
    bool m_StoredSettingsValue;
    QAction *aUseCurrentPatient;
};

class CalendarItemEditorPatientMapper : public Calendar::ICalendarItemDataWidget
{
    Q_OBJECT
public:
    explicit CalendarItemEditorPatientMapper(QObject *parent = 0);
    ~CalendarItemEditorPatientMapper();

    int insertionPlace() const;
    QWidget *createWidget(QWidget *parent = 0);

    bool setCalendarItemModel(Calendar::AbstractCalendarModel *model);
    bool setCalendarItem(const Calendar::CalendarItem &item);

    bool clear();
    bool submitChangesToCalendarItem(const Calendar::CalendarItem &item);

private:
    QPointer<Internal::CalendarItemEditorPatientMapperWidget> m_Widget;
    Calendar::AbstractCalendarModel *m_ItemModel;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // AGENDA_CALENDARITEMEDITORPATIENTMAPPER_H
