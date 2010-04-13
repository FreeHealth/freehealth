/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TEMPLATES_CONSTANTS_H
#define TEMPLATES_CONSTANTS_H

#include <drugsbaseplugin/drugsbase_exporter.h>

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.0
 * \date 30 Nov 2009
*/

namespace Templates {
namespace Constants {
    const char * const DB_TEMPLATES_NAME      = "templates";
    const char * const DB_TEMPLATES_FILENAME  = "templates.db";
    const char * const DB_ACTUAL_VERSION      = "0.3.0";


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
