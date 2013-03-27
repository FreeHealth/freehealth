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
#ifndef MONTH_DAY_WIDGET_H
#define MONTH_DAY_WIDGET_H

#include <QWidget>
#include <QDate>
#include <QMap>

#include <calendar/modelanditem/abstract_calendar_model.h>
#include <calendar/modelanditem/calendar_item.h>

namespace Calendar {
class MonthDayWidget : public QWidget {
    Q_OBJECT
public:
    MonthDayWidget(AbstractCalendarModel *model, const QDate &day, QWidget *parent = 0);

    const QDate &day() const { return m_day; }

protected:
    virtual void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    AbstractCalendarModel *m_model;
    QDate m_day;
    QList<CalendarItem> m_items;
    QMap<QWidget *, QString> m_uidByWidget;

    CalendarItem *getItemByUid(const QString &uid);

private Q_SLOTS:
    void refreshItems();
};
}

#endif
