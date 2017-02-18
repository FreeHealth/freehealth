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
#ifndef ICDCONSTANTS_H
#define ICDCONSTANTS_H

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 09 Oct 2010
*/


namespace ICD {
namespace Constants {

    const char * const DB_NAME = "icd10";

    // Database abstraction
    enum Tables {
        Table_Chapter,
        Table_Common,
        Table_Dagstar,
        Table_Descr,
        Table_Exclude,
        Table_Glossaire,
        Table_Html,
        Table_Include,
        Table_Indir,
        Table_Libelle,
        Table_Master,
        Table_Memo,
        Table_Note,
        Table_Ref,
        Table_System,
        Table_Version
    };


    enum TableChapter_Fields {
        CHAPTER_CHAPTER = 0,
        CHAPTER_SID,
        CHAPTER_ROM
    };

    enum TableCommon_Fields {
        COMMON_SID = 0,
        COMMON_MALE,
        COMMON_FEMALE,
        COMMON_SEQUELLA,
        COMMON_POST,
        COMMON_SECOND,
        COMMON_NOCODE
    };

    enum TableDagstar_Fields {
        DAG_SID = 0,
        DAG_LID,
        DAG_ASSOC,
        DAG_DAGET,
        DAG_PLUS
    };

    enum TableDescr_Fields {
        DESCR_SID = 0,
        DESCR_LID
    };

    enum TableExclude_Fields {
        EXCLUDE_SID = 0,
        EXCLUDE_EXCL,
        EXCLUDE_PLUS,
        EXCLUDE_LID,
        EXCLUDE_DAGET
    };

    enum TableGlossaire_Fields {
        GLOSSAIRE_SID = 0,
        GLOSSAIRE_MID
    };

    enum TableHtml_Fields {
        HTML_REF = 0,
        HTML_FR,
        HTML_EN,
        HTML_DE
    };

    enum TableInclude_Fields {
        INCLUDE_SID = 0,
        INCLUDE_LID
    };

    enum TableIndir_Fields {
        INDIR_SID = 0,
        INDIR_LID
    };

    enum TableLibelle_Fields {
        LIBELLE_LID = 0,
        LIBELLE_SID,
        LIBELLE_SOURCE,
        LIBELLE_VALID,
        LIBELLE_LIBELLE,
        LIBELLE_FR,
        LIBELLE_EN,
        LIBELLE_DE_DIMDI,
        LIBELLE_DE_AUTO,
        LIBELLE_FRCHRONOS,
        LIBELLE_DATE,
        LIBELLE_AUTHOR,
        LIBELLE_COMMENT
    };

    enum TableMaster_Fields {
        MASTER_SID = 0,
        MASTER_CODE,
        MASTER_SORT,
        MASTER_ABBREV,
        MASTER_LEVEL,
        MASTER_TYPE,
        MASTER_ID1,
        MASTER_ID2,
        MASTER_ID3,
        MASTER_ID4,
        MASTER_ID5,
        MASTER_ID6,
        MASTER_ID7,
        MASTER_VALID,
        MASTER_DATE,
        MASTER_AUTHOR,
        MASTER_COMMENT
    };

    enum TableMemo_Fields {
        MEMO_MID = 0,
        MEMO_SID,
        MEMO_SOURCE,
        MEMO_VALID,
        MEMO_MEMO,
        MEMO_FR,
        MEMO_EN,
        MEMO_DE,
        MEMO_DATE,
        MEMO_AUTHOR,
        MEMO_COMMENT
    };

    enum TableNote_Fields {
        NOTE_SID = 0,
        NOTE_MID
    };

    enum TableRef_Fields {
        REF_SID = 0,
        REF_LID,
        REF_REF
    };

    enum TableSystem_Fields {
        SYSTEM_SID = 0,
        SYSTEM_LID
    };

    enum TableVersion_Fields {
        VERSION_NAME = 0,
        VERSION_VERSION,
        VERSION_BUILD,
        VERSION_VALID,
        VERSION_DATE,
        VERSION_COMMENT
    };

    // File association
    const char * const  FREEICD_FILEFILTER = QT_TRANSLATE_NOOP("ICD", "ICD Collection file (*.icd)");

    // Contexts
    const char * const  C_ICD_PLUGINS = "ICD.cGlobal";
    const char * const  G_ICD_DATABASES = "grICD.Db";
    const char * const  G_ICD_SEARCH = "grICDSearch";
    const char * const  G_ICD_SELECTORMODE = "grICDSelectorMode";
    const char * const  G_ICD_COLLECTIONMODE = "grICDCollMode";

    // Menu and action for ActionManager
    const char * const  M_PLUGIN_ICD       = "mICDPlugin";
    const char * const  M_ICD_SEARCH       = "mICDSearch";
    const char * const  M_ICD_MODES        = "mICDModes";
    const char * const  A_RECREATE_ICD_DB  = "aRecreateICDDatabase";
    const char * const  A_DATABASE_INFOS   = "aICDDbInformation";
    const char * const  A_SEARCH_LABEL     = "aICDSearchLabel";
    const char * const  A_SEARCH_CODE      = "aICDSearchCode";

    // Modes
    const char * const  A_SELECTOR_SIMPLEMODE   = "aICDSelSimple";
    const char * const  A_SELECTOR_FULLMODE     = "aICDSelFull";
    const char * const  A_COLLECTION_SIMPLEMODE = "aICDCollSimple";
    const char * const  A_COLLECTION_FULLMODE   = "aICDCollFull";

    const char * const  A_TOGGLE_ICDSELECTOR = "aICDToggleSelector";
    const char * const  A_PRINT_COLLECTION   = "aICDPrintCollection";


    // Icons
    const char * const  I_SEARCH_LABEL     = "search.png";
    const char * const  I_SEARCH_CODE      = "search.png";
    const char * const  I_TOGGLEICDSELECTOR = "icd.png";

    // Tokens
    const char * const  T_IS_PRINTING_ICDCOLLECTION = "IS_ICD";
    const char * const  T_ICD_DATABASE_VERSION = "ICD_DATABASEVERSION";
    const char * const  T_ICD_PLUGIN_VERSION = "ICD_PLUGINVERSION";


    // Translations
    const char * const  ICDCONSTANTS_TR_CONTEXT = "ICD";
    const char* const   RECREATE_DATABASE_TEXT  = QT_TRANSLATE_NOOP("ICD", "Recreate ICD10 database");
    const char* const   DATABASE_INFOS_TEXT     = QT_TRANSLATE_NOOP("ICD", "ICD10 database information");
    const char* const   SEARCHMENU_TEXT         = QT_TRANSLATE_NOOP("ICD", "ICD10 search method");
    const char* const   SEARCHLABEL_TEXT        = QT_TRANSLATE_NOOP("ICD", "Search by label");
    const char* const   SEARCHCODE_TEXT         = QT_TRANSLATE_NOOP("ICD", "Search by code");
    const char* const   MODESMENU_TEXT          = QT_TRANSLATE_NOOP("ICD", "ICD models modes");
    const char* const   TOGGLEICDSELECTOR_TEXT  = QT_TRANSLATE_NOOP("ICD", "Toggle code selector");
    const char* const   PRINTCOLLECTION_TEXT    = QT_TRANSLATE_NOOP("ICD", "Print ICD10 code collection");
    const char* const   SELECTORSIMPLEMODE_TEXT = QT_TRANSLATE_NOOP("ICD", "Selector: simple list mode");
    const char* const   SELECTORFULLMODE_TEXT   = QT_TRANSLATE_NOOP("ICD", "Selector: full mode");
    const char* const   COLLECTIONSIMPLEMODE_TEXT= QT_TRANSLATE_NOOP("ICD", "Collection: simple list mode");
    const char* const   COLLECTIONFULLMODE_TEXT = QT_TRANSLATE_NOOP("ICD", "Collection: full mode");
    const char* const   ICD_CODECOLLECTION_TEXT = QT_TRANSLATE_NOOP("ICD", "ICD10 code collection");

    // Settings and tokens
    const char * const  S_STATEPREFIX            = "ICD/";
    const char * const  S_USER_HEADER            = "UserHeader";
    const char * const  S_USER_FOOTER            = "UserFooter";
    const char * const  S_USER_WATERMARK         = "UserWatermark";


}  // End namespace Constants
}  // End namespace ICD

#endif // ICDCONSTANTS_H
