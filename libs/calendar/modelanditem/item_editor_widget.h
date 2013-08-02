/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef CALENDAR_ITEMEDITORWIDGET_H
#define CALENDAR_ITEMEDITORWIDGET_H

#include <calendar/calendar_exporter.h>

#include <QWidget>
#include <QDateTime>

/**
 * \file item_editor_widget.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 05 Jul 2011
*/

namespace Calendar {
class CalendarItem;
class UserCalendar;
class AbstractCalendarModel;
class ICalendarItemDataWidget;

namespace Internal {
class ItemEditorWidgetPrivate;
}

class CALENDAR_EXPORT ItemEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemEditorWidget(QWidget *parent = 0);
    ~ItemEditorWidget();

    void clear();
    void setModel(AbstractCalendarModel *model);

    void setCalendarItem(const Calendar::CalendarItem &item);
    Calendar::CalendarItem calendarEvent() const;

    void toogleExtraInformation();
    bool isShowingExtraInformation() const;

//    void setAvailableUserCalendar(const QList<Calendar::UserCalendar *> &userCals);

    void addCalendarDataWidget(Calendar::ICalendarItemDataWidget *dataWidget);

public Q_SLOTS:
    void submit();

private Q_SLOTS:
    void on_selectIconButton_clicked();
    void on_durationCombo_currentIndexChanged(int index);
    void onDateTimeChanged(const QDateTime &);
    void changeDuration(const int comboIndex);

protected:
    void changeEvent(QEvent *e);

private:
    Internal::ItemEditorWidgetPrivate *d;
};

}  // End namespace Calendar

#endif // CALENDAR_ITEMEDITORWIDGET_H
