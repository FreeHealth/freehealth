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
#ifndef CALENDAR_INTERNAL_HOURMARK_H
#define CALENDAR_INTERNAL_HOURMARK_H

#include <QWidget>
#include <QTime>

/**
 * \file hourmark.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 26 July 2012
*/

namespace Calendar {
namespace Internal {

class HourMark : public QWidget
{
    Q_OBJECT
public:
    explicit HourMark(QWidget *parent = 0);

    void setDayOfWeek(int dayOfWeek);
    void setTime(const QTime &time);

protected:
    void paintEvent(QPaintEvent *);

private:
    int _day;
    QTime _time;
};

} // namespace Internal
} // namespace Calendar

#endif // CALENDAR_INTERNAL_HOURMARK_H
