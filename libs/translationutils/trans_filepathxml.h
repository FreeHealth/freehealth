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
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_FILEPATHXML_H
#define CONSTANTTRANSLATIONS_FILEPATHXML_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// File filters
const char * const FILE_FILTER_ZIP                   = QT_TRANSLATE_NOOP("tkConstants", "Zip archive (*.zip)");
const char * const FILE_FILTER_HTML                  = QT_TRANSLATE_NOOP("tkConstants", "HTML files (*.htm *.html)");
const char * const FILE_FILTER_XML                   = QT_TRANSLATE_NOOP("tkConstants", "XML files (*.xml)");
const char * const FILE_FILTER_TXT                   = QT_TRANSLATE_NOOP("tkConstants", "Text files (*.txt)");
const char * const FILE_FILTER_RTF                   = QT_TRANSLATE_NOOP("tkConstants", "Rich text files (*.rtf)");
const char * const FILE_FILTER_ODT                   = QT_TRANSLATE_NOOP("tkConstants", "OpenOffice document (*.odt)");
const char * const FILE_FILTER_ALL_FILES             = QT_TRANSLATE_NOOP("tkConstants", "All files(*)");

// Files and Paths
const char* const PATH                               = QT_TRANSLATE_NOOP("tkConstants", "Path");
const char* const PATH_1_DOESNOT_EXISTS              = QT_TRANSLATE_NOOP("tkConstants", "Path %1 does not exist");
const char* const PATH_1_ISNOT_EMPTY                 = QT_TRANSLATE_NOOP("tkConstants", "Path %1 is not empty");
const char* const PATH_1_IS_EMPTY                    = QT_TRANSLATE_NOOP("tkConstants", "Path %1 is empty");
const char* const PATH_1_CONTAINS_DIRS               = QT_TRANSLATE_NOOP("tkConstants", "Path %1 contains dirs");
const char* const PATH_1_CANNOT_BE_REMOVED           = QT_TRANSLATE_NOOP("tkConstants", "Path %1 can not be removed");
const char* const PATH_1_CANNOT_BE_CREATED           = QT_TRANSLATE_NOOP("tkConstants", "Path %1 can not be created");
const char* const CREATE_DIR_1                       = QT_TRANSLATE_NOOP("tkConstants", "Creating dir %1");
const char* const FILE_1_DOESNOT_EXISTS              = QT_TRANSLATE_NOOP("tkConstants", "File %1 does not exist");
const char* const FILE_1_ISNOT_READABLE              = QT_TRANSLATE_NOOP("tkConstants", "File %1 is not readable");
const char* const FILE_1_CAN_NOT_BE_CREATED          = QT_TRANSLATE_NOOP("tkConstants", "File %1 can not be created");
const char* const FILE_1_CANNOT_BE_REMOVED           = QT_TRANSLATE_NOOP("tkConstants", "File %1 can not be removed");
const char* const FILE_1_LOADED                      = QT_TRANSLATE_NOOP("tkConstants", "File %1 loaded");
const char* const FILE_1_ISEMPTY                     = QT_TRANSLATE_NOOP("tkConstants", "File %1 is empty");
const char* const FILE_1_CORRECTLY_REMOVED           = QT_TRANSLATE_NOOP("tkConstants", "File %1 correctly removed");
const char* const FILE_1_CAN_NOT_BE_COPIED           = QT_TRANSLATE_NOOP("tkConstants", "File %1 can not be copied");
const char* const FILE_1_MOVE_TO_2                   = QT_TRANSLATE_NOOP("tkConstants", "File %1 moved to %2");

const char* const OPEN_FILE                          = QT_TRANSLATE_NOOP("tkConstants", "Open file");
const char* const OPENING_FILE_1                     = QT_TRANSLATE_NOOP("tkConstants", "Opening file %1");
const char* const SAVING_FILE_1                      = QT_TRANSLATE_NOOP("tkConstants", "Saving file %1");
const char* const SAVE_FILE                          = QT_TRANSLATE_NOOP("tkConstants", "Save file");

const char* const XML_WRONG_NUMBER_OF_TAG_1          = QT_TRANSLATE_NOOP("tkConstants", "Wrong number of tags (%1)");
const char* const XML_WRONG_ROOT_TAG_1_2             = QT_TRANSLATE_NOOP("tkConstants", "Wrong root tag %1 %2.");
const char* const XML_DOCUMENT_TYPE_MISMATCH         = QT_TRANSLATE_NOOP("tkConstants", "Document type name mismatch.");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_FILEPATHXML_H
