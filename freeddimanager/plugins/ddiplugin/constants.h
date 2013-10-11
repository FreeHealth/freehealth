/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_PLUGIN_CONSTANTS_H
#define DDIMANAGER_PLUGIN_CONSTANTS_H

#include <QString>

namespace DDI {
namespace Constants {

#ifdef Q_OS_MAC
const QString MACBUNDLE = "/../../../";
#else
const QString MACBUNDLE = "";
#endif

const char * const  TR_CONTEXT = "DDI";

const char * const CATEGORY_FREEDRUGSDATABASE = QT_TRANSLATE_NOOP("FreeTools", "Free drugs databases");

const char * const DDIMANAGER_DATABASE_FILENAME  =  "ddimanager.db";
const char * const DDIMANAGER_DATABASE_NAME      =  "ddimanager";

const char * const ATC_CSV_FILENAME                         = "/nonfree/ddi/atc_utf8.csv";
const char * const INTERACTORS_XML_FILENAME                 = "/nonfree/ddi/ddi_interactors_latest.xml";
const char * const ROUTES_CSV_FILENAME                      = "/sql/drugdb/routes.txt";

// Database
enum Tables {
    Table_ATC = 0,
    Table_Interactors,
    Table_CURRENTVERSION
};

enum TableATC_Fields {
    ATC_ID = 0,
    ATC_ISVALID,
    ATC_CODE,
    ATC_UID,
    ATC_FR, ATC_EN, ATC_DE, ATC_SP,
    ATC_DATECREATE,
    ATC_DATEUPDATE,
    ATC_PREVIOUSCODE,
    ATC_WHOYEARUPDATE,
    ATC_COMMENT,
    ATC_MaxParams
};

enum TableInteractors_Fields {
    INTERACTOR_ID = 0,
    INTERACTOR_UID,
    INTERACTOR_ISVALID,
    INTERACTOR_ISREVIEWED,
    INTERACTOR_ISAUTOFOUND,
    INTERACTOR_WARNDUPLICATES,
    INTERACTOR_ATC,
    INTERACTOR_CHILDREN,
    INTERACTOR_FR,
    INTERACTOR_EN,
    INTERACTOR_DE,
    INTERACTOR_INFO_FR,
    INTERACTOR_INFO_EN,
    INTERACTOR_INFO_DE,
    INTERACTOR_DATECREATE,
    INTERACTOR_DATEUPDATE,
    INTERACTOR_DATEREVIEW,
    INTERACTOR_REF,
    INTERACTOR_PMIDS,
    INTERACTOR_COMMENT,
    INTERACTOR_MaxParam
};

enum TableVERSION_Fields {
    CURRENTVERSION_ID = 0,
    CURRENTVERSION_NUMBER
};

//    const char * const MOL_LINK_FILENAME                        = "/global_resources/nonfree/ddi/moleculetoatclinker.xml";

//    const char * const  NEW_INTERACTORS_FILENAME                 = "/global_resources/nonfree/ddi/ddi_interactors_2011.xml";
//    const char * const  AFSSAPS_MOLECULE_LINK_FILENAME           = "/global_resources/nonfree/ddi/afssaps_links.xml";
//    const char * const  AFSSAPS_CLASSTREE_FILENAME               = "/global_resources/nonfree/ddi/afssaps_classtree.xml";
//    const char * const  AFSSAPS_INTERACTIONS_FILENAME            = "/global_resources/nonfree/ddi/thesaurus_interactions.xml";
//    const char * const  NEW_AFSSAPS_INTERACTIONS_FILENAME        = "/global_resources/nonfree/ddi/ddi_2011.xml";
//    const char * const  INTERACTIONS_ENGLISHCORRECTIONS_FILENAME = "/global_resources/nonfree/ddi/interaction_text_english_translation_corrections.csv";
//    const char * const  PIMS_FILENAME                            = "/global_resources/nonfree/pims/pims.xml";
//    const char * const  ICD_GROUP_LINKER_FILENAME                = "/global_resources/nonfree/pims/icdlinks.xml";

const char* const S_WINGEOMETRY      = "MainWin/Geometry";
const char* const S_WINSTATE         = "MainWin/State";
const char* const S_WINSPLITTER      = "MainWin/Splitter";

//    const char* const S_FILEOUTPUT_PATH  = "Path/FileOutput";
//    const char* const S_DBOUTPUT_PATH    = "Path/DatabaseOutput";
//    const char* const S_DATAPACK_SERVER_OUTPUT_PATH    = "Path/DatapackServerOutput";
//    const char* const S_TMP_PATH         = "Path/Temporary";
//    const char* const S_GITFILES_PATH    = "Path/ToSvnCheckout";

//    const char* const PATH_TO_DRUG_DATABASE_DESCRIPTION_FILES = "/global_resources/sql/drugdb/";
//    const char* const PATH_TO_DATAPACK_DESCRIPTION_FILES = "/global_resources/datapack_description/";

}  //  End namespace Constants
}  //  End namespace DDI

#endif // DDIMANAGER_PLUGIN_CONSTANTS_H
