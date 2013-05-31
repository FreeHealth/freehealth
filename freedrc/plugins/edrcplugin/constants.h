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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_CONSTANTS_H
#define EDRC_CONSTANTS_H

#include <QString>
#include <QDateTime>

/**
 * \file constants.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 28 May 2013
*/

namespace eDRC {
namespace Constants {

const char *const DB_NAME = "edrc_ro";
const char *const DB_FILENAME = "edrc.db";
const char *const DB_VERSION = "0.1";

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


//{
//public:

//  static QString m_PathAppli;               /*!< Chemin d'exécution de l'application */
//  static QString m_PathIni;                 /*!< Chemin vers le fichier INI du plugin */
//  static QString m_PathDrTux;               /*!< Chemin vers DrTux */
//  static QString m_PathDrTuxIni;            /*!< Chemin vers le fichier de paramètres de DrTux */
//  static QString m_PathExchg;               /*!< Chemin vers le fichier d'échange avec DrTux */
//  static QString m_MaskFile;                /*!< Chemin vers le masque HTML a utiliser pour mettre en forme
//                                             *   le texte des RC. */

//  static QString m_MedinTux_User;           /*!< Nom de l'utilisateur de MedinTux */
//  static QString m_MedinTux_GUIDPatient;    /*!< GUID du patient auquel on va coller le résultat de la consultation */
//  static QString m_MedinTux_Id_User;        /*!< Clé primaire de l'utilisateur de MedinTux */
//  static QString m_MedinTux_Observ_PrimKey; /*!< Clé primaire de l'observation depuis laquelle est appelé l'eDRC */
//  static QString m_Medintux_BaseName;       /*!< Nom de la base de données de DrTux*/

//  static QString m_BaseName;                /*!< Nom de la base de données */
//  static QString m_BaseLabel;               /*!< Label de la base de données */
//  static QString m_BaseDriver;              /*!< Driver de la base de données */
//  static QString m_BaseHostName;            /*!< Nom de l'hôte de la base de données */
//  static QString m_BaseUser;                /*!< Nom de l'utilisateur de la base pour la connection */
//  static QString m_BasePass;                /*!< Mot de passe de l'utilisateur de la base pour la connection */

//  static QString m_PatientName;             /*!< Nom du patient */
//  static QDateTime m_PatientDDN;
//  static QString m_PatientSexe;
//  static int     m_PatientIdFromBD;         /*!< Id du patient dans la base de données de l'eDRC */
//  static QString m_UserName;                /*!< Nom du patient */
//  static QDateTime m_ObservDate;            /*!< Date de l'observation depuis laquelle est appelée le plug */

//  static QString m_DiagA_Keys;              /*!< Raccourcis clavier pour la position diagnostique A */
//  static QString m_DiagB_Keys;              /*!< Raccourcis clavier pour la position diagnostique A */
//  static QString m_DiagC_Keys;              /*!< Raccourcis clavier pour la position diagnostique A */
//  static QString m_DiagD_Keys;              /*!< Raccourcis clavier pour la position diagnostique A */
//  static QString m_DiagZ_Keys;              /*!< Raccourcis clavier pour la position diagnostique A */
//  static QString m_SuiviN_Keys;             /*!< Raccourcis clavier pour le code suivi N */
//  static QString m_SuiviP_Keys;             /*!< Raccourcis clavier pour le code suivi P */
//  static QString m_SuiviR_Keys;             /*!< Raccourcis clavier pour le code suivi R */
//  static QString m_ALD_Yes_Keys;            /*!< Raccourcis clavier pour le code suivi R */
//  static QString m_ALD_No_Keys;             /*!< Raccourcis clavier pour le code suivi R */
//  static QString m_Sympto_Yes_Keys;         /*!< Raccourcis clavier pour le code suivi R */
//  static QString m_Sympto_No_Keys;          /*!< Raccourcis clavier pour le code suivi R */
//  static QString m_ConsultOrder;            /*!< Ordre de tri des consultations dans l'historique patient */
//  static QString m_DateFormat_CsHist;       /*!< Format de date pour l'affichage de l'historique des RC */
//  static QString m_DateFormat_Export;       /*!< Format de date pour l'exportation HTML des RC */
//  static QString m_DateFormat_Comment;      /*!< Format de date pour le commentaire des RC */

//  static QString m_DebugMessage;            /*!< Message pour débugger le plugin */

//  static bool    m_eDRCBase_Exists;         /*!< Renvoie l'état de création de la base du eDRC */
//  static bool    m_PatientBase_Exists;      /*!< Renvoie l'état de création de la base de données patients */

//};

#endif  // EDRC_CONSTANTS_H
