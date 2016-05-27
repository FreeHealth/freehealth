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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef LIBCALENDAR_DAYRANGEBODY_H
#define LIBCALENDAR_DAYRANGEBODY_H

#include <calendar/view.h>

/**
 * \file dayrangebody.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.8.0
 * \date 04 Oct 2012
*/

namespace Calendar {
namespace Internal {
class DayRangeBodyPrivate;

class DayRangeBody : public ViewWidget
{
    Q_OBJECT
    friend class Calendar::Internal::DayRangeBodyPrivate;

public:
    DayRangeBody(QWidget *parent = 0, int rangeWidth = 7);

    int rangeWidth() const;
    void setRangeWidth(int width);

    int granularity() const;
    void setGranularity(int value);

    int itemDefaultDuration() const;
    void setItemDefaultDuration(int value);

    int dayScaleHourDivider() const;
    void setDayScaleHourDivider(int value);

    int hourHeight() const;
    void setHourHeight(int value);

    virtual QSize sizeHint() const;
    CalendarItem getContextualCalendarItem() const;

protected:
    virtual void paintBody(QPainter *painter, const QRect &visibleRect);
    virtual void refreshItemsSizesAndPositions();

    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void resetItemWidgets();

protected Q_SLOTS:
    void itemInserted(const Calendar::CalendarItem &item);
    void itemModified(const Calendar::CalendarItem &oldItem, const Calendar::CalendarItem &newItem);
    void itemRemoved(const Calendar::CalendarItem &removedItem);

private Q_SLOTS:
    void modifyPressItem();
    void removePressItem();

private:
    Internal::DayRangeBodyPrivate *d_body;

protected:
    static int m_leftScaleWidth;
    static int m_minimumItemHeight;
};

}  // namespace Internal
}  // namespace Calendar

#endif  // LIBCALENDAR_DAYRANGEBODY_H
