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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TEMPLATES_CONSTANTS_H
#define TEMPLATES_CONSTANTS_H

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 29 Feb 2012
*/

namespace Templates {
namespace Constants {
    const char * const DB_TEMPLATES_NAME      = "templates";
    const char * const DB_TEMPLATES_FILENAME  = "templates.db";
    const char * const DB_ACTUAL_VERSION      = "0.4.0";


    const char * const  S_BACKGROUND_CATEGORIES    = "Templates/Background/Categories";
    const char * const  S_BACKGROUND_TEMPLATES     = "Templates/Background/Templates";
    const char * const  S_FOREGROUND_CATEGORIES    = "Templates/Foreground/Categories";
    const char * const  S_FOREGROUND_TEMPLATES     = "Templates/Foreground/Templates";

    const char * const  S_SPLITTER_SIZES           = "Templates/Splitter/Sizes";

    const char * const  S_ALWAYSSHOWEXPANDED       = "Templates/AlwaysShowExpanded";
    const char * const  S_LOCKCATEGORYVIEW         = "Templates/LockCategoryView";
    const char * const  S_SAVEDATAWITHOUTPROMPTING = "Templates/SaveWithoutPrompting";
    const char * const  S_PROMPTFORDELETION        = "Templates/PromptForDeletion";

    const char * const  S_FONT                     = "Templates/Font";

    const char * const MIMETYPE_TEMPLATE           = "application/template";
    const char * const MIMETYPE_CATEGORY           = "application/template.category";

    const char * const XML_TEMPLATE_TAG            = "Template";
    const char * const XML_TEMPLATE_CONTENT_ATTRIB = "content";
    const char * const XML_TEMPLATE_MIME_ATTRIB    = "mime";
    const char * const XML_TEMPLATE_CHILD_TAG      = "Child";

    enum Tables {
        Table_Templates = 0,
        Table_Categories,
        Table_Version
    };

    enum TemplatesFields {
        TEMPLATE_ID = 0,
        TEMPLATE_UUID,
        TEMPLATE_USER_UID,
        TEMPLATE_GROUP_UID,
        TEMPLATE_ID_CATEGORY,
        TEMPLATE_LABEL,
        TEMPLATE_SUMMARY,
        TEMPLATE_CONTENT,
        TEMPLATE_CONTENTMIMETYPES,
        TEMPLATE_DATECREATION,
        TEMPLATE_DATEMODIF,
        TEMPLATE_THEMEDICON,
        TEMPLATE_TRANSMISSIONDATE,
        TEMPLATE_MaxParam
    };

    enum CategoryFields {
        CATEGORIES_ID = 0,
        CATEGORIES_UUID,
        CATEGORIES_USER_UID,
        CATEGORIES_GROUP_UID,
        CATEGORIES_PARENT_ID,
        CATEGORIES_LABEL,
        CATEGORIES_SUMMARY,
        CATEGORIES_MIMETYPES,
        CATEGORIES_DATECREATION,
        CATEGORIES_DATEMODIF,
        CATEGORIES_THEMEDICON,
        CATEGORIES_TRANSMISSIONDATE,
        CATEGROIES_MaxParam
    };

    enum VersionFields {
        VERSION_ACTUAL = 0
    };

    enum ModelDataRepresentation {
        Data_Label = 0,
        Data_Id,
        Data_Uuid,
        Data_UserUuid,
        Data_ParentId,
        Data_Summary,
        Data_ContentMimeTypes,
        Data_Content,
        Data_ThemedIcon,
        Data_CreationDate,
        Data_ModifDate,
        Data_TransmissionDate,
        Data_IsTemplate,
        Data_IsNewlyCreated,
        Data_Max_Param
    };


}  // end namespace Constants
}  // end namespace Templates


#endif // TEMPLATES_CONSTANTS_H
