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
#ifndef DAY_WIDGET_H
#define DAY_WIDGET_H

#include <calendar/modelanditem/calendar_item_widget.h>

/**
 * \file day_widget.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 12 Aug 2011
*/

namespace Calendar {
class AbstractCalendarModel;
namespace Internal {

/**
 * This class represents a day event without hour range specification
*/
class DayWidget : public CalendarItemWidget
{
    Q_OBJECT
public:
    /** if uid is empty, this calendar item is considered as temporary and will be drawn with transparence
         */
    DayWidget(QWidget *parent = 0, const QString &uid = "", AbstractCalendarModel *model = 0);

    bool inMotion() const { return m_inMotion; }
    void setInMotion(bool value);

    QSize sizeHint() const;
    static QSize staticSizeHint();

private:
    bool m_inMotion;
    QWidget *m_aboveWidget;
    QFont m_titleFont; // TODO: choose a better font than the default one

    static QFont getTitleFont();

protected:
    virtual void paintEvent(QPaintEvent *event);
};

}  // namespace Internal
}  // namespace Calendar


#endif
