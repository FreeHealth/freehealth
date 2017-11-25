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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEDDIMANAGER_SPECIFIC_CORE_CONSTANTS_H
#define FREEDDIMANAGER_SPECIFIC_CORE_CONSTANTS_H

/**
 * \file fdm_constants.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 25 Sept 2013
*/

namespace Core {
namespace Constants {

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


// Enum for the command line params that can be used with
// Core::ICore::instance()->commandLine()
// Anywhere in the code
enum CommandLine {
    CL_MedinTux = 0,
    CL_Independant,
    CL_Movements,
    CL_PreferedReceipts,
    CL_Test,
    CL_ReceiptsCreator,
    CL_EMR_Name,
    CL_EMR_Uid,
    CL_SelectionOnly,
    CL_ExchangeOutFile,
    CL_ExchangeOutFileFormat,
    CL_ExchangeInFile,
    CL_PatientName,
    CL_PatientFirstname,
    CL_PatientUid,
    CL_PatientGender,
    CL_DateOfBirth,
    CL_BlockPatientDatas,
    CL_DbHost,
    CL_DbPort,
    CL_MaxParam
};

// Settings

}  // namespace Constants
}  // namespace Core

#endif // FREEDDIMANAGER_SPECIFIC_CORE_CONSTANTS_H
