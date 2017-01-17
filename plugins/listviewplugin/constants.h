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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef VIEWS_PLUGIN_CONSTANTS_H
#define VIEWS_PLUGIN_CONSTANTS_H

#include <QtGlobal>
#include <QString>

namespace Views {
namespace Constants {

    // Translate context
    const char * const  VIEWS_CONSTANTS_TR_CONTEXT = "Views";

    const char* const C_BASIC            = "context.ListView.basic";
    const char* const C_BASIC_ADDREMOVE  = "context.ListView.AddRemove";
    const char* const C_BASIC_MOVE       = "context.ListView.Move";
    const char * const HIDDEN_ID         = "@#HiDdEnId#@";


    /** \brief Enumerates the available default actions that can be added. Actions are pop-upped. */
    enum AvailableAction {
        AddRemove      = 0x01,
        MoveUpDown     = 0x02,
        Edit           = 0x04,
        DefaultActions = AddRemove,
        AllActions     = AddRemove | MoveUpDown
    };
    Q_DECLARE_FLAGS(AvailableActions, AvailableAction)

}  // End namespace Constants
}  // End namespace Views

Q_DECLARE_OPERATORS_FOR_FLAGS(Views::Constants::AvailableActions)

#endif // VIEWS_PLUGIN_CONSTANTS_H
