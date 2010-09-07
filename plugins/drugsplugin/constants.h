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
    const char * const  M_PLUGINS_DRUGS         =  "DrugsPlugins.mDrugs";
    const char * const  M_PLUGINS_SEARCH        =  "DrugsPlugins.mSearch";
    const char * const  M_PLUGINS_MODES         =  "DrugsPlugins.mModes";

    const char * const  G_PLUGINS_VIEWS         =  "DrugsPlugins.grViews";
    const char * const  G_PLUGINS_DRUGS         =  "DrugsPlugins.grDrugs";
    const char * const  G_PLUGINS_SEARCH        =  "DrugsPlugins.grSearch";
    const char * const  G_PLUGINS_MODES         =  "DrugsPlugins.grModes";
    const char * const  G_PLUGINS_INTERACTIONS  =  "DrugsPlugins.grInteractions";

    const char * const  C_DRUGS_PLUGINS         =  "DrugsPlugins.cDrugs";
    const char * const  C_DRUGS_SEARCH          =  "DrugsPlugins.cSearch";

    const char* const   A_TOGGLE_DRUGSELECTOR   = "a.Drugs.ToggleDrugSelector";
    const char* const   A_VIEW_INTERACTIONS     = "a.Drugs.ViewInteractions";
    const char* const   A_CHANGE_DURATION       = "a.Drugs.ChangeDuration";
    const char* const   A_SEARCH_COMMERCIAL     = "a.Drugs.SearchCom";
    const char* const   A_SEARCH_MOLECULES      = "a.Drugs.SearchMol";
    const char* const   A_SEARCH_INN            = "a.Drugs.SearchINN";
    const char* const   A_PRINT_PRESCRIPTION    = "a.Drugs.PrintPrescr";
    const char* const   A_TOGGLE_TESTINGDRUGS   = "a.Drugs.ToggleTestings";
    const char* const   A_DRUGSDATABASE_INFORMATIONS = "a.Drugs.DrugsDbInfos";
    const char* const   A_DOSAGESDATABASE_INFORMATIONS = "a.Drugs.ProtocolDbInfos";
    const char* const   A_IAMDATABASE_INFORMATIONS = "a.Drugs.IAMDbInfos";
    const char* const   A_PRESCRIBERMODE        = "a.Drugs.PrescriberMode";
    const char* const   A_SELECTONLYMODE        = "a.Drugs.SelectOnlyMode";
    const char* const   A_OPENDOSAGEDIALOG      = "a.Drugs.OpenDosage";
    const char* const   A_OPENDOSAGEPREFERENCES = "a.Drugs.OpenDosagePreferences";
    const char* const   A_RESETPRESCRIPTIONSENTENCE_TODEFAULT = "a.Drugs.ResetPrescriptionSentenceToDefault";


    const char* const   DRUGSMENU_TEXT          = QT_TRANSLATE_NOOP("mfDrugsConstants", "Prescription");
    const char* const   TOGGLEDRUGSELECTOR_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Toggle drug selector");
    const char* const   SEARCHCOMMERCIAL_TEXT   = QT_TRANSLATE_NOOP("mfDrugsConstants", "Search by commercial name");
    const char* const   SEARCHINN_TEXT          = QT_TRANSLATE_NOOP("mfDrugsConstants", "Search by INN");
    const char* const   SEARCHMOLECULES_TEXT    = QT_TRANSLATE_NOOP("mfDrugsConstants", "Search by molecules");
    const char* const   MODEMENU_TEXT           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Edition modes");
    const char* const   PRESCRIBERMODE_TEXT     = QT_TRANSLATE_NOOP("mfDrugsConstants", "Full prescriber");
    const char* const   SELECTONLYMODE_TEXT     = QT_TRANSLATE_NOOP("mfDrugsConstants", "Select only");
    const char* const   PRINTPRESCRIPTION_TEXT  = QT_TRANSLATE_NOOP("mfDrugsConstants", "Print prescription");
    const char* const   TOGGLETESTINGDRUGS_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Toggle testing drugs");
    const char* const   OPENDOSAGEDIALOG_TEXT      = QT_TRANSLATE_NOOP("mfDrugsConstants", "Open protocol dialog");
    const char* const   OPENDOSAGEPREFERENCES_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Open posologic sentence preferences");
    const char* const   RESETPRESCRIPTIONSENTENCETODEFAULT_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Reset the prescription sentence to the default one");

    const char * const  I_TOGGLETESTINGDRUGS       =  "pill_hidden.png";       /*!< \brief Themed icon name for Commercial search. */
    const char * const  I_TOGGLEDRUGSELECTOR       =  "drugselector.png";       /*!< \brief Themed icon name for Commercial search. */

    // Search Method
    enum SearchMethod {
        SearchCommercial = 0,
        SearchMolecules,
        SearchInn
    };



    // Settings names
    const char * const  S_GROUP                  = "DrugsWidget";                              /*!< \brief Key for settings. */
    const char * const  S_CONFIGURED             = "DrugsWidget/Configured";                   /*!< \brief Key for settings. */
    const char * const  S_VIEWFONT               = "DrugsWidget/view/Font";                    /*!< \brief Key for settings. */
    const char * const  S_VIEWFONTSIZE           = "DrugsWidget/view/FontSize";                /*!< \brief Key for settings. */
    const char * const  S_INTERACTIONVIEW_ZOOM   = "DrugsWidget/view/InteractionViewer/Zoom";  /*!< \brief Key for settings. */
    const char * const  S_DRUGFONT               = "DrugsWidget/print/drug/Font";              /*!< \brief Key for settings. */
    const char * const  S_PRESCRIPTIONFONT       = "DrugsWidget/print/prescription/Font";      /*!< \brief Key for settings. */

    const char * const  S_USER                   = "DrugsWidget/user/Name";                    /*!< \brief Key for settings. */
    const char * const  S_PASSWORD               = "DrugsWidget/user/Pass";                    /*!< \brief Key for settings. */
    const char * const  S_SERIAL                 = "DrugsWidget/user/SN";                      /*!< \brief Key for settings. */

    const char * const  S_SEARCHMETHOD           = "DrugsWidget/searchMethod";                 /*!< \brief Key for settings. */
    const char * const  S_HISTORYSIZE            = "DrugsWidget/historySize";                  /*!< \brief Key for settings. */
    const char * const  S_DRUGHISTORY            = "DrugsWidget/drugsHistory";                 /*!< \brief Key for settings. */

    const char * const  S_DYNAMICALERTS          = "DrugsWidget/dynamicAlerts";
    const char * const  S_DYNAMICALERTS_LEVEL    = "DrugsWidget/dynamicAlertsMinimalLevel";

    const char * const  S_USERRECORDEDFORMS      = "DrugsWidget/userRecordedForms";            /*!< \brief Key for settings. */
    const char * const  S_PROTOCOLCREATOR_DEFAULTBUTTON = "DrugsWidget/protocolCreator/sefautButton";            /*!< \brief Key for settings. */
    const char * const  S_PROTOCOLCREATOR_AUTOCHANGE = "DrugsWidget/protocolCreator/autoChange";            /*!< \brief Key for settings. */
    const char * const  S_PROTOCOLCREATOR_AUTOCHANGE_BUTTON = "DrugsWidget/protocolCreator/autoChangeButton";            /*!< \brief Key for settings. */

    const char * const  S_VALUE_PRESCRIBEONLY      = "PrescribeOnly";            /*!< \brief Key for settings. */
    const char * const  S_VALUE_SAVEONLY           = "SaveOnly";            /*!< \brief Key for settings. */
    const char * const  S_VALUE_TESTONLY           = "TestOnly";            /*!< \brief Key for settings. */
    const char * const  S_VALUE_SAVEPRESCRIBE      = "SavePrescribe";            /*!< \brief Key for settings. */

    const char * const  S_STATEPREFIX            = "DrugsWidget/";

    const char * const  USERRECORDEDFORMS_COLOR  = "#FFE4E0";


}  // End namespace DrugsWidget::Constants
}  // End namespace DrugsWidget

#endif // DRUGSCONSTANTS_H
