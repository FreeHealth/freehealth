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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef LIBCALENDAR_MONTH_VIEW_H
#define LIBCALENDAR_MONTH_VIEW_H

#include <calendar/view.h>

/**
 * \file month_view.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 12 Aug 2011
*/

namespace Calendar {
namespace Internal {

class MonthHeader : public ViewWidget
{
    Q_OBJECT
public:
    MonthHeader(QWidget *parent = 0);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);
};

class MonthBody : public ViewWidget
{
    Q_OBJECT
public:
    MonthBody(QWidget *parent = 0);

    virtual int topHeaderHeight() const;
    virtual int leftHeaderWidth() const;

protected:
    virtual void paintBody(QPainter *painter, const QRect &visibleRect);
    virtual void resetItemWidgets();
    virtual void refreshItemsSizesAndPositions();
    QRect getDayRect(const QDate &day) const;

private:
    int m_weekCount;
    QPair<QDate,QDate> m_monthBoundingDays;
    //		void paintEvents(QPainter &painter, const QDate &day, const QRect &dayRect);

private Q_SLOTS:
    void firstDateChanged();
};

}  // namespace Internal
}  // namespace Calendar

#endif  // LIBCALENDAR_MONTH_VIEW_H
