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
#ifndef LIBCALENDAR_DAYRANGEHEADER_H
#define LIBCALENDAR_DAYRANGEHEADER_H

#include <calendar/view.h>

/**
 * \file dayrangeheader.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.8.0
 * \date 26 July 2012
*/

namespace Calendar {
namespace Internal {
class DayRangeHeaderPrivate;

class HourRangeWidget;
class DayWidget;

class DayRangeHeader : public ViewWidget
{
    Q_OBJECT
public:
    DayRangeHeader(QWidget *parent = 0, int rangeWidth = 7);

    QSize sizeHint() const;

    int rangeWidth() const;
    void setRangeWidth(int width);

protected:
    void paintEvent(QPaintEvent *event);
    void resetItemWidgets();
    void refreshItemsSizesAndPositions();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private Q_SLOTS:
    void modifyPressItem();
    void removePressItem();

private:
    Internal::DayRangeHeaderPrivate *d_header;
};

}  // namespace Internal
}  // namespace Calendar

#endif  // LIBCALENDAR_DAYRANGEHEADER_H
