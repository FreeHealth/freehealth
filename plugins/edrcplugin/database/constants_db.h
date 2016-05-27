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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRCPLUGIN_DATABASE_CONSTANTS_H
#define EDRCPLUGIN_DATABASE_CONSTANTS_H

#include <QString>
#include <QDateTime>

/**
 * \file constants_db.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 25 Jun 2013
*/

namespace eDRC {
namespace Constants {

const char *const DB_NAME = "edrc_ro";
const char *const DB_FILENAME = "edrc.db";
const char *const DB_VERSION = "2013.06-eDRC:2005";

    enum SfmgBaseTables {
        Table_REF_RC = 0,           // Résultats de consultation (RC)
        Table_Ref_RCItem,           // Éléments de vocabulaire au sein des critères du RC == RCITEM
        Table_Ref_RCE,              // Risques Critiques Évitables
        Table_Ref_ClassRC,          // Classe de regroupement des RC
        Table_Ref_CIM10,            //
        Table_RC_Link_RC_CIM10,
        Table_RC_Link_RC_Class,
        Table_RC_Link_RC_Criteres,
        Table_RC_Link_RC_RCE,
        Table_RC_Link_VA,
        Table_Version,
        MaxTables
    };

    enum FieldsTable_REF_RC {
        REF_RC_SEQ = 0,
        LIB_RC_FR,          // Libellé français
        VALIDITE,           // Validité de l'item
        PA, PB, PC, PD, PZ, // Positions diag  pour les RC
        P1, P2, P3, P4, P5, // Niveaux de risque pour les DPIO
        REG_RC_LIB,         // Libellé du RC de regroupement (quand item passe de valide à invalide)
        REG_RC_ID,          // Id du RC de regroupement
        SUB_RC_ID,          // RC de substitution (quand item passe de valide à invalide)
        VALID_DEBUT,        // Date d'arrivée dans le dictionnaire
        VALID_FIN,          // Date de fin d'utilisation dans le dictionnaire
        ARGUMENTAIRE,       // Explications / argumentaire
        NATURE              // Nature du RC :
                            //  1 = RC (accepte unique P[A-Z]
                            //  2 = DPIO v1 (éléments de dépistage et prévention individuels organisés) (accepte uniquement P[1-5]
    };

    enum FieldsTable_Ref_RCItem {
        REF_RCITEM_SEQ = 0,
        LIB_RCITEM_FR
    };

    enum FieldsTable_Ref_RCE {
        REF_RCE_SEQ = 0,
        RCE_LIB
    };

    enum FieldsTable_Ref_ClassRC {
        REF_CLASSRC_SEQ,
        REF_CLASSRC_LIB
    };

    enum FieldsTable_Ref_CIM10 {
        REF_CIM10_SEQ = 0,
        COD_CIM10,
        CIM10_LIBELLE
    };

    enum FieldsTable_RC_Link_RC_Criteres { // Permet de lier RC <-> RCITEM
        RC_LCRITERES_REF_LRCCRITERES_SEQ = 0,
        RC_LCRITERES_REF_RC_ID,                  // Clé externe sur table RC
        RC_LCRITERES_AFFICH_ORDRE,
        RC_LCRITERES_REF_RCITEM_ID,              // Clé externe sur table RCITEM
        RC_LCRITERES_REF_RETRAIT_ID,             // Clé externe sur table REF_RETRAIT
        RC_LCRITERES_REF_PONDER_ID,              // Clé externe sur table REF_PONDER
        RC_LCRITERES_LIB_CRITERES_FR             // Libellé
    };

    enum FieldsTable_RC_Link_RC_RCE {   // Permet de lier RC <-> RCE
        RC_LRCE_REF_RC_ID = 0,
        RC_LRCE_REF_RCE_ID
    };

    enum FieldsTable_RC_Link_RC_CIM10 { // Permet de lier RC <-> CIM10
        RC_LCIM_REF_CIM10_ID = 0,
        RC_LCIM_REF_RC_ID,
        RC_LCIM_CONTEXTE
    };

    enum FieldsTable_RC_Link_RC_Class { // Permet de lier RC <-> Classes de RC
        RC_LCLASS_REF_RC_ID = 0,
        RC_LCLASS_REF_CLASSRC_ID
    };

    enum FieldsTable_RC_Link_VA {       // Permet de lier RC <-> RC
        VOIR_AUSSI_REF_RC_ID = 0,   // Clé externe du RC_SEQ
        VOIR_AUSSI_ID               // Clé externe du RC_SEQ
    };

    enum FieldsTable_Version {
        VERSION_TEXT = 0,
        VERSION_DATE
    };

} // namespace Constants
} // namespace eDRC

#endif  // EDRCPLUGIN_DATABASE_CONSTANTS_H
