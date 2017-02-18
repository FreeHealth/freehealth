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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef LIBS_DATAPACKS_CONSTANTS_H
#define LIBS_DATAPACKS_CONSTANTS_H

/**
 * \file constants.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 14 May 2014
*/

namespace DataPack {
namespace Constants {

const char * const PACKDESCRIPTION_FILENAME = "packdescription.xml";
const char * const PACKCREATIONQUEUE_DEFAULT_FILENAME = "packcreation.xml";

// Server internal UID
const char *const SERVER_COMMUNITY_FREE = "comm_free";
const char *const SERVER_ASSO_FREE      = "asso_free";
const char *const SERVER_COMMUNITY_NONFREE = "comm_nonfree";
const char *const SERVER_ASSO_NONFREE   = "asso_nonfree";

} // namespace Constants
} // namespace DataPack


#endif // LIBS_DATAPACKS_CONSTANTS_H
