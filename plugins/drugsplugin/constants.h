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
#ifndef DRUGSCONSTANTS_H
#define DRUGSCONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 \brief Some constants for Drugs management
 \ingroup freediams drugswidget
*/
namespace DrugsWidget {
namespace Constants {

    // Menu and action for ActionManager
    const char * const  DRUGCONSTANTS_TR_CONTEXT = "mfDrugsConstants";
    const char * const  M_PLUGINS_DRUGS         =  "DrugsPlugins.menuDrugs";
    const char * const  M_PLUGINS_SEARCH        =  "DrugsPlugins.menuSearch";
    const char * const  G_PLUGINS_DRUGS         =  "DrugsPlugins.groupDrugs";
    const char * const  G_PLUGINS_SEARCH        =  "DrugsPlugins.groupSearch";
    const char * const  C_DRUGS_PLUGINS         =  "DrugsPlugins.ContextDrugs";
    const char * const  C_DRUGS_SEARCH          =  "DrugsPlugins.ContextSearch";
    const char* const   A_SEARCH_COMMERCIAL     = "a.DrugsPlug.SearchCom";
    const char* const   A_SEARCH_MOLECULES      = "a.DrugsPlug.SearchMol";
    const char* const   A_SEARCH_INN            = "a.DrugsPlug.SearchINN";
    const char* const   A_PRINT_PRESCRIPTION    = "a.DrugsPlug.PrintPrescr";
    const char* const   A_TOOGLE_TESTINGDRUGS   = "a.DrugsPlug.ToogleTestings";
    const char* const   DRUGSMENU_TEXT          = QT_TRANSLATE_NOOP("mfDrugsConstants", "Prescription");
    const char* const   SEARCHMENU_TEXT         = QT_TRANSLATE_NOOP("mfDrugsConstants", "Search method");
    const char* const   SEARCHCOMMERCIAL_TEXT   = QT_TRANSLATE_NOOP("mfDrugsConstants", "Search by commercial name");
    const char* const   SEARCHINN_TEXT          = QT_TRANSLATE_NOOP("mfDrugsConstants", "Search by INN");
    const char* const   SEARCHMOLECULES_TEXT    = QT_TRANSLATE_NOOP("mfDrugsConstants", "Search by molecules");
    const char* const   PRINTPRESCRIPTION_TEXT  = QT_TRANSLATE_NOOP("mfDrugsConstants", "Print prescription");
    const char* const   TOOGLETESTINGDRUGS_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Toogle testing drugs");

    const char * const  I_TOOGLETESTINGDRUGS       =  "pill_hidden.png";       /*!< \brief Themed icon name for Commercial search. */

    // Search Method
    enum SearchMethod {
        SearchCommercial = 0,
        SearchMolecules,
        SearchInn
    };



    // Settings names
    const char * const  MFDRUGS_SETTING_GROUP                  = "DrugsWidget";                              /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_CONFIGURED             = "DrugsWidget/Configured";                   /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_VIEWFONT               = "DrugsWidget/view/Font";                    /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_VIEWFONTSIZE           = "DrugsWidget/view/FontSize";                /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_INTERACTIONVIEW_ZOOM   = "DrugsWidget/view/InteractionViewer/Zoom";  /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_DRUGFONT               = "DrugsWidget/print/drug/Font";              /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_PRESCRIPTIONFONT       = "DrugsWidget/print/prescription/Font";      /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_WATERMARKALIGNEMENT    = "DrugsWidget/print/watermark/Alignment";    /*!< \brief Key for settings. \obsolete*/
    const char * const  MFDRUGS_SETTING_WATERMARKPRESENCE      = "DrugsWidget/print/watermark/Presence";     /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_WATERMARK_HTML         = "DrugsWidget/print/watermark/Html";         /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_USERHEADER             = "DrugsWidget/user/Header";                  /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_USERFOOTER             = "DrugsWidget/user/Footer";                  /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_USER                   = "DrugsWidget/user/Name";                    /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_PASSWORD               = "DrugsWidget/user/Pass";                    /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_SERIAL                 = "DrugsWidget/user/SN";                      /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_SEARCHMETHOD           = "DrugsWidget/searchMethod";                 /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_HISTORYSIZE            = "DrugsWidget/historySize";                  /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_DRUGHISTORY            = "DrugsWidget/drugsHistory";                 /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_USERRECORDEDFORMS      = "DrugsWidget/userRecordedForms";            /*!< \brief Key for settings. */

    // DEFAULTS SETTINGS
    const char * const  MFDRUGS_DEFAULT_WATEMARKHTML           = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body>\n<p align=\"center\"  style=\" font-family:'Lucida Grande'; font-size:12pt; font-weight:bold; font-style:normal; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#9c9c9c;\">Duplicata ne permettant pas la d&eacute;livrance des m&eacute;dicaments</span></p>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#9c9c9c;\">A adresser &agrave; votre centre de S&eacute;curit&eacute; Sociale.</span></p></body></html>";              /*!< \brief Default value for settings. */
    const char * const  MFDRUGS_DEFAULT_USERHEADER             = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body>\n"
                                                                 "<table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>\n"
                                                                 "<td width=\"33%\">\n<p align=\"center\"><span style=\" font-weight:600;\">Dr TRUCMUCHE Bidulle</span>\n<br /><span style=\" font-weight:600;\">M&eacute;decine Machin</span><\n<br /><br /><span style=\" font-size:10pt; font-style:italic;\">Ancien Assistant des h&ocirc;pitaux</span>\n<br /><span style=\" font-size:10pt; font-style:italic;\">Tous mes dipl&ocirc;mes</span>\n<br />----------<br />\n<span style=\" font-size:10pt;\">Mes Consultations sur rendez-vous</span>\n<br />----------\n<br /><span style=\" font-size:10pt; font-style:italic;\">Mon ADELI</span></p></td>\n<td width=\"33%\"></td>\n"
                                                                 "<td width=\"33%\">\n<p align=\"center\"><span style=\" font-size:9pt;\">2, rue de la M&eacute;decine\n<br />62 223 SAINT-LAURENT-BLANGY<br />docteur@trucmuche.fr<br />T&eacute;l : 03.21.00.00.00<br />Fax : 03.21.00.00.06<br />----------<br /></span>"
                                                                 "<span style=\" font-size:12pt;font-weight:bold;\">[[PATIENTNAME]<br />][Le [DATE]]</span></p></td></tr></table></body></html>";              /*!< \brief Default value for settings. */
//    const char * const  MFDRUGS_DEFAULT_USERFOOTER             = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Lucida Grande'; font-size:13pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">En cas d'urgence applez le SAMU : 112</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Veuillez signalez les demandes de visites &agrave; domicile, tant que possible, avant 10h.</span></p></body></html>";


    const char * const  MFDRUGS_SETTINGS_STATEPREFIX =   "DrugsWidget/";


}  // End namespace DrugsWidget::Constants
}  // End namespace DrugsWidget

#endif // DRUGSCONSTANTS_H
