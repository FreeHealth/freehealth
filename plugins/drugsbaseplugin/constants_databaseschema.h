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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_CONSTANTS_DATABASESCHEMA_H
#define DRUGSDB_CONSTANTS_DATABASESCHEMA_H

/**
 * \file constants_databaseschema.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 12 Dec 2011
*/

namespace DrugsDB {
namespace Constants {

    // DATABASE
    const char * const DB_DRUGS_NAME     = "drugs";            /*!< \brief Drugs database connection name. */
    const char * const DB_DRUGS_FILENAME = "master.db";         /*!< \brief Drug database file name (for SQLite). */
    const char * const DB_DEFAULT_IDENTIFIANT  = "FR_AFSSAPS";
    const char * const DB_FDA_IDENTIFIANT  = "FDA_US";

    //--------------------------------------------------------------------------------------------------------
    //------------------------------------ Enums for Database Management -------------------------------------
    //--------------------------------------------------------------------------------------------------------
    /** \brief Represents the tables index of drugs database */
    enum Tables  // ORDER OF THIS ENUM MUST NEVER CHANGE
    {
        Table_MASTER = 0, Table_SOURCES, Table_DRUGS, Table_MOLS, Table_COMPO,
        Table_DRUG_FORMS, Table_DRUG_ROUTES, Table_ROUTES, Table_SEARCHENGINES,
        Table_LABELS, Table_LABELSLINK, Table_PACKAGING, Table_BIB, Table_BIB_LINK, Table_UNITS,

        Table_ATC, Table_ATC_LABELS, Table_IAM_TREE, Table_LK_MOL_ATC,
        Table_INTERACTIONS, Table_IAKNOWLEDGE, Table_IA_IAK, Table_PIM_SOURCES,

        Table_PIM_TYPES, Table_PIMS, Table_PIMS_RELATED_ATC,
        Table_PIMS_RELATED_ICD, Table_VERSION, Table_CURRENTVERSION,

        Table_MaxParam
    };

    enum ProtocolTables
    {
        Table_PROTOMASTER = 100, Table_PROTORELAT, Table_PROTODISTRIB, Table_PROTOTRANSMIT,
        Table_PROTODRUG, Table_PROTOATC, Table_PROTOPREC,
        Table_PROTO_MaxParam
    };

    enum MASTERFields {
        MASTER_DID = 0, MASTER_UID1, MASTER_UID2, MASTER_UID3, MASTER_OLDUID,  MASTER_SID,
        MASTER_MaxParam
    };

    enum SOURCESFields {
        SOURCES_SID = 0, SOURCES_DBUID, SOURCES_MASTERLID, SOURCES_LANG,
        SOURCES_WEB, SOURCES_COPYRIGHT, SOURCES_LICENSE, SOURCES_DATE,
        SOURCES_DRUGS_VERSION, SOURCES_AUTHORS,  // 10
        SOURCES_VERSION, SOURCES_PROVIDER,
        SOURCES_WEBLINK, SOURCES_DRUGUID_NAME, SOURCES_ATC, SOURCES_INTERACTIONS,
        SOURCES_COMPL_WEBSITE, SOURCES_PACKUID_NAME, SOURCES_COMPLETION,
        SOURCES_AUTHOR_COMMENTS, //10
        SOURCES_DRUGNAMECONSTRUCTOR,  SOURCES_FMFCOMPAT,
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
        BIB_BIBID = 0, BIB_TYPE, BIB_LINK, BIB_TEXTREF, BIB_ABSTRACT, BIB_EXPLAIN, BIB_XML,
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
        MOLS_MID = 0, MOLS_SID, MOLS_NAME, MOLS_WWW,
        MOLS_MaxParam
    };

    enum COMPOFields {
        COMPO_ID = 0, COMPO_DID, COMPO_MID, COMPO_STRENGTH, COMPO_STRENGTH_NID,
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
        ATC_ID = 0, ATC_CODE, ATC_WARNDUPLICATES,
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
        IAM_TREE_ID = 0, IAM_TREE_ID_CLASS, IAM_TREE_ID_ATC, IAM_TREE_BIBMASTERID,
        IAM_TREE_MaxParam
    };

    enum PIM_SOURCESFields {
        PIM_SOURCES_SID = 0, PIM_SOURCES_UID, PIM_SOURCES_NAME, PIM_SOURCES_PMID,
        PIM_SOURCES_COUNTRY, PIM_SOURCES_WWW,
        PIM_SOURCES_MaxParam
    };

    enum PIM_TYPESFields {
        PIM_TYPES_TID = 0, PIM_TYPES_UID, PIM_TYPES_MASTER_LID,
        PIM_TYPES_MaxParam
    };

    enum PIMSFields {
        PIMS_ID = 0, PIMS_SID, PIMS_TID, PIMS_LEVEL, PIMS_RISK_MASTER_LID,
        PIMS_MaxParam
    };

    enum PIMS_RELATED_ATCFields {
        PIMS_RELATC_RMID = 0, PIMS_RELATC_PIM_ID, PIMS_RELATC_ATC_ID,
        PIMS_RELATC_MAXDAYDOSE, PIMS_RELATC_MAXDAYDOSEUNIT,
        PIMS_RELATC_MaxParam
    };

    enum PIMS_RELATED_ICDFields {
        PIMS_RELICD_RMID = 0, PIMS_RELICD_PIM_ID, PIMS_RELICD_ICD_SID,
        PIMS_RELICD_MaxParam
    };

    enum CURRENTVERSIONFields {
        CURRENTVERSION_ID = 0, CURRENTVERSION_NUMBER,
        CURRENTVERSION_MaxParam
    };

}  // End namespace Constants

}  // End namespace DrugsDB

#endif // DRUGSDB_CONSTANTS_DATABASESCHEMA_H
