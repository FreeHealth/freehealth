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
#ifndef LIBCALENDAR_VIEW_H
#define LIBCALENDAR_VIEW_H

#include <QWidget>
#include <QDate>
#include <QPointer>
#include <QMenu>

#include <calendar/modelanditem/calendar_item_widget.h>
#include <calendar/modelanditem/abstract_calendar_model.h>

class QPainter;
class QRect;
class QScrollArea;

/**
 * \file view.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 12 Aug 2011
*/

namespace Calendar {
namespace Internal {

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    ViewWidget(QWidget *parent = 0);

    const QDate &firstDate() const { return m_firstDate; }
    void setFirstDate(const QDate &firstDate);
    virtual void refreshCurrentDateTimeStuff();
    AbstractCalendarModel *model() const { return m_model; }
    void setModel(AbstractCalendarModel *model);
    void setMasterScrollArea(QScrollArea *scrollArea);
    void setContextMenuForItems(QMenu *menu) {m_ItemContextMenu = menu;}

    // TODO: this was protected
    QScrollArea *masterScrollArea;

Q_SIGNALS:
    void firstDateChanged();

protected:
    QPixmap generatePixmap();
    void forceUpdate() { m_refreshGrid = true; update(); }

    virtual void paintBody(QPainter *, const QRect &) {}
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void refreshItemSizeAndPosition(CalendarItemWidget *) {}
    virtual void refreshItemsSizesAndPositions();
    virtual void resetItemWidgets() {}
    bool eventFilter(QObject *obj, QEvent *event);

    QMenu *itemContextMenu() const {return m_ItemContextMenu;}

    CalendarItemWidget *getWidgetByUid(const QString &uid) const;
    QList<CalendarItemWidget*> getWidgetsByDate(const QDate &dayDate) const;
    void deleteAllWidgets();

protected Q_SLOTS:
    virtual void itemInserted(const Calendar::CalendarItem &item) { Q_UNUSED(item); }
    virtual void itemModified(const Calendar::CalendarItem &oldItem, const Calendar::CalendarItem &newItem) { Q_UNUSED(oldItem); Q_UNUSED(newItem); }
    virtual void itemRemoved(const Calendar::CalendarItem &removedItem) { Q_UNUSED(removedItem); }
    virtual void reset();

private:
    QDate m_firstDate;
    bool m_refreshGrid;
    AbstractCalendarModel *m_model;
    QPointer<QMenu> m_ItemContextMenu;
};

}  // namespace Internal
}  // namespace Calendar

#endif  // LIBCALENDAR_VIEW_H
