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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef XMLIO_CONSTANTS_H
#define XMLIO_CONSTANTS_H

#include <QStringList>

namespace XmlForms {
namespace Constants {

    // User Database connection
    const char * const DB_NAME                      = "xmlforms";
    const char * const DB_VERSION                   = "0.1";
    const int          DB_CURRENT_CODE_VERSION      = 1;

    // For database management
    /** \brief Enumerates all the tables of the users' database */
    enum Tables
    {
        Table_FORMS = 0,
        Table_FORM_CONTENT,
        Table_SCHEMA,
        Table_MaxParam
    };

    /** \brief Enumerates the fields of the table USERS */
    enum fieldsFORMS
    {
        FORM_ID = 0,
        FORM_UUID, FORM_ORIGINALUID, FORM_ORIGINALDATE,
        FORM_MaxParam
    };

    enum fieldsFORMS_CONTENT {
        FORMCONTENT_ID = 0,
        FORMCONTENT_FORM_ID,
        FORMCONTENT_TYPE,
        FORMCONTENT_MODENAME,
        FORMCONTENT_ISVALID,
        FORMCONTENT_ISORIGINAL,
        FORMCONTENT_CONTENT,
        FORMCONTENT_MaxParam
    };

    enum SchemaFields {
        SCHEMA_ID =0 ,
        SCHEMA_VERSION,
        SCHEMA_SCRIPT,
        SCHEMA_TIMESTAMP
    };

    /** \todo Add an attribute when creating Forms/Page --> don't present in MainWindow::centralWidget's TreeWidget */
    /** \todo Add an attribute when creating Scripts --> load script from file 'fileName/#anchor' so that file can contain a multitude of scripts marked with an anchor. */
    /** \todo Manage multilingual specs. */

const char* const DOCTYPE_NAME     = "freemedforms";
const char* const DOCTYPE_EXTENSION= "xml";
const char* const PMHXCATEGORIES_FILENAME = "pmhcategories.xml";

const char* const TAG_MAINXMLTAG   = "FreeMedForms";
const char* const TAG_NEW_FORM     = "MedForm";
const char* const TAG_NEW_PAGE     = "Page";
const char* const TAG_NEW_ITEM     = "Item";
const char* const TAG_PMHX_CATEGORIES = "PMHx";
const char* const TAG_CATEGORY     = "Category";
const char* const TAG_ADDFILE      = "file";
const char* const TAG_NAME         = "name";
const char* const TAG_TABSTOPS     = "tabstops";
const char* const TAG_TABSTOP      = "tabstop";

const char* const TAG_UUID_EQUIVALENCE_MAIN = "fieldequivalence";
const char* const TAG_UUID_EQUIVALENCE_ITEM = "equivalent";
const char* const ATTRIB_OLD_UUID = "old";
const char* const ATTRIB_NEW_UUID = "new";
const char* const ATTRIB_UUID_EQUIVALENCE_VERSION = "setInVersion";

const char* const TAG_SPEC_AUTHORS      = "authors";
const char* const TAG_SPEC_CATEGORY     = "category";
const char* const TAG_SPEC_LICENSE      = "license";
const char* const TAG_SPEC_SPECIALTIES  = "specialties";
const char* const TAG_SPEC_CREATIONDATE = "cdate";
const char* const TAG_SPEC_LASTMODIFDATE= "lmdate";
const char* const TAG_SPEC_BIBLIOGRAPHY = "bibliography";
const char* const TAG_SPEC_DESCRIPTION  = "description";
const char* const TAG_SPEC_HTMLDESCRIPTION  = "htmldescription";
const char* const TAG_SPEC_HTMLSYNTHESIS  = "htmlsynthesis";
const char* const TAG_SPEC_LABEL        = "label";
const char* const TAG_SPEC_EXTRALABEL   = "extralabel";
const char* const TAG_SPEC_PLUGINNAME   = "type";
const char* const TAG_FORM_DESCRIPTION  = "formdescription";
const char* const TAG_SPEC_VERSION      = "version";
const char* const TAG_SPEC_COMPTAVERSION= "compatversion";
const char* const TAG_SPEC_WEBLINK      = "weblink";
const char* const TAG_SPEC_ICON         = "icon";
const char* const TAG_SPEC_TOOLTIP      = "tooltip";
const char* const TAG_SPEC_PLACEHOLDER  = "placeholder";
const char* const TAG_SPEC_PRIORITY      = "priority";
const char* const TAG_SPEC_EXTRA        = "extra";
const char* const TAG_SPEC_UPDATEINFO   = "updateinfo";
const char* const TAG_SPEC_UPDATEINFOVERSION  = "updateversion";
const char* const TAG_SPEC_UPDATEINFOVERSIONTEXT  = "text";
const char* const TAG_SPEC_HPRIM        = "hprim";

const char* const ATTRIB_UPDATEINFOVERSION_FROM  = "from";
const char* const ATTRIB_UPDATEINFOVERSION_TO    = "to";

const char* const TAG_UI_UILINK         = "uilink";
const char* const ATTRIB_UI_UILINK_LABEL   = "label";
const char* const ATTRIB_UI_UILINK_WIDGET  = "widget";
const char* const ATTRIB_UI_UILINK_INSERTINTOLAYOUT  = "layout";

const char* const TAG_VALUE              = "value";
const char* const TAG_VALUE_UUID         = "uuid";
const char* const TAG_VALUE_NUMERICAL    = "numerical";
const char* const TAG_VALUE_POSSIBLE     = "possible";
const char* const TAG_VALUE_PRINT        = "print";
const char* const TAG_VALUE_SCRIPT       = "script";
const char* const TAG_VALUE_DEPENDENCIES = "dependon";
const char* const TAG_VALUE_DEFAULT      = "default";

const char* const TAG_SCRIPT                    = "script";
const char* const TAG_SCRIPT_ONLOAD             = "onload";
const char* const TAG_SCRIPT_POSTLOAD           = "postload";
const char* const TAG_SCRIPT_ONDEMAND           = "ondemand";
const char* const TAG_SCRIPT_ONVALUECHANGED     = "onvaluechanged";
const char* const TAG_SCRIPT_ONVALUEREQUIRED    = "onvaluerequired";
const char* const TAG_SCRIPT_ONDEPENDENCIESCHANGED = "ondependencieschanged";
const char* const TAG_SCRIPT_ONCLICKED          = "onclicked";
const char* const TAG_SCRIPT_ONTOGGLED          = "ontoggled";

const char* const TAG_DATAPATIENT                = "patientdata";

const char* const TAG_OPTIONS                       = "options";
const char* const TAG_OPTION                        = "option";
const char* const TAG_OPTIONS_UNIQUE_EPISODE        = "unique";
const char* const TAG_OPTIONS_NO_EPISODE            = "noepisode";
const char* const TAG_OPTIONS_NO_HEADER             = "noheader";
const char* const TAG_OPTIONS_GENERIC_HEADER        = "genericheader";
const char* const TAG_OPTIONS_ADMINISTRATIVE_HEADER = "administrativeheader";
const char* const TAG_OPTIONS_PRESCRIPTION_HEADER   = "prescriptionheader";

const char* const ATTRIB_ID             = "id";
const char* const ATTRIB_UUID           = "uuid";
const char* const ATTRIB_SORT_ID        = "sortId";
const char* const ATTRIB_NAME           = "name";
const char* const ATTRIB_TYPE           = "type";
const char* const ATTRIB_UIFILE         = "uifilename";
const char* const ATTRIB_COMPLETION     = "completion";
const char* const ATTRIB_ICON           = "icon";
const char* const ATTRIB_OPTIONAL      = "optional";
const char* const ATTRIB_LANGUAGE       = "lang";
const char* const ATTRIB_FILE           = "file";
const char* const ATTRIB_ISIDENTITYFORM = "isidentity";
const char* const ATTRIB_PATIENTREPRESENTATION  = "patient";
const char* const FILETYPE_SCRIPT       = "script";
const char* const FILETYPE_FORM         = "form";
const char* const FILETYPE_PRINTHTMLMASK       = "htmlprintmask";
const char* const FILETYPE_PRINTPLAINTEXTMASK  = "plaintextprintmask";
const char* const FILETYPE_EXPORTHTMLMASK       = "htmlexportmask";
const char* const FILETYPE_EXPORTPLAINTEXTMASK  = "plaintextexportmask";

const char* const OPTION_PLUGIN_NAME  = "type";
const char* const OPTION_USEFORMNAMEASNS  = "useformnameasnamespace";

enum creationTagsEnum {
    CreateForm = 0,
    CreatePage,
    CreateItem
};

static const QStringList createTags =
        QStringList() << TAG_NEW_FORM << TAG_NEW_PAGE << TAG_NEW_ITEM;

enum uiTagsEnum {
    Option
};
static const QStringList uiTags =
        QStringList() << "option";

}  // End Constants
}  // End XmlForms

#endif // XMLIO_CONSTANTS_H
