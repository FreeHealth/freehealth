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
#ifndef USERCALENDARDELEGATESMAPPER_H
#define USERCALENDARDELEGATESMAPPER_H

#include <calendar/icalendaritemdatawidget.h>

#include <QWidget>
#include <QHash>
#include <QPointer>
#include <QModelIndex>

/**
 * \file usercalendardelegatesmapper.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 04 Aug 2011
*/

namespace UserPlugin {
class UserLineEditCompleterSearch;
}

namespace Calendar {
class CalendarPeopleModel;
}

namespace Agenda {
class UserCalendarModel;

namespace Internal {
namespace Ui {
    class UserCalendarDelegatesMapperWidget;
}

class UserCalendarDelegatesMapperWidget : public QWidget
{
    Q_OBJECT
public:
    UserCalendarDelegatesMapperWidget(QWidget *parent);
    ~UserCalendarDelegatesMapperWidget();

    void clear();
    void setUserCalendarModel(UserCalendarModel *model);

public Q_SLOTS:
    void setUserCalendarIndex(const int index);
    bool submit();

private:
    void addRow(const QString &name, const QString &uid);

private Q_SLOTS:
    void removePerson(QAction *action);
    void onPersonSelected(const QString &name, const QString &uid);
    void handlePressed(const QModelIndex &index);
    void handleClicked(const QModelIndex &index);

private:
    Internal::Ui::UserCalendarDelegatesMapperWidget *ui;
    UserPlugin::UserLineEditCompleterSearch *m_Completer;
    QHash<QString, QWidget *> m_UserWidgets;
    UserCalendarModel *m_UserCalendarModel;
    Calendar::CalendarPeopleModel *m_PeopleModel;
    int m_Row;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // USERCALENDARDELEGATESMAPPER_H

