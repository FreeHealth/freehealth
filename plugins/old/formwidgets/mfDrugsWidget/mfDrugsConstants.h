/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFDRUGSCONSTANTS_H
#define MFDRUGSCONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 \brief Some constants for Drugs management
 \ingroup freediams drugswidget
*/
namespace mfDrugsConstants
{
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
    const char * const  MFDRUGS_SETTING_DRUGFONT               = "DrugsWidget/print/drug/Font";              /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_HIDELABORATORY         = "DrugsWidget/print/drug/hideLaboratory";    /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_PRESCRIPTIONFONT       = "DrugsWidget/print/prescription/Font";      /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_WATERMARKALIGNEMENT    = "DrugsWidget/print/watermark/Alignment";    /*!< \brief Key for settings. \obsolete*/
    const char * const  MFDRUGS_SETTING_WATERMARKPRESENCE      = "DrugsWidget/print/watermark/Presence";     /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_WATERMARK_HTML         = "DrugsWidget/print/watermark/Html";         /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_ALD_PRE_HTML           = "DrugsWidget/print/ALDPreHtml";             /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_ALD_POST_HTML          = "DrugsWidget/print/ALDPostHtml";            /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_USERHEADER             = "DrugsWidget/user/Header";                  /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_USERFOOTER             = "DrugsWidget/user/Footer";                  /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_USER                   = "DrugsWidget/user/Name";                    /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_PASSWORD               = "DrugsWidget/user/Pass";                    /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_SERIAL                 = "DrugsWidget/user/SN";                      /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_SEARCHMETHOD           = "DrugsWidget/searchMethod";                 /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_HISTORYSIZE            = "DrugsWidget/historySize";                  /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_DRUGHISTORY            = "DrugsWidget/drugsHistory";                 /*!< \brief Key for settings. */
    const char * const  MFDRUGS_SETTING_LEVELOFWARNING         = "DrugsWidget/levelOfWarning";               /*!< \brief Key for settings. */

    const char * const  MFDRUGS_SETTING_USERRECORDEDFORMS      = "DrugsWidget/userRecordedForms";            /*!< \brief Key for settings. */

    // DEFAULTS SETTINGS
    const char * const  MFDRUGS_DEFAULT_ALD_PRE_HTML           = "<table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" style=\"border-style:solid;\">\n<tr>\n<td width=\"100%\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;font-family:Arial Narrow;font-size:10pt;color:black;\">Prescriptions relatives au traitement de l'affection de longue dur&eacute;e reconnue (liste ou hors liste)<br />(AFFECTION EXON&Eacute;RANTE)</span></p></td></tr></table>";              /*!< \brief Default value for settings. */
    const char * const  MFDRUGS_DEFAULT_ALD_POST_HTML          = "<table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" style=\"border-style:solid;\">\n<tr>\n<td width=\"100%\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;font-family:Arial Narrow;font-size:10pt;color:black;\">Prescriptions SANS RAPPORT avec l'affection de longue dur&eacute;e<br />(MALADIES INTERCURRENTES)</span></p></td></tr></table>";              /*!< \brief Default value for settings. */

    const char * const  MFDRUGS_DEFAULT_WATEMARKHTML           = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body>\n<p align=\"center\"  style=\" font-family:'Lucida Grande'; font-size:12pt; font-weight:bold; font-style:normal; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#9c9c9c;\">Duplicata ne permettant pas la d&eacute;livrance des m&eacute;dicaments</span></p>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#9c9c9c;\">A adresser &agrave; votre centre de S&eacute;curit&eacute; Sociale.</span></p></body></html>";              /*!< \brief Default value for settings. */
    const char * const  MFDRUGS_DEFAULT_USERHEADER             = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body>\n"
                                                                 "<table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>\n"
                                                                 "<td width=\"33%\">\n<p align=\"center\"><span style=\" font-weight:600;\">Dr TRUCMUCHE Bidulle</span>\n<br /><span style=\" font-weight:600;\">M&eacute;decine Machin</span><\n<br /><br /><span style=\" font-size:10pt; font-style:italic;\">Ancien Assistant des h&ocirc;pitaux</span>\n<br /><span style=\" font-size:10pt; font-style:italic;\">Tous mes dipl&ocirc;mes</span>\n<br />----------<br />\n<span style=\" font-size:10pt;\">Mes Consultations sur rendez-vous</span>\n<br />----------\n<br /><span style=\" font-size:10pt; font-style:italic;\">Mon ADELI</span></p></td>\n<td width=\"33%\"></td>\n"
                                                                 "<td width=\"33%\">\n<p align=\"center\"><span style=\" font-size:9pt;\">2, rue de la M&eacute;decine\n<br />62 223 SAINT-LAURENT-BLANGY<br />docteur@trucmuche.fr<br />T&eacute;l : 03.21.00.00.00<br />Fax : 03.21.00.00.06<br />----------<br /></span>"
                                                                 "<span style=\" font-size:12pt;font-weight:bold;\">[[PATIENTNAME]<br />][Le [DATE]]</span></p></td></tr></table></body></html>";              /*!< \brief Default value for settings. */
//    const char * const  MFDRUGS_DEFAULT_USERFOOTER             = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Lucida Grande'; font-size:13pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">En cas d'urgence applez le SAMU : 112</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Veuillez signalez les demandes de visites &agrave; domicile, tant que possible, avant 10h.</span></p></body></html>";


    const char * const  MFDRUGS_SETTINGS_STATEPREFIX =   "DrugsWidget/";

    // ICONS
    const char * const  ICONTOOGLETESTINGDRUGS       =  "pill_hidden.png";       /*!< \brief Themed icon name for Commercial search. */
    const char * const  MFDRUGS_ICONSEARCHCOMMERCIAL =  "pill.png";              /*!< \brief Themed icon name for Commercial search. */
    const char * const  MFDRUGS_ICONSEARCHMOLS =        "molecule.png";          /*!< \brief Themed icon name for Molecules search. */
    const char * const  MFDRUGS_ICONSEARCHINN =         "black_dci.png";         /*!< \brief Themed icon name for INN search. */

    // INTERACTIONS ICONS
    const char * const  INTERACTION_ICONCRITICAL =         "critical.png";      /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONDECONSEILLEE =     "deconseille.png";   /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONTAKEINTOACCOUNT =  "warning.png";       /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONPRECAUTION =       "info.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONINFORMATION =      "info.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONUNKONW =           "help.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONOK =               "ok.png";            /*!< \brief Themed icon name for Interactions logo. */

    // TEXT COMPLETION
    const char * const  TOKEN_PATIENTAGE   ="AGE";                         /*!< /brief Tokens for diPatient::replaceTokens() */
    const char * const  TOKEN_PATIENTNAME  ="PATIENTNAME";                      /*!< /brief Tokens for diPatient::replaceTokens() */
    const char * const  TOKEN_DATEOFBIRTH  ="DATEOFBIRTH";                      /*!< /brief Tokens for diPatient::replaceTokens() */
    const char * const  TOKEN_WEIGHT       ="WEIGHT";                      /*!< /brief Tokens for diPatient::replaceTokens() */
    const char * const  TOKEN_SIZE         ="SIZE";                      /*!< /brief Tokens for diPatient::replaceTokens() */
    const char * const  TOKEN_CLCR         ="CLCR";                      /*!< /brief Tokens for diPatient::replaceTokens() */
    const char * const  TOKEN_DATE         ="DATE";

    // HTML PREPARERS
    const char * const  ENCODEDHTML_FULLDOC =
            "<html>\n"
            "<head>\n"
            "  <meta name=\"qrichtext\" content=\"1\" />\n"
            "  <meta name=\"charset\" content=\"UTF-8\" />\n"
            "  <meta name=\"generator\" content=\"{GENERATOR}\" />\n"
            "  <title>FreeDiams : Prescription</title>\n"
            "</head>\n"
            "<body style=\"font-family:Arial;font-size:12pt\">\n"
            "<a href=\"{ENCODEDPRESCRIPTION}\">{PRESCRIPTION}</a>\n"
            "</body>\n"
            "</html>\n";
    const char * const  ENCODEDHTML_DRUG =
//            "<p>"
            "<span style=\"{DRUGSTYLE}\">"
            "{NUMBER}&nbsp;.&nbsp;{DRUG}"
            "</span>\n"
            "<br />\n"
            "<span style=\"{PRESCRIPTIONSTYLE}\">\n"
            "{PRESCRIPTION}<br />"
//            "&nbsp;&nbsp;&nbsp;{NOTE}\n"
            "</span>\n"
            "<br />\n";
    const char * const  ENCODEDHTML_FULLPRESCRIPTION     = "<table border=0><tr><td>{FULLPRESCRIPTION}</td></tr></table>";
    const char * const  ENCODEDHTML_FREEDIAMSTAG = "FreeDiamsEncodedPrescription:";          /*!< \brief Default tag prepend to serialized prescription when saving. */

    const char* const  FREEDIAMS_FILEFILTER = QT_TRANSLATE_NOOP("mfDrugsConstants", "FreeDiams Prescriptions (*.di)");

    // LIST OF KNOWN LABORATORY FOR INN PRESCRIPTION
    const QStringList LABOS = QStringList()
                              << "ABBOT FRANCE " << "ACTAVIS " << "AGHETTANT " << "ALMUS " << "ALTER "
                              << "APOTEX FRANCE " << "APOTEX " << "ARROW CONSEIL " << "ARROW " <<"AEROCID"
                              << "ALPHARMA " << "ALCON " << "AGHETTANT " << "ACTAVIS " << "AENORASIS"
                              << "BAYER " << "BESINS-ISCOVESCO " << "BIOGARAN " << "BIOGALENIQUE " << "BOUCHARA RECORDATI "
                              << "BGR " << "BIO VALYS " << "B.R.M. " << "BAXTER BIOSCIENCE " << "BAXTER " << "BMS"
                              << "BOEHRINGER INGELHEIM FRANCE" << "BIOSTABILEX"
                              << "CRISTERS " << "CCD " << "COOPER " << "CERA "
                              << "DAKOTA PHARM " << "DAKOTA" << "DCI PHARMA "
                              << "ETHYPHARM " << "EG " << "ENIREX " << "EVOLUPHARM " << "ELAIAPHARM"
                              << "FERLUX"
                              << "G GAM " << "GNR " << "GENEVRIER " << "GONNON " << "GLAXOSMITHKLINE SANTE GRAND PUBLIC"
                              << "GSK " << "GIFRER" << "GENECEF" << "GERDA" << "GENEVAR" << "GSL" << "GENERES"
                              << "HEXAL SANTE " << "HERMES " << "HOSPIRA " << "H3 PHARMA" << "H3 SANTE"
                              << "IVAX " << "IDD " << "IBD3"
                              << "LEURQUIN MEDIOLANUM"
                              << "MERCK GENERIQUES" << "MERCK GENERICS FRANCE HOLDING" << "MERCK " << "MYLAN PHARMA " << "MYLAN " << "MEDA PHARM " << "MG PHARMA "
                              << "MEDIPHA SANTE "
                              << "NIALEX"
                              << "PAUCOURT " << "PHARMEMMA " << "PANPHARMA" << "PHARMANOVA" << "PFIZER" << "PANPHARMA"
                              << "PIERRE FABRE" << "PROGRAPHARM" << "PAN PHARMA"
                              << "QUALIMED " << "QUALIHEALTH " << "QUIVER "
                              << "RANBAXY " << "RATIOPHARM CONSEIL " << "RATIOPHARM " << "RANBAXY " << "RPG " << "RENAUDIN "
                              << "ROTTAPHARM " << "RATIO"
                              << "SANDOZ CONSEIL " << "SANDOZ " << "STRAGEN " << "SAINT GERMAIN " << "SET " << "SUBSTIPHARM "
                              << "SANOFI PHARMA " << "SANOFI SYNTHELABO FRANCE" << "SICOR" << "SG-PHARM" << "SPECIFAR"
                              << "SMITHKLINE BEECHAM" << "SOLVAY PHARMA" << "SYNTHON"
                              << "TEVA PHARMA" << "TEVA CONSEIL " << "TEVA " << "THERAPLIX " << "TORLAN "
                              << "TOP PHARM " << "TRADIPHAR"
                              << "TABUGEN"
                              << "VIAREF"
                              << "WINTHROP " << "WYVERN MEDICAL" << "WYVERN"
                              << "ZYDUS "
                              ;


    // DATABASE
    const char * const DRUGS_DATABASE_NAME     = "drugs";            /*!< \brief Drugs database connection name. */
    const char * const DRUGS_DATABASE_FILENAME = "drugs.db";         /*!< \brief Drug database file name (for SQLite). */

    //--------------------------------------------------------------------------------------------------------
    //------------------------------------ Enums for Database Management -------------------------------------
    //--------------------------------------------------------------------------------------------------------
    /** \brief Represents the tables index of drugs database */
    enum Tables
    {
        Table_CIS=0, Table_COMPO, Table_CIS_CIP, Table_CPAM, Table_INTERACT,
        Table_IAM, Table_IAM_DENOMINATION, Table_DOSAGE,
        Table_MaxParam
    };

    /** \brief Represents the fields index of drugs database table CIS */
    enum CISfields
    { CIS_CIS = 0,CIS_DENOMINATION, CIS_FORME, CIS_ADMINISTRATION, CIS_AMM,
      CIS_AUTORISATION, CIS_COMMERCIALISATION, CIS_CODE_RPC,
      CIS_MaxParam
    };

    /** \brief Represents the fields index of drugs database table CIP */
    enum CISPfields // table CIS_CIP
    { CISP_CIS = 0, CISP_CIP, CISP_LIBELLE, CISP_STATUT, CISP_COMMERCIALISATION,
      CISP_DATE, CISP_CIPLONG,
      CISP_MaxParam
    };

    /** \brief Represents the fields index of drugs database table COMPO */
    enum COMPOfields
    { COMPO_CIS = 0, COMPO_NOM, COMPO_CODE_SUBST, COMPO_DENOMINATION, COMPO_DOSAGE, COMPO_REF_DOSAGE,
      COMPO_NATURE, COMPO_LK_NATURE,
      COMPO_MaxParam, COMPO_IAM_DENOMINATION, COMPO_IAM_CLASS_DENOMINATION, COMPO_VeryMaxParam
    };

    /** \brief Represents the fields index of drugs database table IAM_DENOMINATION */
    enum IAM_DENOMINATIONfields
    { IAM_DENOMINATION_ID = 0, IAM_DENOMINATION,
      IAM_DENOMINATION_MaxParam
    };

    /** \brief Represents the fields index of drugs database table IAM_IMPORT */
    enum IAMfields
    { IAM_ID = 0, IAM_ID1, IAM_ID2, IAM_TYPE, IAM_TEXT_IAM, IAM_TEXT_CAT,
      IAM_MaxParam, IAM_MAIN, IAM_INTERACTOR
    };

    //--------------------------------------------------------------------------------------------------------
    //------------------------------------- Enums for Models Management --------------------------------------
    //--------------------------------------------------------------------------------------------------------
/**
 \brief Used by mfDrugsModel for column representation.
 \ingroup freediams
 \ingroup drugswidget
 */
namespace Drug
{
    enum Drug
    {
        Id = 1000,
        CIS,
        CIPs,
        Denomination,
        Form,
        IsScoredTablet,
        Molecules,
        CodeMoleculesList,
        AllInnsKnown,
        Inns,                    // names
        InnClasses,              // names
        MainInnCode,             // -1 if no INN known or more than one INN
        MainInnName,             // -1 if no INN known or more than one INN
        MainInnDosage,
        Administration,
        Interacts,
        MaximumLevelOfInteraction,
        CompositionString,
        InnCompositionString,    // inn dose form ; inn dose form...
        AvailableForms,
        AvailableDosages,
        HasPrescription,
        FullPrescription,
        MaxParam
    };
}  // End namespace mfDrugsConstants::Drug

/**
 \brief Used by mfDrugsModel for column representation.
 \ingroup freediams
 \ingroup drugswidget
 */
namespace Prescription
{
    enum Prescription
    {
        Id = 1200,
        UsedDosage,
        CIP,
        OnlyForTest,
        IntakesFrom,
        IntakesTo,
        IntakesScheme,
        IntakesUsesFromTo,
        IntakesFullString,
        DurationFrom,
        DurationTo,
        DurationScheme,
        DurationUsesFromTo,
        Period,
        PeriodScheme,
        DailyScheme,
        MealTimeSchemeIndex,
        IntakesIntervalOfTime,
        IntakesIntervalScheme,
        Note,
        IsINNPrescription,
        SpecifyForm,
        SpecifyPresentation,
        IsALD,
        ToHtml,
        MaxParam
    };


}  // End namespace mfDrugsConstants::Prescription

}  // End namespace mfDrugsConstants


/**
 \brief Some constants.
 \ingroup freediams
 \ingroup drugswidget
*/
namespace mfDosagesConstants
{
    const char * const DOSAGES_DATABASE_NAME = "dosages";          /*!< \brief Dosage database connection name. */
    const char * const DOSAGES_DATABASE_FILENAME = "dosages.db";   /*!< \brief Dosage database name (SQLite). */
    const char * const DOSAGES_TABLE_NAME = "DOSAGE";              /*!< \brief Table name for dosages. */
    const char * const DOSAGES_DEFAULT_USER_UUID = "freediams.default.user";  // this is only used by DrugsInteractions

/**
 \brief Used by mfDosageModel for column representation.
 \ingroup freediams
 \ingroup drugswidget
*/
namespace Dosage
{
    /** \brief Used by mfDosageModel for column representation. */
    enum Dosage
    {
        Id = 0,                 /*!< \brief Identifiant du dosage. integer. */
        Uuid,                   /*!< \brief Identifiant unique --> varchar(40) */
        INN_LK,                 /*!< \brief Si la prescription est la même pour toutes les DCI alors le champs est renseigné, sinon laissé à -1. integer */
        InnLinkedDosage,        /*!< \brief Linked dosage of the Inn. */
        CIS_LK,                 /*!< \brief Si la prescription est unique valable pour ce médicament le code CIS et renseigné, sinon laissé à -1. integer */
        CIP_LK,                 /*!< \brief Code CIP           --> int */
        Label,                  /*!< \brief Libellé            --> varchar( 300 ) */

        IntakesFrom,             /*!< \brief Nb de prises       --> double */
        IntakesTo,               /*!< \brief Nb de prises       --> double  */
        IntakesUsesFromTo,       /*!< \brief bool : intake uses the "from x to y" scheme */
        IntakesScheme,           /*!< \brief Forme sélectionnée */
        IntakesIntervalOfTime,  /*!< \brief temps entre chaque prise --> int */
        IntakesIntervalScheme,  /*!< \brief schéma de temps --> int cf enum */

        DurationFrom,           /*!< \brief espacement prises  --> double */
        DurationTo,             /*!< \brief espacement prises  --> double */
        DurationUsesFromTo,     /*!< \brief bool : duration uses the "from x to y" scheme */
        DurationScheme,         /*!< \brief codage             --> int cf enum Period */

        Period,
        PeriodScheme,
        AdministrationScheme,
        DailyScheme,            /*!< \brief  */
        MealScheme,             /*!< \brief Schéma de prise par rapport aux repas // cf enum mfDrugDosage::MealTime */
        IsALD,                  /*!< \brief Prescription en ALD */
        TypeOfTreatment,        /*!< \brief Type de traitement (chronique, aiguë...) enumerator à créer  \todo this */

        MinAge,                 /*!< \brief Limite d'âge en mois */
        MaxAge,                 /*!< \brief Limite d'âge en mois */
        MinAgeReferenceIndex,   /*!< \brief Reference pour la limite d'âge : heures, jours, semaines, mois, années      --> int cf Enum Period */
        MaxAgeReferenceIndex,   /*!< \brief Reference pour la limite d'âge : heures, jours, semaines, mois, années      --> int cf Enum Period */
        MinWeight,              /*!< \brief Poids minimal      --> int                     40 (kg) */
        SexLimitedIndex,        /*!< \brief int 0 = M / 1 = F */
        MinClearance,           /*!< \brief Clearance de la créatinine minimale pour la prescription */
        MaxClearance,           /*!< \brief Clearance de la créatinine maximale pour la prescription (définition d'interval). Eg : HBPM */
        PregnancyLimitsFlag,    /*!< \brief Limits for pregnancy flaged from tkConstants::Physiology::PregnancyLimits. */
        BreastFeedingLimitsIndex, /*!< \brief Limits for pregnancy flaged from tkConstants::Physiology::BreastFeedingLimits. */
        PhysiologicalLimitsFlag,  /*!< \brief Physiological limits flaged from tkConstants::Physiology::PhysiologicalLimitsFlag. */

        Note,                   /*!< \brief note               --> varchar ( 500 ) */

        CIM10Links,             /*!< \brief CIM10 codes related to the prescription. Codes are separated by a ';'. \todo this  */
        CIM10LimitsLinks,       /*!< \brief CIM10 codes that must not be used with the prescription. Codes are separated by a ';'.  \todo this */
        EDRCLinks,              /*!< \brief EDRC codes related to the prescription. Codes are separated by a ';'.  \todo this  */

        Extras,                 /*!< \brief blob that contains extra datas XML formatted (not used for now */
        UserValidatorName,      /*!< \brief nom de celui qui a validé la posologie */
        CreationDate,           /*!< \brief Date création      --> date */
        ModificationDate,       /*!< \brief Date modification  --> date */
        Transmitted,            /*!< \brief Sera utilisé pour récupérer via internet les dosages référencés en local. date */
        MaxParam                /*!< \brief Nombre maximum de colonnes du modèle (et de la table SQL) */
    };

}  // End namespace mfDosagesConstants::Dosage

}  // End namespace mfDosagesConstants

namespace mfInteractionsConstants
{

/**
 \brief Used by mfDrugsModel for column representation.
 \ingroup freediams
 \ingroup drugswidget
*/
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

    enum TypeOfIAM
    {
        noIAM            = 0x00000000,
        Precaution       = 0x00000001,
        APrendreEnCompte = 0x00000010,
        Deconseille      = 0x00000100,
        ContreIndication = 0x00001000,
        Information      = 0x00010000,
        Unknown          = 0x00100000
    };
    Q_DECLARE_FLAGS( TypesOfIAM, TypeOfIAM );

}  // End namespace mfInteractionsConstants::Interaction
Q_DECLARE_OPERATORS_FOR_FLAGS( Interaction::TypesOfIAM )

}  // End namespace mfInteractionsConstants

typedef QHash<int, QString> QHashWhere;

#endif // MFDRUGSCONSTANTS_H
