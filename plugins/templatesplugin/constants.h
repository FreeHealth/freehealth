/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
    const char * const TEMPLATES_DATABASE_NAME     = "templates";
    const char * const TEMPLATES_DATABASE_FILENAME = "templates.db";

    const char * const  S_BACKGROUND_CATEGORIES    = "Templates/Background/Categories";
    const char * const  S_BACKGROUND_TEMPLATES     = "Templates/Background/Templates";
    const char * const  S_FOREGROUND_CATEGORIES    = "Templates/Foreground/Categories";
    const char * const  S_FOREGROUND_TEMPLATES     = "Templates/Foreground/Templates";

    const char * const  S_SPLITTER_SIZES           = "Templates/Splitter/Sizes";

    const char * const  S_ALWAYSSHOWEXPANDED       = "Templates/AlwaysShowExpanded";
    const char * const  S_LOCKCATEGORYVIEW         = "Templates/LockCategoryView";
    const char * const  S_SAVEDATAWITHOUTPROMPTING = "Templates/SaveWithoutPrompting";
    const char * const  S_PROMPTFORDELETION        = "Templates/PromptForDeletion";

    const char * const MIMETYPE_TEMPLATE           = "application/template";
    const char * const MIMETYPE_CATEGORY           = "application/template.category";

    const char * const XML_TEMPLATE_TAG            = "Template";
    const char * const XML_TEMPLATE_CONTENT_ATTRIB = "content";
    const char * const XML_TEMPLATE_MIME_ATTRIB    = "mime";
    const char * const XML_TEMPLATE_CHILD_TAG      = "Child";


    enum TemplatesFields {
        TF_Id = 0,
        TF_OwnerUuid,
        TF_GroupUuid,
        TF_Type,
        TF_CategoryId,
        TF_Label,
        TF_Summary,
        TF_Contents
    };

    enum CategoryFields {
        CF_Id = 0,
        CF_OwnerUuid,
        CF_GroupUuid,
        CF_Type,
        CF_ParentId,
        CF_Label,
        CF_Summary
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
