/***************************************************************************
 *  FreeHealth EHR is a set of free and open source medical applications.  *
 *  © 2016 Jerome Pinguet <jerome@jerome.cc>                               *
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
 *  Authors:                                                               *
 *  Jerome Pinguet <jerome@jerome.cc>                                      *
 *  NAME <MAIL@ADDRESS.COM>                                                *
 ***************************************************************************/
#ifndef TEXTEDITORPLUGIN_CONSTANTS_H
#define TEXTEDITORPLUGIN_CONSTANTS_H

#include <qglobal.h>

namespace Editor {
namespace Constants {

// Maximum size in bytes for file drag and drop
const qint64 MAX_FILE_SIZE = Q_INT64_C(10000);

} // namespace Constants
} // namespace Editor

#endif // TEXTEDITORPLUGIN_CONSTANTS_H
