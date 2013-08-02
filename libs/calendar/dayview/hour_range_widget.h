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
#ifndef HOUR_RANGE_WIDGET_H
#define HOUR_RANGE_WIDGET_H

#include <calendar/modelanditem/calendar_item_widget.h>

/**
 * \file hour_range_widget.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 12 Aug 2011
*/

namespace Calendar {
class AbstractCalendarModel;
namespace Internal {

class HourRangeWidget : public CalendarItemWidget
{
    Q_OBJECT
public:
    HourRangeWidget(QWidget *parent = 0, const QString &uid = "", AbstractCalendarModel *model = 0);

    bool inMotion() const { return m_inMotion; }
    void setInMotion(bool value);

private:
    bool m_inMotion;
    QWidget *m_aboveWidget;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    //		virtual void mouseReleaseEvent(QMouseEvent *event);
};

}  // namespace Internal
}  // namespace Calendar

#endif  // HOUR_RANGE_WIDGET_H
