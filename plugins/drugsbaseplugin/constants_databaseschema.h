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
#ifndef DRUGSDB_CONSTANTS_DATABASESCHEMA_H
#define DRUGSDB_CONSTANTS_DATABASESCHEMA_H


/**
 * \file constants_databaseschema.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 24 Feb 2011
*/

namespace DrugsDB {
namespace Constants {

    // DATABASE
    const char * const DB_DRUGS_NAME     = "drugs";            /*!< \brief Drugs database connection name. */
    const char * const DB_DRUGS_FILENAME = "master.db";         /*!< \brief Drug database file name (for SQLite). */
    const char * const DB_DEFAULT_IDENTIFIANT  = "FR_AFSSAPS";

    //--------------------------------------------------------------------------------------------------------
    //------------------------------------ Enums for Database Management -------------------------------------
    //--------------------------------------------------------------------------------------------------------
    /** \brief Represents the tables index of drugs database */
    enum Tables
    {
        Table_MASTER = 0, Table_SOURCES, Table_LABELS,
        Table_LABELSLINK, Table_BIB, Table_BIB_LINK, Table_DRUGS,
        Table_MOLS, Table_COMPO, Table_UNITS, Table_LK_MOL_ATC, Table_PACKAGING,
        Table_DRUG_ROUTES, Table_DRUG_FORMS, Table_ROUTES, Table_SEARCHENGINES,
        Table_VERSION, Table_ATC, Table_INTERACTIONS, Table_IAKNOWLEDGE,
        Table_IA_IAK, Table_ATC_LABELS, Table_IAM_TREE,
        Table_MaxParam
    };

    enum ProtocolTables
    {
        Table_PROTOMASTER = 100, Table_PROTORELAT, Table_PROTODISTRIB, Table_PROTOTRANSMIT,
        Table_PROTODRUG, Table_PROTOATC, Table_PROTOPREC,
        Table_PROTO_MaxParam
    };

    enum MASTERFields {
        MASTER_DID, MASTER_UID1, MASTER_UID2, MASTER_UID3, MASTER_OLDUID,  MASTER_SID,
        MASTER_MaxParam
    };

    enum SOURCESFields {
        SOURCES_SID = 0, SOURCES_DBUID, SOURCES_MASTERLID, SOURCES_LANG,
        SOURCES_WEB, SOURCES_COPYRIGHT, SOURCES_DATE,
        SOURCES_DRUGS_VERSION, SOURCES_AUTHORS, SOURCES_VERSION, SOURCES_PROVIDER,
        SOURCES_WEBLINK, SOURCES_DRUGUID_NAME, SOURCES_ATC, SOURCES_INTERACTIONS,
        SOURCES_COMPL_WEBSITE, SOURCES_PACKUID_NAME, SOURCES_COMPLETION,
        SOURCES_AUTHOR_COMMENTS, SOURCES_DRUGNAMECONSTRUCTOR,  SOURCES_FMFCOMPAT,
        SOURCES_OPENREACT_COMPAT,
        SOURCES_MaxParam
    };

    enum LABELSFields {
        LABELS_LID = 0, LABELS_LANG, LABELS_LABEL,
        LABELS_MaxParam
    };

    enum LABELSLINKFields {
        LABELSLINK_MASTERLID = 0, LABELSLINK_LID,
        LABELSLINK_MaxParam
    };

    enum BIBFields {
        BIB_BIBID = 0, BIB_TYPE, BIB_LINK, BIB_TEXTREF, BIB_ABSTRACT, BIB_EXPLAIN,
        BIB_MaxParam
    };

    enum BIB_LINKFields {
        BIB_LINK_MASTERID = 0, BIB_LINK_BIBID,
        BIB_LINK_MaxParam
    };

    enum DRUGSFields {
        DRUGS_ID = 0, DRUGS_DID, DRUGS_SID, DRUGS_NAME,
        DRUGS_ATC_ID, DRUGS_STRENGTH, DRUGS_VALID,
        DRUGS_MARKET, DRUGS_AID_MASTER_LID, DRUGS_LINK_SPC, DRUGS_EXTRA_XML,
        DRUGS_MaxParam
    };

    enum MOLSFields {
        MOLS_MID, MOLS_SID, MOLS_NAME, MOLS_WWW,
        MOLS_MaxParam
    };

    enum COMPOFields {
        COMPO_DID = 0, COMPO_MID, COMPO_STRENGTH, COMPO_STRENGTH_NID,
        COMPO_DOSE_REF, COMPO_REF_NID, COMPO_NATURE, COMPO_LK_NATURE,
        COMPO_MaxParam
    };

    enum UNITSFields {
        UNITS_NID = 0, UNITS_VALUE,
        UNITS_MaxParam
    };

    enum LK_MOL_ATCFields {
        LK_MID = 0, LK_ATC_ID, LK_ATC_SID,
        LK_MaxParam
    };

    enum PACKAGINGfields {
        PACK_DID = 0, PACK_SID, PACK_PACK_UID, PACK_LABEL, PACK_STATUS, PACK_MARKET,
        PACK_DATE, PACK_OPTION_CODE,
        PACK_MaxParam
    };

    enum DRUG_ROUTESFields {
        DRUG_ROUTES_DID = 0, DRUG_ROUTES_RID,
        DRUG_ROUTES_MaxParam
    };

    enum DRUG_FORMSFields {
        DRUG_FORMS_DID = 0, DRUG_FORMS_MASTERLID,
        DRUG_FORMS_MaxParam
    };

    enum ROUTESFields {
        ROUTES_RID = 0, ROUTES_MASTERLID, ROUTES_SYSTEMIC,
        ROUTES_MaxParam
    };

    enum SEARCHENGINESFields {
        SEARCHENGINE_ID = 0, SEARCHENGINE_LABEL, SEARCHENGINE_URL,
        SEARCHENGINES_MaxParam
    };

    enum VERSIONFields {
        VERSION_ID = 0, VERSION_VERSION, VERSION_DATE, VERSION_COMMENT,
        VERSION_MaxParam
    };

    enum ATCFields {
        ATC_ID = 0, ATC_CODE,
        ATC_MaxParam
    };

    enum INTERACTIONSFields {
        INTERACTIONS_IAID = 0, INTERACTIONS_ATC_ID1, INTERACTIONS_ATC_ID2,
        INTERACTIONS_MaxParam
    };

    enum IAKNOWLEDGEFields {
        IAKNOWLEDGE_IAKID = 0, IAKNOWLEDGE_TYPE, IAKNOWLEDGE_RISK_MASTERLID,
        IAKNOWLEDGE_MANAGEMENT_MASTERLID, IAKNOWLEDGE_BIB_MASTERID, IAKNOWLEDGE_WWW,
        IAKNOWLEDGE_MaxParam
    };

    enum IA_IAKFields {
        IA_IAK_IAID = 0, IA_IAK_IAKID,
        IA_IAK_MaxParam
    };

    enum ATC_LABELSFields {
        ATC_LABELS_ATCID = 0, ATC_LABELS_MASTERLID,
        ATC_LABELS_MaxParam
    };

    enum IAM_TREEFields {
        IAM_TREE_ID_CLASS = 0, IAM_TREE_ID_ATC, IAM_TREE_BIBMASTERID,
        IAM_TREE_MaxParam
    };


}  // End namespace Constants

}  // End namespace DrugsDB

#endif // DRUGSDB_CONSTANTS_DATABASESCHEMA_H
