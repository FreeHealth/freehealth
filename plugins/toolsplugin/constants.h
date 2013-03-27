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
 *   Main Developpers:                                                     *
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_GLOBAL_CONSTANTS_H
#define TOOLS_GLOBAL_CONSTANTS_H

/**
 * \file constants.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 23 Mar 2013
*/

namespace Tools {
namespace Constants {

const char * const S_HORIZ_CORRECTION_MM = "Tools/Fsp/PrinterCorrection/horiz_mm";
const char * const S_VERTIC_CORRECTION_MM = "Tools/Fsp/PrinterCorrection/vertic_mm";
const char * const S_PRINT_DIRECTION = "Tools/Fsp/PrinterCorrection/direction";

// Settings Values
const char * const S_TOPTOBOTTOM = "topToBottom";
const char * const S_BOTTOMTOTOP = "bottomToTop";

}
}

#endif // TOOLS_GLOBAL_CONSTANTS_H
