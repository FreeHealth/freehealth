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

/**
 * Translates a QPoint in millimeters to QPoint in pixels
 * according to coefficient computed in setPageSize()
 */
QPoint PrintAxisHelper::pointToPixels(double x_millimeter, double y_millimeter)
{
    return QPoint(x_millimeter * _pixToMmCoefX, y_millimeter * _pixToMmCoefY);
}

/**
 * Translate a QSize in millimeters to QSize in pixels
 * according to coefficient computed in setPageSize()
 */
QSize PrintAxisHelper::sizeToPixels(double width_millimeter, double height_millimeter)
{
    return QSize(width_millimeter * _pixToMmCoefX, height_millimeter * _pixToMmCoefY);
}

} // namespace Utils
