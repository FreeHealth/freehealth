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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORMMANAGER_SETTINGS_CONSTANTS_H
#define FORMMANAGER_SETTINGS_CONSTANTS_H

namespace Form {
namespace Constants {

    // Settings
    const char * const  S_EPISODEMODEL_FORM_FOREGROUND    = "EpisodeModel/FormForeGround";
    const char * const  S_EPISODEMODEL_EPISODE_FOREGROUND = "EpisodeModel/EpisodeForeGround";
    const char * const  S_EPISODEMODEL_FORM_FONT    = "EpisodeModel/Font/Form";
    const char * const  S_EPISODEMODEL_EPISODE_FONT = "EpisodeModel/Font/Episode";

    const char * const  S_EPISODEMODEL_LONGDATEFORMAT         = "EpisodeModel/LongDateFormat";
    const char * const  S_EPISODEMODEL_SHORTDATEFORMAT         = "EpisodeModel/ShortDateFormat";
    const char * const  S_PLACEHOLDERSPLITTER_SIZES       = "Splitters/PlaceHolderSizes";

    const char * const S_USEALTERNATEROWCOLOR = "EpisodeModel/useAltRowCol";
    const char * const S_USESPECIFICCOLORFORROOTS= "EpisodeModel/useSpeColorForRoot";
    const char * const S_EPISODELABELCONTENT = "EpisodeModel/episodeLabel";
    const char * const S_FOREGROUNDCOLORFORROOTS = "EpisodeModel/foregroundRoot";

    const char * const S_EPISODEVIEW_SORTEDCOLUMN = "EpisodeModel/sortedColumn";
    const char * const S_EPISODEVIEW_SORTORDER = "EpisodeModel/sortOrder";

    const char * const XML_FORM_GENERAL_TAG = "FormXmlContent";

    // Tokens
    const char * const T_LABEL = "LABEL";
    const char * const T_SMALLDATE = "SMALL_DATE";
    const char * const T_FULLDATE = "FULL_DATE";

    // Style sheets
    const char * const FORMTREEVIEW_SHEET =
            " QTreeView {"
            "    show-decoration-selected: 1;"
            "}"

            "QTreeView::item {"
    //        "    border: 0px;"
            "    background: base;"
    //        "    border-top-color: transparent;"
    //        "    border-bottom-color: transparent;"
            "}"

            "QTreeView::item:hover {"
            "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
    //        "    border: 0px solid #bfcde4;"
            "}"

    //        "QTreeView::branch:hover {"
    //        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
    //        "    border: 0px solid #bfcde4;"
    //        "}"

    //        "QTreeView::item:selected {"
    //        "    border: 0px solid #567dbc;"
    //        "}"

            "QTreeView::item:selected {"
            "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
            "}"

            "QTreeView::branch:selected {"
            "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
            "}"

    //        "QTreeView::item:selected:!active {"
    //        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
    //        "}"
            ;


}  // End namespace Form::Constants
}  // End namespace Form

#endif // FORMMANAGER_SETTINGS_CONSTANTS_H
