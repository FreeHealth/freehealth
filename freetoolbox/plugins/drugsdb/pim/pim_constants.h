/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PIM_CONSTANTS_H
#define PIM_CONSTANTS_H

namespace DrugsDB {
namespace Constants {

const char * const XML_TAG_ROOT = "FreeMedForms_PIMs";

const char * const XML_TAG_PIM_ROOT = "PIMs";
const char * const XML_TAG_PIM_SOURCE = "PIM_Source";
const char * const XML_TAG_PIM_TYPE = "PIM_Type";
const char * const XML_TAG_PIM = "PIM";

const char * const XML_TAG_RISK = "Risk";
const char * const XML_TAG_MOLECULES = "Molecules";
const char * const XML_TAG_MOLECULE = "Molecule";
const char * const XML_TAG_DISEASES = "Diseases";
const char * const XML_TAG_DISEASE = "Disease";

const char * const XML_TAG_SOURCE_ROOT = "PIMSources";
const char * const XML_TAG_SOURCE = "Source";
const char * const XML_TAG_TYPE_ROOT = "PIMTypes";
const char * const XML_TAG_TYPE = "Type";

const char * const XML_ATTRIB_SOURCE_ID = "id";
const char * const XML_ATTRIB_SOURCE_NAME = "name";
const char * const XML_ATTRIB_SOURCE_PMID = "pmid";
const char * const XML_ATTRIB_SOURCE_COUNTRY = "country";
const char * const XML_ATTRIB_SOURCE_WWW = "www";

const char * const XML_ATTRIB_TYPE_ID = "id";
const char * const XML_ATTRIB_TYPE_FR = "fr";
const char * const XML_ATTRIB_TYPE_EN = "en";
const char * const XML_ATTRIB_TYPE_DE = "de";
const char * const XML_ATTRIB_TYPE_ES = "es";

const char * const XML_ATTRIB_RISK_LANG = "lang";
const char * const XML_ATTRIB_RISK_VALUE = "value";

const char * const XML_ATTRIB_PIM_TYPE = "type";
const char * const XML_ATTRIB_PIM_LEVEL = "level";

const char * const XML_ATTRIB_ICDGROUP = "icd";
const char * const XML_ATTRIB_CLASSNAME = "class";
const char * const XML_ATTRIB_MAXDAILYDOSE = "maxDailyDose";
const char * const XML_ATTRIB_MAXDAILYDOSEUNIT = "maxDailyDoseUnit";

}  //  namespace Constants
}  //  namespace DrugsDB

#endif // PIM_CONSTANTS_H
