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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Calendar::Internal::HourMark
 * Draw a simple mark with hour and day of week.
 */

#include "hourmark.h"

#include <QPixmap>
#include <QPainter>

using namespace Calendar;
using namespace Internal;

HourMark::HourMark(QWidget *parent) :
    QWidget(parent)
{
}

void HourMark::setDayOfWeek(int dayOfWeek)
{
    _day = dayOfWeek;
}

void HourMark::setTime(const QTime &time)
{
    _time=time;
}

void HourMark::paintEvent(QPaintEvent *)
{
    // on the top draw a line
    QPixmap linepixmap(size().width(), 22);
//    linepixmap.fill(Qt::white);
    linepixmap.fill(Qt::transparent);

    QPainter linePainter(&linepixmap);
    linePainter.setRenderHint(QPainter::Antialiasing);
    QPen linePen;
    linePen.setColor(Qt::lightGray);
    linePainter.setPen(linePen);
    linePainter.drawLine(0, 1, width(), 1);
    linePen.setColor(Qt::black);
    linePainter.setPen(linePen);
    linePainter.drawLine(0, 0, width(), 0);

    // text
    QRect r = rect();
    r.setHeight(15);
    r.adjust(2, 0, 0, 0);
    QPen fontPen;
    QFont font = linePainter.font();
    font.setPixelSize(10);
//    font.setBold(true);
    linePainter.setFont(font);
    fontPen.setColor(Qt::lightGray);
    linePainter.setPen(fontPen);
    r.adjust(2, 0, 0, 0);
    linePainter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, _time.toString("hh:mm"));
    fontPen.setColor(Qt::black);
    linePainter.setPen(fontPen);
    r.adjust(1, 0, 0, 0);
    linePainter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, _time.toString("hh:mm"));

    // main
    QPainter painter(this);
    painter.drawPixmap(QPoint(0, 0), linepixmap, QRect(0, 0, width(), 22));

}
