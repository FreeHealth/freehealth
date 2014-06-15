/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2014 by Eric MAEKER, MD                                      *
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
#ifndef FREETOOLS_CONSTANTS_H
#define FREETOOLS_CONSTANTS_H

#include <QString>


namespace Core {

namespace Constants {

#ifdef Q_OS_MAC
    const QString MACBUNDLE = "/../../../";
#else
    const QString MACBUNDLE = "";
#endif

    const char * const  FREETOOLBOX_TR_CONTEXT = "FreeTools";

    const char * const  CATEGORY_FREEDRUGSDATABASE = QT_TRANSLATE_NOOP("FreeTools", "Free drugs databases");
    const char * const  CATEGORY_NONFREEDRUGSDATABASE = QT_TRANSLATE_NOOP("FreeTools", "Non-free drugs databases");
    const char * const  CATEGORY_INTERACTIONSDATABASE = QT_TRANSLATE_NOOP("FreeTools", "Interactions databases");
    const char * const  CATEGORY_DRUGINFOSDATABASE = QT_TRANSLATE_NOOP("FreeTools", "Drugs infos databases");
    const char * const  CATEGORY_ATC_HELPERS = QT_TRANSLATE_NOOP("FreeTools", "ATC Helpers");
    const char * const  CATEGORY_ICD10DATABASE = QT_TRANSLATE_NOOP("FreeTools", "ICD 10");
    const char * const  CATEGORY_ZIPCODESDATABASE = QT_TRANSLATE_NOOP("FreeTools", "ZipCodes");

    const char * const  CREATEFULLRELEASE_TEXT = QT_TRANSLATE_NOOP("FreeTools", "Create full release data set");

    const char * const  MASTER_DATABASE_FILENAME                 =  "/drugs/master.db";
    const char * const  MASTER_DATABASE_NAME                     =  "drugs";

    const char * const  FILE_MASTER_DATABASE_SCHEME              = "/global_resources/sql/drugdb/newdbschema.sql";
    const char * const  FILE_DRUGINFOS_DATABASE_SCHEME           = "/global_resources/sql/druginfodb/biam2/druginfos_schema.sql";
    const char * const  FILE_DRUGS_ROUTES                        = "/global_resources/sql/drugdb/routes.txt";

    const char * const  MOL_LINK_FILENAME                        = "/global_resources/nonfree/ddi/moleculetoatclinker.xml";
    const char * const  ATC_FILENAME                             = "/global_resources/nonfree/ddi/atc_utf8.csv";

    const char * const  NEW_INTERACTORS_FILENAME                 = "/global_resources/nonfree/ddi/ddi_interactors_2011.xml";
    const char * const  AFSSAPS_MOLECULE_LINK_FILENAME           = "/global_resources/nonfree/ddi/afssaps_links.xml";
    const char * const  AFSSAPS_CLASSTREE_FILENAME               = "/global_resources/nonfree/ddi/afssaps_classtree.xml";
    const char * const  AFSSAPS_INTERACTIONS_FILENAME            = "/global_resources/nonfree/ddi/thesaurus_interactions.xml";
    const char * const  NEW_AFSSAPS_INTERACTIONS_FILENAME        = "/global_resources/nonfree/ddi/ddi_2011.xml";
    const char * const  INTERACTIONS_ENGLISHCORRECTIONS_FILENAME = "/global_resources/nonfree/ddi/interaction_text_english_translation_corrections.csv";
    const char * const  PIMS_FILENAME                            = "/global_resources/nonfree/pims/pims.xml";
    const char * const  ICD_GROUP_LINKER_FILENAME                = "/global_resources/nonfree/pims/icdlinks.xml";

    const char* const S_WINGEOMETRY      = "MainWin/Geometry";
    const char* const S_WINSTATE         = "MainWin/State";
    const char* const S_WINSPLITTER      = "MainWin/Splitter";

    const char* const S_FILEOUTPUT_PATH  = "Path/FileOutput";
    const char* const S_DBOUTPUT_PATH    = "Path/DatabaseOutput";
    const char* const S_DATAPACK_SERVER_OUTPUT_PATH    = "Path/DatapackServerOutput";
    const char* const S_TMP_PATH         = "Path/Temporary";
    const char* const S_GITFILES_PATH    = "Path/ToSvnCheckout";

    const char* const PATH_TO_DRUG_DATABASE_DESCRIPTION_FILES = "/global_resources/sql/drugdb/";
    const char* const PATH_TO_DATAPACK_DESCRIPTION_FILES = "/global_resources/datapack_description/";

}  //  End namespace Constants

}  //  End namespace DrugsDbCreator

#endif // FREETOOLS_CONSTANTS_H
