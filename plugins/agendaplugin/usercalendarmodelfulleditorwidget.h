/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_UserCalendarModelFullEditorWidget_H
#define AGENDA_UserCalendarModelFullEditorWidget_H

#include <QWidget>
#include <QModelIndex>

/**
 * \file usercalendarmodelfulleditorwidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.2
 * \date 22 May 2012
*/

namespace Agenda {
class UserCalendarModel;

namespace Ui {
    class UserCalendarModelFullEditorWidget;
}

/** \todo code here : put this in Internal */
class UserCalendarModelFullEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserCalendarModelFullEditorWidget(QWidget *parent = 0);
    ~UserCalendarModelFullEditorWidget();

    void setUserCalendarModel(UserCalendarModel *model);

public Q_SLOTS:
    void clear();
    bool submit();

private Q_SLOTS:
    void setCurrentIndex(const QModelIndex &index);

private:
    void changeEvent(QEvent *e);

private:
    Ui::UserCalendarModelFullEditorWidget *ui;
    UserCalendarModel *m_UserCalendarModel;
    QAction *aCreateCalendar;
};

}  // End namespace Agenda

#endif // AGENDA_UserCalendarModelFullEditorWidget_H
