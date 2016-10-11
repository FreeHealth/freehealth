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
#ifndef DRUGSBASE_CONSTANTS_H
#define DRUGSBASE_CONSTANTS_H

#include <drugsbaseplugin/constants_databaseschema.h>

#include <QHash>
#include <QStringList>

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.6
 * \date 12 May 2013
*/

namespace DrugsDB {

    /** \brief Some constants for Drugs management */

namespace Constants {

    // SETTINGS
    const char * const S_USERRECORDEDFORMS       = "DrugsWidget/userRecordedForms";
    const char * const S_SHOWICONSINPRESCRIPTION = "DrugsWidget/view/ShowIconsInPrescription";      /*!< \brief Key for settings. */
    const char * const S_HIDELABORATORY          = "DrugsWidget/print/drug/hideLaboratory";    /*!< \brief Key for settings. */

    const char * const S_LEVELOFWARNING_STATICALERT = "DrugsWidget/levelOfWarning";               /*!< \brief Key for settings. */
    const char * const S_USEDYNAMICALERTS           = "DrugsWidget/dynamicAlerts";
    const char * const S_LEVELOFWARNING_DYNAMICALERT= "DrugsWidget/dynamicAlertsMinimalLevel";


    const char * const  S_USERHEADER             = "DrugsWidget/user/Header";                  /*!< \brief Key for settings. */
    const char * const  S_USERFOOTER             = "DrugsWidget/user/Footer";                  /*!< \brief Key for settings. */

    const char * const  S_WATERMARKALIGNEMENT    = "DrugsWidget/print/watermark/Alignment";    /*!< \brief Key for settings. \obsolete*/
    const char * const  S_WATERMARKPRESENCE      = "DrugsWidget/print/watermark/Presence";     /*!< \brief Key for settings. */
    const char * const  S_WATERMARK_HTML         = "DrugsWidget/print/watermark/Html";         /*!< \brief Key for settings. */

    const char * const  S_PRINTDUPLICATAS        = "DrugsWidget/PrintDuplicatas";
    const char * const  S_AUTOSORT_ON_PRINTING   = "DrugsWidget/PrintAutoSort";
    const char * const  S_ADD_BIOMETRICS_ON_PRINTS = "DrugsWidget/PrintAddBiometrics";

    // DEFAULTS SETTINGS
    const char * const  S_DEF_WATEMARKHTML           = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body>\n<p align=\"center\"  style=\" font-family:'Lucida Grande'; font-size:12pt; font-weight:bold; font-style:normal; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#9c9c9c;\">Duplicata ne permettant pas la d&eacute;livrance des m&eacute;dicaments</span></p>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#9c9c9c;\">A adresser &agrave; votre centre de S&eacute;curit&eacute; Sociale.</span></p></body></html>";              /*!< \brief Default value for settings. */

    // OBSOLETE
    const char * const  S_DEF_USERHEADER             = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body>\n"
                                                                 "<table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>\n"
                                                                 "<td width=\"33%\">\n<p align=\"center\"><span style=\" font-weight:600;\">Dr TRUCMUCHE Bidulle</span>\n<br /><span style=\" font-weight:600;\">M&eacute;decine Machin</span><\n<br /><br /><span style=\" font-size:10pt; font-style:italic;\">Ancien Assistant des h&ocirc;pitaux</span>\n<br /><span style=\" font-size:10pt; font-style:italic;\">Tous mes dipl&ocirc;mes</span>\n<br />----------<br />\n<span style=\" font-size:10pt;\">Mes Consultations sur rendez-vous</span>\n<br />----------\n<br /><span style=\" font-size:10pt; font-style:italic;\">Mon ADELI</span></p></td>\n<td width=\"33%\"></td>\n"
                                                                 "<td width=\"33%\">\n<p align=\"center\"><span style=\" font-size:9pt;\">2, rue de la M&eacute;decine\n<br />62 223 SAINT-LAURENT-BLANGY<br />docteur@trucmuche.fr<br />T&eacute;l: 03.21.00.00.00<br />Fax: 03.21.00.00.06<br />----------<br /></span>"
                                                                 "<span style=\" font-size:12pt;font-weight:bold;\">[[PATIENTTITLE] ][[PATIENTFULLNAME]<br />][Le [DATE]]</span></p></td></tr></table></body></html>";              /*!< \brief Default value for settings. */
//    const char * const  MFDRUGS_DEFAULT_USERFOOTER             = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Lucida Grande'; font-size:13pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">En cas d'urgence applez le SAMU: 112</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Veuillez signalez les demandes de visites &agrave; domicile, tant que possible, avant 10h.</span></p></body></html>";


    const char * const S_PRESCRIPTIONFORMATTING_HTML      = "DrugsWidget/print/prescription/HtmlFormatting";      /*!< \brief Key for settings. */
    const char * const S_PRESCRIPTIONFORMATTING_PLAIN     = "DrugsWidget/print/prescription/PlainFormatting";      /*!< \brief Key for settings. */

#ifdef WITH_PAD
    const char * const S_DEF_PRESCRIPTION_TOKENFILE_1_LANG = "/textfiles/prescription/padtoolsstyle_%1.html";
#else
    const char * const S_DEF_PRESCRIPTION_TOKENFILE_1_LANG = "/textfiles/prescription/oldtokenstyle_%1.html";
#endif

    // OBSOLETE
    const char * const S_DEF_PRESCRIPTIONFORMATTING       = QT_TRANSLATE_NOOP("mfDrugsConstants",
                                                                              "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">"
                                                                              "[[DRUG]]</span><span style=\"font-style:italic;\">[<br />"
                                                                              "Take [Q_FROM]][ to [Q_TO]] [[Q_SCHEME]][ [REPEATED_DAILY_SCHEME]][ [MEAL]][ each [[PERIOD] ][PERIOD_SCHEME]][ for [D_FROM]][ to [D_TO]][ [D_SCHEME]][<br />"
                                                                              "Route: [ROUTE]][<br />"
                                                                              "Daily Distribution: [DISTRIBUTED_DAILY_SCHEME]][<br />"
                                                                              "Minimum interval between intakes: [MIN_INTERVAL]][<br />"
                                                                              "[NOTE] ]</span></p>");
    // OBSOLETE
    const char * const S_DEF_PRESCRIPTIONFORMATTING_PLAIN = QT_TRANSLATE_NOOP("mfDrugsConstants",
                                                                              "[[DRUG]][\n"
                                                                              "Take [Q_FROM]][ to [Q_TO]] [[Q_SCHEME]][ [REPEATED_DAILY_SCHEME]][ [MEAL]][ each [[PERIOD] ][PERIOD_SCHEME]][ for [D_FROM]][ to [D_TO]][ [D_SCHEME]][\n"
                                                                              "Route: [ROUTE]][\n"
                                                                              "Daily Distribution: [DISTRIBUTED_DAILY_SCHEME]][\n"
                                                                              "Minimum interval between intakes: [MIN_INTERVAL]][\n"
                                                                              "[NOTE] ]");

    const char * const S_ALD_PRE_HTML               = "DrugsWidget/print/ALDPreHtml";             /*!< \brief Key for settings. */
    const char * const S_ALD_POST_HTML              = "DrugsWidget/print/ALDPostHtml";            /*!< \brief Key for settings. */
    const char * const S_PRINTLINEBREAKBETWEENDRUGS = "DrugsWidget/print/prescription/LineBreakBetweenDrugsWhenPrinting";      /*!< \brief Key for settings. */

    const char * const S_SELECTOR_SHOWDRUGSNAME   = "DrugsWidget/Selector/ShowDrugsName";
    const char * const S_SELECTOR_SHOWROUTE       = "DrugsWidget/Selector/ShowRoute";
    const char * const S_SELECTOR_SHOWFORM        = "DrugsWidget/Selector/ShowForm";
    const char * const S_SELECTOR_SHOWSTRENGTH    = "DrugsWidget/Selector/ShowStrength";
    const char * const S_SELECTOR_SHOWMOLECULES   = "DrugsWidget/Selector/ToolTipShowMolecules";
    const char * const S_ALLERGYBACKGROUNDCOLOR   = "DrugsWidget/Selector/AllergiesBackground";
    const char * const S_INTOLERANCEBACKGROUNDCOLOR = "DrugsWidget/Selector/IntolerancesBackground";
    const char * const S_MARKDRUGSWITHAVAILABLEDOSAGES = "DrugsWidget/MarkDrugsWithAvailableDosages";      /*!< \brief Key for settings. */
    const char * const S_AVAILABLEDOSAGESBACKGROUNGCOLOR = "DrugsWidget/AvailableDosagesColorBackGround";      /*!< \brief Key for settings. */
    const char * const S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR = "#ffdddd";

    const char * const S_DATABASE_PATHS                 ="DrugsWidget/DatabaseSearchPaths";
    const char * const S_SELECTED_DATABASE_FILENAME     ="DrugsWidget/SelectedDatabaseFileName";

    const char * const S_PROTOCOL_DEFAULT_SCHEMA        ="Protocols/DefaultSchema";

    const char * const S_ACTIVATED_INTERACTION_ENGINES  ="DrugsWidget/Engines/Activated";


    // ICONS
    const char * const  I_SEARCHCOMMERCIAL =  "pill.png";              /*!< \brief Themed icon name for Commercial search. */
    const char * const  I_SEARCHMOLS =        "molecule.png";          /*!< \brief Themed icon name for Molecules search. */
    const char * const  I_SEARCHINN =         "black_dci.png";         /*!< \brief Themed icon name for INN search. */

    const char * const  I_DRUGSINFOS =        "drug_infos.png";
    const char * const  I_PROTOCOL_PRESCRIBEANDSAVE =  "protocol_saveandprescribe.png";
    const char * const  I_PROTOCOL_PRESCRIBE =         "protocol_prescribe.png";
    const char * const  I_PROTOCOL_SAVE =              "protocol_save.png";
    const char * const  I_PROTOCOL_TESTONLY =          "protocol_testonly.png";

    const char * const  I_DRUGENGINE = "drugengines.png";
    const char * const  I_DRUGDRUGINTERACTIONENGINE = "drugengine_ddi.png";
    const char * const  I_PIMENGINE = "pimengine.png";
    const char * const  I_ALLERGYENGINE = "drugengines_allergies.png";

    const char * const  I_DRUGALERT_ACCEPT = "drugalert_accept.png";
    const char * const  I_DRUGALERT_OVERRIDE = "drugalert_override.png";
    const char * const  I_DRUGREPORT = "drugreport.png";

    // INTERACTIONS ICONS
    const char * const  INTERACTION_ICONCRITICAL =         "critical.png";      /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONDECONSEILLEE =     "unadvised.png";   /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONTAKEINTOACCOUNT =  "warning.png";       /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONPRECAUTION =       "info.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONINFORMATION =      "information.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONUNKONW =           "help.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONOK =               "ok.png";            /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONP450 =             "p450iam.png";            /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONGPG =              "gpgiam.png";            /*!< \brief Themed icon name for Interactions logo. */

    // HELP PAGES
    const char * const  H_INTERACTION_SYNTHETISOR = "interactions.html";


    // LIST OF KNOWN LABORATORY FOR INN PRESCRIPTION
    const QStringList LABOS = QStringList()
                              << "ABBOT FRANCE" << "ACTAVIS" << "AGHETTANT" << "ALMUS" << "ALTER"
                              << "APOTEX FRANCE" << "APOTEX" << "ARROW CONSEIL" << "ARROW" <<"AEROCID"
                              << "ALPHARMA" << "ALCON" << "AGHETTANT" << "ACTAVIS" << "AENORASIS"
                              << "BAYER" << "BESINS-ISCOVESCO" << "BIOGARAN" << "BIOGALENIQUE" << "BOUCHARA RECORDATI"
                              << "BGR" << "BIO VALYS" << "B.R.M." << "BAXTER BIOSCIENCE" << "BAXTER" << "BMS"
                              << "BOEHRINGER INGELHEIM FRANCE" << "BIOSTABILEX"
                              << "CRISTERS" << "CCD" << "COOPER" << "CERA"
                              << "DAKOTA PHARM" << "DAKOTA" << "DCI PHARMA"
                              << "ETHYPHARM" << "EG" << "ENIREX" << "EVOLUPHARM" << "ELAIAPHARM"
                              << "FERLUX"
                              << "G GAM" << "GNR" << "GENEVRIER" << "GONNON" << "GLAXOSMITHKLINE SANTE GRAND PUBLIC"
                              << "GSK" << "GIFRER" << "GENECEF" << "GERDA" << "GENEVAR" << "GSL" << "GENERES"
                              << "HEXAL SANTE" << "HERMES" << "HOSPIRA" << "H3 PHARMA" << "H3 SANTE"
                              << "IVAX" << "IDD" << "IBD3"
                              << "LEURQUIN MEDIOLANUM"
                              << "MERCK GENERIQUES" << "MERCK GENERICS FRANCE HOLDING" << "MERCK" << "MYLAN PHARMA" << "MYLAN" << "MEDA PHARM" << "MG PHARMA"
                              << "MEDIPHA SANTE"
                              << "NIALEX"
                              << "PAUCOURT" << "PHARMEMMA" << "PANPHARMA" << "PHARMANOVA" << "PFIZER" << "PANPHARMA"
                              << "PIERRE FABRE" << "PROGRAPHARM" << "PAN PHARMA"
                              << "QUALIMED" << "QUALIHEALTH" << "QUIVER"
                              << "RANBAXY" << "RATIOPHARM CONSEIL" << "RATIOPHARM" << "RANBAXY" << "RPG" << "RENAUDIN"
                              << "ROTTAPHARM" << "RATIO"
                              << "SANDOZ CONSEIL" << "SANDOZ" << "STRAGEN" << "SAINT GERMAIN" << "SET" << "SUBSTIPHARM"
                              << "SANOFI PHARMA" << "SANOFI SYNTHELABO FRANCE" << "SICOR" << "SG-PHARM" << "SPECIFAR"
                              << "SMITHKLINE BEECHAM" << "SOLVAY PHARMA" << "SYNTHON"
                              << "TEVA PHARMA" << "TEVA CONSEIL" << "TEVA" << "THERAPLIX" << "TORLAN"
                              << "TOP PHARM" << "TRADIPHAR"
                              << "TABUGEN"
                              << "VIAREF"
                              << "WINTHROP" << "WYVERN MEDICAL" << "WYVERN"
                              << "ZYDUS"
                              ;


    // Translations
    const char * const  DRUGSBASE_TR_CONTEXT = "mfDrugsConstants";
    const char * const  NATURE_OF_RISK = QT_TRANSLATE_NOOP("mfDrugsConstants", "Nature of risk");
    const char * const  MANAGEMENT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Management");
    const char * const  DDI_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Drug-drug interactions");

    // Tokens translations
    const char * const  TOKEN_TEXT_PRESCRIPTION     = QT_TRANSLATE_NOOP("mfDrugsConstants", "Prescription");
    const char * const  TOKEN_TEXT_DRUG             = QT_TRANSLATE_NOOP("mfDrugsConstants", "Drug");
    const char * const  TOKEN_TEXT_PROTOCOL         = QT_TRANSLATE_NOOP("mfDrugsConstants", "Protocol");
    const char * const  TOKEN_TOOLTIP_PRESCRIPTION  = QT_TRANSLATE_NOOP("mfDrugsConstants", "This token namespace contains all prescription's' related tokens.\n"
                                                                       "It includes the drug brand name and all the items of your protocol.");
    const char * const  TOKEN_TEXT_DRUGNAME         = QT_TRANSLATE_NOOP("mfDrugsConstants", "Drug brand name");
    const char * const  TOKEN_TEXT_Q_FROM           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Quantity (from)");
    const char * const  TOKEN_TEXT_Q_TO             = QT_TRANSLATE_NOOP("mfDrugsConstants", "Quantity (to)");
    const char * const  TOKEN_TEXT_Q_SCHEME         = QT_TRANSLATE_NOOP("mfDrugsConstants", "Quantity (scheme)");
    const char * const  TOKEN_TEXT_Q_FULL           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Quantity (complete)");
    const char * const  TOKEN_TEXT_MEAL             = QT_TRANSLATE_NOOP("mfDrugsConstants", "Meal relation");
    const char * const  TOKEN_TEXT_P_VALUE          = QT_TRANSLATE_NOOP("mfDrugsConstants", "Period (value)");
    const char * const  TOKEN_TEXT_P_SCHEME         = QT_TRANSLATE_NOOP("mfDrugsConstants", "Period (scheme)");
    const char * const  TOKEN_TEXT_P_FULL           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Period (complete)");
    const char * const  TOKEN_TEXT_D_FROM           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Duration (from)");
    const char * const  TOKEN_TEXT_D_TO             = QT_TRANSLATE_NOOP("mfDrugsConstants", "Duration (to)");
    const char * const  TOKEN_TEXT_D_SCHEME         = QT_TRANSLATE_NOOP("mfDrugsConstants", "Duration (scheme)");
    const char * const  TOKEN_TEXT_D_FULL           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Duration (complete)");
    const char * const  TOKEN_TEXT_ROUTE            = QT_TRANSLATE_NOOP("mfDrugsConstants", "Route");
    const char * const  TOKEN_TEXT_DISTRIB          = QT_TRANSLATE_NOOP("mfDrugsConstants", "Daily distribution");
    const char * const  TOKEN_TEXT_REPEAT           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Repeated ditribution");
    const char * const  TOKEN_TEXT_MININTERV_FULL   = QT_TRANSLATE_NOOP("mfDrugsConstants", "Minimum interval between intakes (complete)");
    const char * const  TOKEN_TEXT_MININTERV_VALUE  = QT_TRANSLATE_NOOP("mfDrugsConstants", "Minimum interval between intakes (value)");
    const char * const  TOKEN_TEXT_MININTERV_SCHEME = QT_TRANSLATE_NOOP("mfDrugsConstants", "Minimum interval between intakes (scheme)");
    const char * const  TOKEN_TEXT_REFILL           = QT_TRANSLATE_NOOP("mfDrugsConstants", "Refill");
    const char * const  TOKEN_TEXT_NOTE             = QT_TRANSLATE_NOOP("mfDrugsConstants", "Note");

     /*: This is the short name of the drug-drug interactions engine. */
    const char * const  DDI_SHORT_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "DDI");

    const char * const  PIMENGINE_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Potentially inappropriate medications");

    /*: This is the short name of the potentially inappropriate medications engine. */
    const char * const  PIMENGINE_SHORT_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "PIM");
    const char * const  PIM_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Potentially inappropriate medications");


    const char * const  ALLERGYENGINE_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Allergy / Intolerances to medications");

    /*: This is the short name of the potentially inappropriate medications engine. */
    const char * const  ALLERGY_TEXT = QT_TRANSLATE_NOOP("mfDrugsConstants", "Allergy");

    // Some useful enums
    enum LevelOfInteractionWarnings {
        MaximumLevelOfWarning = 0,
        ModerateLevelOfWarning,
        MinimumLevelOfWarning
    };

    // Drugs engines uid
    const char * const  DDI_ENGINE_UID = "ddiEngine";
    const char * const  PIM_ENGINE_UID = "pimEngine";
    const char * const  ALLERGY_ENGINE_UID = "allergyEngine";

    //--------------------------------------------------------------------------------------------------------
    //------------------------------------- Enums for Models Management --------------------------------------
    //--------------------------------------------------------------------------------------------------------

    /** \brief Used by DrugsModel for column representation. */

namespace Drug
{
    enum Drug
    {
        Id = 1000,
        UIDs,
        OldUid,
        DrugId,
        Pack_UID,
        Denomination,
        UidName,
        Form,
        Route,
        GlobalStrength,
        IsScoredTablet,
        Molecules,
        CodeMoleculesList,
        AllInnsKnown,
        Inns,                    // names
        InnCodes,                // all INN in ATC format (classes not included)
        InnsATCcodes,            // all INN in ATC format (classes are included)
        InnClasses,              // names
        MainInnCode,             // -1 if no INN known or more than one INN
        MainInnName,             // -1 if no INN known or more than one INN
        ATC,
        MainInnDosage,
        Administration,
        Interacts,
        OwnInteractionsSynthesis,
//        MaximumLevelOfInteraction,
        CompositionString,
        InnCompositionString,    // inn dose form ; inn dose form...
        AvailableForms,
        AvailableRoutes,
        AvailableDosages,
        HasPrescription,
        FullPrescription,
        LinkToSCP,
        MaxParam
    };

}  // End namespace Drugs::Constants::Drug

/** \brief Used by DrugsModel for column representation. */
namespace Prescription
{
    enum Prescription
    {
        Id = 1200,
        IsTextualOnly,    // Set for textual drugs
        UsedDosage,
        Pack_UID,
        OnlyForTest,
        IntakesFullString,  // READ ONLY
        IntakesFrom,
        IntakesTo,
        IntakesScheme,
        IntakesUsesFromTo,
        Route,            // Textual translated route
        RouteId,          // Route Id from drugs database
        DurationFullString,  // READ ONLY
        DurationFrom,
        DurationTo,
        DurationScheme,
        DurationUsesFromTo,
        PeriodFullString,  // READ ONLY
        Period,
        PeriodScheme,
        SerializedDailyScheme,
        MealTimeSchemeIndex,
        IntakesIntervalFullString,
        IntakesIntervalOfTime,
        IntakesIntervalSchemeIndex,
        Note,
        IsINNPrescription,
        SpecifyForm,
        SpecifyPresentation,
        IsALD,
        Refill,
        ToHtml,
        MaxParam
    };


}  // End namespace Drugs::Constants::Prescription

/** \brief Used by DrugsModel for column representation. */
namespace Interaction
{
    enum InteractionModelFields
    {
        Id = 2000,
        Icon,
        Pixmap,
        ToolTip,
        FullSynthesis,
        MaxParam
    };

}  // End namespace Drugs::Constants::Interaction

}  // End namespace DrugsDB::Constants
}  // End namespace DrugsDB


namespace Dosages {

    /**  \brief Some constants. */
namespace Constants {
    const char * const DB_DOSAGES_NAME = "dosages";          /*!< \brief Dosage database connection name. */
    const char * const DB_DOSAGES_FILENAME = "dosages.db";   /*!< \brief Dosage database name (SQLite). */
    const char * const DB_DOSAGES_TABLE_NAME = "DOSAGE";              /*!< \brief Table name for dosages. */

    /** \brief Used by DosageModel for column representation. */
    enum Dosage
    {
        Id = 0,                 /*!< \brief Dose Identifier. Integer.*/
        Uuid,                   /*!< \brief Unique identifier. Varchar(40).*/
        DrugsDatabaseIdentifiant,    /*!< \brief Unique FreeRx identifier.*/
        INN_LK,                 /*!< \brief If the prescription is the same for
                                   all INNs, then this field  is used. If not,
                                   it defaults to -1. Integer.*/
        InnLinkedDosage,        /*!< \brief Linked dosage of the INN.*/
        DrugUid_LK,                 /*!< \brief If the prescription is valid only
                                    only for this drug, the CIS code is used. If
                                    not, it defaults to -1. Integer.*/
        CIP_LK,                 /*!< \brief CIP code. Integer.*/
        Label,                  /*!< \brief Label. Varchar(300).*/

        IntakesFrom,             /*!< \brief Minimum number of intakes. Double.*/
        IntakesTo,               /*!< \brief Maximum number of intakes. Double.*/
        IntakesUsesFromTo,       /*!< \brief Intake uses the minium/maximum
                                    "from x to y" scheme. Bool.*/
        IntakesScheme,           /*!< \brief Selected pharmaceutical formulation.*/
        IntakesIntervalOfTime,  /*!< \brief Time interval between intakes. Integer.*/
        IntakesIntervalScheme,  /*!< \brief Time interval unit. Integer. cf enum */
        RouteId,

        DurationFrom,           /*!< \brief Minimum duration of treatment. Integer.*/
        DurationTo,             /*!< \brief Maximum duration of treatment. Integer.*/
        DurationUsesFromTo,     /*!< \brief Duration uses the "from x to y" scheme. Bool.*/
        DurationScheme,         /*!< \brief Duration unit encoding. Integer cf enum Period.*/

        Period,
        PeriodScheme,
        AdministrationScheme,
        SerializedDailyScheme,
        MealScheme,             /*!< \brief Time of intake relative to meal/food
                                    cf enum mfDrugDosage::MealTime */
        IsALD,                  /*!< \brief ALD prescription (French social security specific.)*/
        TypeOfTreatment,        /*!< \brief Drugs prescription type (acute, long-term medication)
                                      \todo this */
        MinAge,                 /*!< \brief Minimum age in months.*/
        MaxAge,                 /*!< \brief Maximum age in months.*/
        MinAgeReferenceIndex,   /*!< \brief Reference (unit) for minimum age: hours,
                                days, weeks, months, years. Integer. cf Enum Period */
        MaxAgeReferenceIndex,   /*!< \brief Reference (unit) for maximum age: hours,
                                days, weeks, months, years. Integer. cf Enum Period */
        MinWeight,              /*!< \brief Minimum weight in kilograms. Integer.*/
        SexLimitedIndex,        /*!< \brief int 0 = M / 1 = F */
        MinClearance,           /*!< \brief Minimum creatinine clearance to allow prescription.*/
        MaxClearance,           /*!< \brief Maximum creatinine clearance. TODO: Is this relevant?*/
        PregnancyLimitsFlag,    /*!< \brief Limits for pregnancy flaged from tkConstants::Physiology::PregnancyLimits. */
        BreastFeedingLimitsIndex, /*!< \brief Limits for pregnancy flaged from tkConstants::Physiology::BreastFeedingLimits. */
        PhysiologicalLimitsFlag,  /*!< \brief Physiological limits flaged from tkConstants::Physiology::PhysiologicalLimitsFlag. */

        Note,                   /*!< \brief note               --> varchar ( 500 ) */

        CIM10Links,             /*!< \brief CIM10 codes related to the prescription. Codes are separated by a ';'. \todo this  */
        CIM10LimitsLinks,       /*!< \brief CIM10 codes that must not be used with the prescription. Codes are separated by a ';'.  \todo this */
        EDRCLinks,              /*!< \brief EDRC codes related to the prescription. Codes are separated by a ';'.  \todo this  */

        Extras,                 /*!< \brief blob that contains extra data, XML formatted (not used by now) */
        UserValidatorName,      /*!< \brief Name of user who validated the dosage.*/
        CreationDate,           /*!< \brief Creation date --> date */
        ModificationDate,       /*!< \brief Modification date --> date */
        Transmitted,            /*!< \brief Could be used for Internet transmission of local dosages. Date.*/
        Route,                  /*!< \brief Outside SQL */
        Refill,                  /*!< \brief Outside SQL */
        MaxParam                /*!< \brief Maximal number of columns of the model
                                (and of the SQL table).*/
    };

}  // End namespace Dosages::Constants
}  // End namespace Dosages


typedef QHash<int, QString> QHashWhere;

#endif // DRUGSBASE_CONSTANTS_H
