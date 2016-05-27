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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_PRINTAXISHELPER_H
#define UTILS_PRINTAXISHELPER_H

#include <utils/global_exporter.h>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QPrinter>

/**
 * \file printaxishelper.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 21 Mar 2013
*/

namespace Utils {

struct UTILS_EXPORT PrintString {
    PrintString() :
        splitChars(false), autoFontResize(true),
        minFontPixelSize(10), drawBoundingRect(false),
        alignment(Qt::AlignVCenter)
    {}

    QString content;        /**< Content of the string */
    bool splitChars;        /**< Split chars: if defined all chars are positioned equally to each other */
    bool autoFontResize;    /**< Force resizing of the chars for them to feet inside the size of the printing rect */
    int minFontPixelSize;

    bool drawBoundingRect;

    QPointF topMillimeters;
    QSizeF contentSizeMillimeters;
    Qt::Alignment alignment;
};

class UTILS_EXPORT PrintAxisHelper
{
public:
    PrintAxisHelper();
    void setPaperSize(const QRect &pageRect, const QSizeF &pageSizeInMillimeters, const enum QPrinter::Unit);
    void setMargins(qreal left, qreal top, qreal right, qreal bottom);
    void translatePixels(int x, int y);
    void translateMillimeters(double x, double y);

    // Millimeters to pixels
    QPointF pointToPixels(const QPointF &pointInMilliters);
    QPointF pointToPixels(double x_millimeter, double y_millimeter);

    QSizeF sizeToPixels(const QSizeF &sizeMilliters);
    QSizeF sizeToPixels(double width_millimeter, double height_millimeter);

    // Print easiers
    void printString(QPainter *painter, const PrintString &printString);

private:
    QRect _pageRect;
    double _pixToMmCoefX, _pixToMmCoefY;
    qreal _left, _right, _top, _bottom;
    int _transXPixels, _transYPixels;
};

} // namespace Utils

#endif // UTILS_PRINTAXISHELPER_H
