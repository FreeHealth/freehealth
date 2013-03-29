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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Utils::PrintAxisHelper
 * Eases the process of transforming millimeters to pixels when drawing a QPainter
 * over a QPrinter.
 */

#include "printaxishelper.h"

#include <QDebug>

namespace Utils {

PrintAxisHelper::PrintAxisHelper() :
    _pixToMmCoefX(0.),
    _pixToMmCoefY(0.),
    _left(0), _right(0), _top(0), _bottom(0),
    _transXPixels(0.),
    _transYPixels(0.)
{
}

/** Computes the coefficient to transform millimeters to pixels and vice-versa */
void PrintAxisHelper::setPageSize(const QRect &pageRectPixels, const QSizeF &pageSizeInMillimeters)
{
    _pixToMmCoefX = (qreal)pageRectPixels.width() / pageSizeInMillimeters.width();
    _pixToMmCoefY = (qreal)pageRectPixels.height() / pageSizeInMillimeters.height();
    _pageRect = pageRectPixels;
}

/** Define the unprintable margins of the page */
void PrintAxisHelper::setMargins(qreal left, qreal top, qreal right, qreal bottom)
{
    _left = left;
    _right = right;
    _top = top;
    _bottom = bottom;
}

/**
 * Translates a QPoint in millimeters to QPoint in pixels
 * according to coefficient computed in setPageSize().
 * Manages translation of coordinates.
 * \sa translateMillimeters(), translatePixels()
 */
QPointF PrintAxisHelper::pointToPixels(const QPointF &pointInMilliters)
{
    return pointToPixels(pointInMilliters.x(), pointInMilliters.y());
}

/**
 * Translates a QPoint in millimeters to QPoint in pixels
 * according to coefficient computed in setPageSize()
 */
QPointF PrintAxisHelper::pointToPixels(double x_millimeter, double y_millimeter)
{
    return QPointF(x_millimeter * _pixToMmCoefX + _transXPixels, y_millimeter * _pixToMmCoefY + _transYPixels);
}

/**
 * Translate a QSize in millimeters to QSize in pixels
 * according to coefficient computed in setPageSize()
 */
QSizeF PrintAxisHelper::sizeToPixels(const QSizeF &sizeMilliters)
{
    return sizeToPixels(sizeMilliters.width(), sizeMilliters.height());
}

/**
 * Translate a QSize in millimeters to QSize in pixels
 * according to coefficient computed in setPageSize()
 */
QSizeF PrintAxisHelper::sizeToPixels(double width_millimeter, double height_millimeter)
{
    return QSize(width_millimeter * _pixToMmCoefX, height_millimeter * _pixToMmCoefY);
}

void PrintAxisHelper::translatePixels(int x, int y)
{
    _transXPixels = x;
    _transYPixels = y;
}

void PrintAxisHelper::translateMillimeters(double x, double y)
{
    _transXPixels = x * _pixToMmCoefX;
    _transYPixels = y * _pixToMmCoefY;
}

void PrintAxisHelper::printString(QPainter *painter, const PrintString &printString)
{
    painter->save();

    QRectF content(pointToPixels(printString.topMillimeters),  sizeToPixels(printString.contentSizeMillimeters));

    // If defined -> manage margins
    if (_left != 0.
            || _top != 0.
            || _right != 0.
            || _bottom != 0.) {
        if (content.left() < _left)
            content.setLeft(_left);

        if (content.right() > (_pageRect.right() - _right))
            content.setRight(_pageRect.right() - _right);

        if (content.top() < _top)
            content.setTop(_top);

        if (content.bottom() > (_pageRect.bottom() - _bottom))
            content.setBottom(_pageRect.bottom() - _bottom);
    }

//    QPen pen(QColor("red"));
//    painter->setPen(pen);

    if (printString.drawBoundingRect)
        painter->drawRect(content);

    if (printString.content.isEmpty()) {
        painter->restore();
        return;
    }

    QFont font = painter->font();
    QFontMetrics metrics(font);
    if (printString.autoFontResize) {
        // Ensure height of the chars does fit the content rect
        int heightPixels = content.size().height() * 0.9; // printString.contentSizeMillimeters.height() * _pixToMmCoefY * 0.85;
        double heightScaleFactor = ((double)metrics.height()/(double)heightPixels);

        if (!printString.splitChars) {
            // Don't split chars (check width)
            int widthPixels = content.size().width(); //printString.contentSizeMillimeters.width() * _pixToMmCoefX;
            double widthScaleFactor = ((double)metrics.width(printString.content)/(double)widthPixels);

            double scaleFactor = qMax(heightScaleFactor, widthScaleFactor);

            font.setPointSizeF(((double)font.pointSize() / scaleFactor));

            metrics = QFontMetrics(font);
            painter->setFont(font);
            painter->drawText(content, printString.alignment, printString.content);
        } else {
            // Split chars and center
            int widthPixels = content.size().width();
            double widthScaleFactorPerChar = ((double)metrics.width(printString.content)/(double)widthPixels) / (double)(printString.content.size());

            for(int i=0; i < printString.content.size(); ++i) {
                widthScaleFactorPerChar = qMax(widthScaleFactorPerChar, ((double)metrics.width(printString.content, i)/(double)widthPixels));
            }

            double scaleFactor = qMax(heightScaleFactor, widthScaleFactorPerChar);

            font.setPointSizeF(((double)font.pointSize() / scaleFactor));

            metrics = QFontMetrics(font);
            painter->setFont(font);

            double pixelPerChar = (double)widthPixels / (double)(printString.content.size());

            for(int i=0; i < printString.content.size(); ++i) {
                // if (printString.content.at(i) == "*")
                //    continue;
                QRectF charRect = QRectF(content.topLeft() + QPointF(i*pixelPerChar, 0.1), QSizeF(pixelPerChar, content.height()));
                //                if (printString.drawBoundingRect)
                //                    painter->drawRect(charRect);
                painter->drawText(charRect, Qt::AlignCenter, printString.content.at(i));
            }
        }
    } else {
        painter->drawText(content, printString.alignment, printString.content);
    }


    //    qWarning() << "fontPixelSize" << font.pixelSize();

    painter->restore();
}

} // namespace Utils
