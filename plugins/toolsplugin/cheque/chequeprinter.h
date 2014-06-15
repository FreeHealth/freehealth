/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
<<<<<<< HEAD:plugins/toolsplugin/cheque/chequeprinter.h
 *   Main developers : Eric Maeker                                         *
=======
 *   Main developers : Eric Maeker
>>>>>>> 05812a06306f1d862154c30b64da662ece672ed4:plugins/toolsplugin/chequeprinter.h
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_CHEQUEPRINTER_H
#define TOOLS_CHEQUEPRINTER_H

#include <toolsplugin/tools_exporter.h>
#include <QPixmap>
#include <QDate>

/**
 * \file chequeprinter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 27 Mar 2013
 * \note requires Qt 4.8+
*/

namespace Tools {
namespace Internal {
class ChequePrintFormat;
class ChequePrinterPrivate;
} // namespace Internal

class TOOLS_EXPORT ChequePrinter
{
public:
    explicit ChequePrinter();
    ~ChequePrinter();
    bool initialize();
    
    void setDrawRects(bool drawRects);
    void setOrder(const QString &order);
    void setPlace(const QString &place);
    void setDate(const QDate &date);
    void setAmount(double amount);

    bool print(const Internal::ChequePrintFormat &format);
    QPixmap preview(const Internal::ChequePrintFormat &format);

private:
    Internal::ChequePrinterPrivate *d;
};

} // namespace Tools

#endif  // TOOLS_CHEQUEPRINTER_H

