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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
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
    _pixToMmCoefY(0.)
{
}

/** Computes the coefficient to transform millimeters to pixels and vice-versa */
void PrintAxisHelper::setPageSize(const QRect &pageRect, const QSizeF &pageSizeInMillimeters)
{
    _pixToMmCoefX = pageRect.width() / pageSizeInMillimeters.width();
    _pixToMmCoefY = pageRect.height() / pageSizeInMillimeters.height();
}

QPoint PrintAxisHelper::pointToPixels(const QPoint &pointInMilliters)
{
    return pointToPixels(pointInMilliters.x(), pointInMilliters.y());
}

/**
 * Translates a QPoint in millimeters to QPoint in pixels
 * according to coefficient computed in setPageSize()
 */
QPoint PrintAxisHelper::pointToPixels(double x_millimeter, double y_millimeter)
{
    return QPoint(x_millimeter * _pixToMmCoefX, y_millimeter * _pixToMmCoefY);
}

QSize PrintAxisHelper::sizeToPixels(const QSize &sizeMilliters)
{
    return sizeToPixels(sizeMilliters.width(), sizeMilliters.height());
}

/**
 * Translate a QSize in millimeters to QSize in pixels
 * according to coefficient computed in setPageSize()
 */
QSize PrintAxisHelper::sizeToPixels(double width_millimeter, double height_millimeter)
{
    return QSize(width_millimeter * _pixToMmCoefX, height_millimeter * _pixToMmCoefY);
}

void PrintAxisHelper::printString(QPainter *painter, const PrintString &printString)
{
    painter->save();

    QRect content(pointToPixels(printString.topMillimeters),  sizeToPixels(printString.contentSizeMillimeters));

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
        int heightPixels = printString.contentSizeMillimeters.height() * _pixToMmCoefY * 0.85;
        double heightScaleFactor = ((double)metrics.height()/(double)heightPixels);
//        qWarning() << "font size: fontsize" << font.pointSize()
//                   << "heighPixel" << heightPixels << "heightScaleFactor" << heightScaleFactor;

        if (!printString.splitChars) {
            // Don't split chars (check width)
            int widthPixels = printString.contentSizeMillimeters.width() * _pixToMmCoefX;
            double widthScaleFactor = ((double)metrics.width(printString.content)/(double)widthPixels);

//            qWarning() << "font size: fontsize" << font.pointSize()
//                       << "widthPixels" << widthPixels << "widthScaleFactor" << widthScaleFactor;

            double scaleFactor = qMax(heightScaleFactor, widthScaleFactor);
//            qWarning() << "scaleFactor" << scaleFactor;

            font.setPointSizeF(((double)font.pointSize() / scaleFactor));

            metrics = QFontMetrics(font);
//            qWarning() << "fontMetrics.width" << metrics.width(printString.content)
//                       << "fontMetrics.height" << metrics.height()
//                       << "heightpixel" << heightPixels << "widthPixels" << widthPixels
//                       << "fontPointSize" << font.pointSizeF()
//                       << "\n\n";

            painter->setFont(font);
            painter->drawText(content, Qt::AlignVCenter, printString.content);
        } else {
//            // Split chars and center
            int widthPixels = printString.contentSizeMillimeters.width() * _pixToMmCoefX;
            double widthScaleFactorPerChar = ((double)metrics.width(printString.content)/(double)widthPixels) / (double)(printString.content.size());

            // qWarning() << "width" << widthPixels << "widthScaleFactorPerChar" << widthScaleFactorPerChar << "content.size()" << printString.content.size();

            for(int i=0; i < printString.content.size(); ++i) {
                widthScaleFactorPerChar = qMax(widthScaleFactorPerChar, ((double)metrics.width(printString.content, i)/(double)widthPixels));
            }

//            qWarning() << "font size: fontsize" << font.pointSize()
//                       << "widthPixels" << widthPixels << "widthScaleFactorPerChar" << widthScaleFactorPerChar;

            double scaleFactor = qMax(heightScaleFactor, widthScaleFactorPerChar);
//            qWarning() << "scaleFactor" << scaleFactor;

            font.setPointSizeF(((double)font.pointSize() / scaleFactor));

            metrics = QFontMetrics(font);
//            qWarning() << "fontMetrics.width" << metrics.width(printString.content)
//                       << "fontMetrics.height" << metrics.height()
//                       << "heightpixel" << heightPixels << "widthPixels" << widthPixels
//                       << "fontPointSize" << font.pointSizeF();

            painter->setFont(font);

            double pixelPerChar = (double)widthPixels / (double)(printString.content.size());
//            qWarning() << "pixelPerChar" << pixelPerChar << (pixelPerChar * _pixToMmCoefX) << "\n\n";

            for(int i=0; i < printString.content.size(); ++i) {
                // if (printString.content.at(i) == "*")
                //    continue;
                QRectF charRect = QRectF(content.topLeft() + QPointF(i*pixelPerChar, 0.1), QSizeF(pixelPerChar, content.height()));
//                if (printString.drawBoundingRect)
//                    painter->drawRect(charRect);
                painter->drawText(charRect, Qt::AlignCenter, printString.content.at(i));
            }
        }

    }


//    qWarning() << "fontPixelSize" << font.pixelSize();

    painter->restore();
}

} // namespace Utils
