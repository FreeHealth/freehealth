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

namespace XmlForms {
namespace Constants {

    /** \todo Add an attribute when creating Forms/Page --> don't present in MainWindow::centralWidget's TreeWidget */
    /** \todo Add an attribute when creating Scripts --> load script from file 'fileName/#anchor' so that file can contain a multitude of scripts marked with an anchor. */
    /** \todo Manage multilingual specs. */

const char* const DOCTYPE_NAME     = "freemedforms";
const char* const DOCTYPE_EXTENSION= "xml";

const char* const TAG_NEW_FORM     = "MedForm";
const char* const TAG_NEW_PAGE     = "Page";
const char* const TAG_NEW_ITEM     = "Item";
const char* const TAG_ADDFILE      = "file";
const char* const TAG_NAME         = "name";

const char* const TAG_SPEC_AUTHORS      = "authors";
const char* const TAG_SPEC_CATEGORY     = "category";
const char* const TAG_SPEC_LICENSE      = "licence";
const char* const TAG_SPEC_CREATIONDATE = "cdate";
const char* const TAG_SPEC_BIBLIOGRAPHY = "bibliography";
const char* const TAG_SPEC_DESCRIPTION  = "description";
const char* const TAG_SPEC_LABEL        = "label";
const char* const TAG_SPEC_PLUGINNAME   = "type";
const char* const TAG_SPEC_VERSION      = "version";

const char* const TAG_VALUE              = "value";
const char* const TAG_VALUE_NUMERICAL    = "numerical";
const char* const TAG_VALUE_POSSIBLE     = "possible";
const char* const TAG_VALUE_SCRIPT       = "script";
const char* const TAG_VALUE_DEPENDENCIES = "dependon";
const char* const TAG_VALUE_DEFAULT      = "default";

const char* const TAG_SCRIPT                    = "script";
const char* const TAG_SCRIPT_ONLOAD             = "onload";
const char* const TAG_SCRIPT_POSTLOAD           = "postload";
const char* const TAG_SCRIPT_ONDEMAND           = "ondemand";
const char* const TAG_SCRIPT_ONVALUECHANGED     = "onvaluechanged";
const char* const TAG_SCRIPT_ONVALUEREQUIERED   = "onvaluerequiered";
const char* const TAG_SCRIPT_ONDEPENDENCIESCHANGED = "ondependencieschanged";

const char* const ATTRIB_ID           = "id";
const char* const ATTRIB_COMPLETION   = "completion";
const char* const ATTRIB_OPTIONNAL    = "optional";
const char* const ATTRIB_LANGUAGE     = "lang";

const char* const OPTION_PLUGIN_NAME  = "type";

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

} // End Constants
} // End XmlForms
