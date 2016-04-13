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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_FSPPRINTER_H
#define TOOLS_INTERNAL_FSPPRINTER_H

#include <QImage>

/**
 * \file fspprinter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 23 Mar 2013
*/

namespace Tools {
namespace Internal {
class Fsp;
class FspPrinterPrivate;

class FspPrinter
{
public:
    enum Cerfa {
        S12541_01 = 0,
        S12541_02,
        S12541_02_2
    };

    enum PrintDirection {
        TopToBottom = 0,
        BottomToTop
    };

    explicit FspPrinter();
    ~FspPrinter();

    void setDrawRects(bool drawRects);
    bool print(const Fsp &fsp, Cerfa cerfa = S12541_01, bool printCerfaAsBackground = false);
    QPixmap preview(const Fsp &fsp, Cerfa cerfa);

private:
    FspPrinterPrivate *d;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_FSPPRINTER_H

