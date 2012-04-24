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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_ITOKEN_H
#define PADTOOLS_ITOKEN_H

#include <QString>
#include <QVariant>

/**
 * \file itoken.h
 * \author Eric Maekerw
 * \version 0.8.0
 * \date 25 Apr 2012
*/


namespace PadTools {

class IToken
{
public:
    IToken() {}
    virtual ~IToken() {}

    virtual QString name() const = 0;
    virtual QString tooltip() const = 0;
    virtual QString helpText() const = 0;
    virtual QVariant testValue() const = 0;
};

} // namespace PadTools

#endif // PADTOOLS_ITOKEN_H
