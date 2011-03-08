/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FORMMANAGER_DB_CONSTANTS_H
#define FORMMANAGER_DB_CONSTANTS_H

namespace Form {
namespace Constants {

    const char * const  DB_NAME            = "episodes";
    const char * const  DB_FILENAME        = "episodes.db";
    const char * const  DB_ACTUALVERSION   = "0.1";

    // Tables
    enum Tables {
      Table_EPISODES = 0,
      Table_EPISODE_CONTENT,
      Table_VERSION
    };

    // Fields
    enum EpisodesFields {
        EPISODES_ID = 0,
        EPISODES_PATIENT_UID,
        EPISODES_LK_TOPRACT_LKID,
        EPISODES_FORM_PAGE_UID,
        EPISODES_LABEL,
        EPISODES_DATE,
        EPISODES_DATEOFCREATION,
        EPISODES_DATEOFMODIFICATION,
        EPISODES_DATEOFVALIDATION,
        EPISODES_VALIDATED
    };
    enum EpisodesContentFields {
        EPISODE_CONTENT_ID = 0,
        EPISODE_CONTENT_EPISODE_ID,
        EPISODE_CONTENT_XML
    };
    enum VersionFields {
        VERSION_TEXT = 0
    };

}  // End namespace Form::Constants
}  // End namespace Form

#endif // End FORMMANAGER_DB_CONSTANTS_H
