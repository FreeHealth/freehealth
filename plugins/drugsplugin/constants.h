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
#ifndef DRUGSCONSTANTS_H
#define DRUGSCONSTANTS_H

/**
 \brief Some constants for Drugs management
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

    const char* const   A_CLEAR_PRESCRIPTION     = "a.Drugs.ClearPrescription";
    const char* const   A_SAVE_PRESCRIPTION     = "a.Drugs.SavePrescription";
    const char* const   A_SAVEAS_PRESCRIPTION   = "a.Drugs.SaveAsPrescription";
    const char* const   A_TOGGLE_DRUGSELECTOR   = "a.Drugs.ToggleDrugSelector";
    const char* const   A_VIEW_INTERACTIONS     = "a.Drugs.ViewInteractions";
    const char* const   A_CHANGE_DURATION       = "a.Drugs.ChangeDuration";
    const char* const   A_SEARCH_COMMERCIAL     = "a.Drugs.SearchCom";
    const char* const   A_SEARCH_MOLECULES      = "a.Drugs.SearchMol";
    const char* const   A_SEARCH_INN            = "a.Drugs.SearchINN";
    const char* const   A_PRINT_PRESCRIPTION    = "a.Drugs.PrintPrescr";
    const char* const   A_TOGGLE_TESTINGDRUGS   = "a.Drugs.ToggleTestings";
    const char* const   A_DRUGSDATABASE_INFORMATION = "a.Drugs.DrugsDbInfo";
    const char* const   A_DOSAGESDATABASE_INFORMATION = "a.Drugs.ProtocolDbInfo";
    const char* const   A_IAMDATABASE_INFORMATION = "a.Drugs.IAMDbInfo";
    const char* const   A_PRESCRIBERMODE        = "a.Drugs.PrescriberMode";
    const char* const   A_SELECTONLYMODE        = "a.Drugs.SelectOnlyMode";
    const char* const   A_OPENDOSAGEDIALOG      = "a.Drugs.OpenDosage";
    const char* const   A_OPENDOSAGEPREFERENCES = "a.Drugs.OpenDosagePreferences";
    const char* const   A_RESETPRESCRIPTIONSENTENCE_TODEFAULT = "a.Drugs.ResetPrescriptionSentenceToDefault";
    const char* const   A_TOGGLEDRUGPRECAUTIONS = "a.Drugs.TogglePrecautions";
    const char* const   A_COPYPRESCRIPTIONITEM = "a.Drugs.CopyPrescriptionItem";


    const char* const   DRUGSMENU_TEXT          = QT_TRANSLATE_NOOP("mfDrugsConstants", "Prescription");
    const char* const   CLEARPRESCRIPTION_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Clear prescription");
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
    const char* const   TOGGLEDRUGPRECAUTIONS_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Toggle drug precautions");
    const char* const   DRUGPRECAUTIONS_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Drug precautions");
    const char* const   COPYPRESCRIPTIONITEM_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Copy prescription item to clipboard");
    const char* const   ADDLONGTERMTHERAPEUTICS_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Add long-term medications");

    const char * const  I_TOGGLETESTINGDRUGS       =  "pill_hidden.png";       /*!< \brief Themed icon name for Commercial search. */
    const char * const  I_TOGGLEDRUGSELECTOR       =  "drugselector.png";       /*!< \brief Themed icon name for Commercial search. */
    const char * const  I_SHOWDRUGPRECAUTIONS      =  "drugprecautions.png";       /*!< \brief Themed icon name for Commercial search. */

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

    const char * const  S_USER                   = "DrugsWidget/user/Name";                    /*!< \brief Key for settings. */
    const char * const  S_PASSWORD               = "DrugsWidget/user/Pass";                    /*!< \brief Key for settings. */
    const char * const  S_SERIAL                 = "DrugsWidget/user/SN";                      /*!< \brief Key for settings. */

    const char * const  S_PATIENTNAMESORDER      = "DrugsWidget/PatientNamesOrder";                      /*!< \brief Key for settings. */

    const char * const  S_SEARCHMETHOD           = "DrugsWidget/searchMethod";                 /*!< \brief Key for settings. */
    const char * const  S_HISTORYSIZE            = "DrugsWidget/historySize";                  /*!< \brief Key for settings. */
    const char * const  S_DRUGHISTORY            = "DrugsWidget/drugsHistory";                 /*!< \brief Key for settings. */
    const char * const  S_TOOLBARICONSIZE        = "DrugsWidget/toolbarIconSize";

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

    // Help files
    const char * const H_PROTOCOL_DIALOG = "prescribe.html";
    const char * const H_PREFERENCES_VIEW_FR = "preferences.html#parametres_du_prescripteur";
    const char * const H_PREFERENCES_VIEW_EN = "preferences.html";
    const char * const H_PREFERENCES_SELECTOR_FR = "preferences.html";
    const char * const H_PREFERENCES_SELECTOR_EN = "preferences.html";
    const char * const H_PREFERENCES_PRINT_FR = "preferences.html#le_masque_de_creation_de_la_phrase_posologie";
    const char * const H_PREFERENCES_PRINT_EN = "preferences.html";
    const char * const H_PREFERENCES_USER_FR = "preferences.html#parametrage_des_entete_pied_de_page_et_tatouage_d_impression";
    const char * const H_PREFERENCES_USER_EN = "preferences.html";
    const char * const H_PREFERENCES_EXTRA_FR = "preferences.html#texte_pour_les_medicaments_rembourses_a_100_et_autres_extras";
    const char * const H_PREFERENCES_EXTRA_EN = "preferences.html";
    const char * const H_PREFERENCES_DBSELECTOR_FR = "multidrugsdatabase.html";
    const char * const H_PREFERENCES_DBSELECTOR_EN = "multidrugsdatabase.html";
    const char * const H_PREFERENCES_PROTOCOL_FR = "preferences.html#parametres_pour_les_protocoles";
    const char * const H_PREFERENCES_PROTOCOL_EN = "preferences.html";



}  // End namespace DrugsWidget::Constants
}  // End namespace DrugsWidget

#endif // DRUGSCONSTANTS_H
