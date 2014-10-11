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
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef GUARD_DATABASE_CONSTANTS_H
#define GUARD_DATABASE_CONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 * \file constants_db.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 10 Oct 2014
*/

namespace Guard {
namespace Constants {

    const char * const DB_GUARD           = "guard";
    const char * const DB_DEFAULT_USER    = "GuardDefaultUser";
    const char * const DB_ACTUALVERSION   = "0.0.1";

    //DatapackModel
//    const char * const DATAPACK_GUARD = "guard";
//    const char * const DATAPACK_DB = "guard_datapack";
//    const char * const DATAPACK_GUARD_FILENAME = "guard_datapack.db";

    // Database abstraction
/**
    Lignes de gardes    = GUARD_LINES
    un tour de garde = horaires par jour modulable selon la journée (ex: samedi = 2 tours, semaine = 1 tour)
    x tours de garde
    x médecins par tour de garde
    chaque tour est pondéré par une priorité (remplir d'abord les tours prioritaires)


    Médecins:   = GUARDIANS + GUARDIANS_PREFS
    - dispos et souhaits de dates
    - regroupement des dates
    - indispos
      - ponctuelles
      - régulières (formations, annivs...)
    - par tour de garde uniquement
    - participent à des tours de gardes définis ou tous les tours de garde
    - peuvent être prioritaires sur certains tours (priorité positive ou négative - eg participe prio aux samedis et peu au dimanches)
    - peuvent définir pour chaque tours (et pour l'ensemble des tours) un maximum acceptable de présence
    - accepte d'éffectuer plusieurs tours successifs (si un tour = WE, accepte x we d'affilé ou pas)

    Tableaux de gardes: = GUARDS_PLANS
    - tableaux préparatoires
    - tableaux définitifs (non modifiables)

*/
    enum Tables {
        Table_Guardian = 0,
        Table_GuardLine,
        Table_GuardianPref,
        Table_GuardTable,
        Table_VERSION
    };

    enum TableGuardian_Fields {
        GUARDIAN_ID = 0,
        GUARDIAN_UID,
        GUARDIAN_FULLNAME,
        GUARDIAN_EMAIL,
        GUARDIAN_ACCEPT_MAILS,
        GUARDIAN_MOBILEPHONE,
        GUARDIAN_ACCEPT_SMS,
        GUARDIAN_MaxParam
    };

    enum Table_GuardianPref_Fields {
        GUARDIAN_PREFS_ID = 0,
        GUARDIAN_PREFS_GUARDIAN_UID,
        GUARDIAN_PREFS_GUARDLINE_UID,
        GUARDIAN_PREFS_MaxParam
    };

    enum TableGuardLine_Fields {
        GUARDLINE_ID = 0,
        GUARDLINE_UID,
        GUARDLINE_LABEL,
        GUARDLINE_MaxParam
    };

    enum TableGuardTable_Fields {
        GUARDTABLE_ID = 0,
        GUARDTABLE_UID,
        GUARDTABLE_ISVALID,
        GUARDTABLE_STATE,           // Preview (rw), updated preview (rw), definitive (ro)
        GUARDTABLE_MaxParam
    };

    enum Table_Version_Fields {
        VERSION_TEXT = 0
    };

}  // End namespace Constants
}  // End namespace Guard

#endif // GUARD_DATABASE_CONSTANTS_H
