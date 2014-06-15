/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,       *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT_CONSTANTS_H
#define ACCOUNT_CONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.1.0
 * \date 18 Mar 2010
 * \brief Some constants for Accountancy management
 * \ingroup freeaccount account
*/
namespace Account {
namespace Constants {

    // Menu and action for ActionManager
    const char * const  DRUGCONSTANTS_TR_CONTEXT = "Account";
    const char * const  M_PLUGINS_ACCOUNT       =  "Acc.menuAccount";
    const char * const  M_PLUGINS_SEARCH        =  "Acc.menuSearch";
    const char * const  M_PLUGINS_MODES         =  "Acc.menuModes";

    const char * const  C_ACCOUNT               =  "ContextAccount";

    const char * const  G_ACCOUNT_APPS          =  "Acc.groupApps";
    const char * const  G_ACCOUNT_SEARCH        =  "Acc.groupSearch";
    const char * const  G_ACCOUNT_MODES         =  "Acc.groupModes";

    const char * const  C_DRUGS_PLUGINS         =  "Acc.ContextDrugs";
    const char * const  C_DRUGS_SEARCH          =  "Acc.ContextSearch";

    const char* const   A_ACCOUNT_BOOK          = "a.Acc.AccBook";
    const char* const   A_REVENUES              = "a.Acc.Revenues";
    const char* const   A_EXPENSES              = "a.Acc.Expenses";
    const char* const   A_ADDRECEIPTS           = "a.Acc.AddReceipts";
    const char* const   A_RECEIPTS              = "a.Acc.Receipts";
    const char* const   A_LEDGER                = "a.Acc.Ledger";
    const char* const   A_MOVEMENTS             = "a.Acc.Movements";
    const char* const   A_ASSETS                = "a.Acc.Assets";
    const char* const   A_ACCOUNT               = "a.Acc.AccountView";

    // Translations
    const char* const   ACCOUNT_TR_CONTEXT      = "Account";
    const char* const   ACCOUNTMENU_TEXT        = QT_TRANSLATE_NOOP("Account", "Account");
    const char* const   RECEIPTS                = QT_TRANSLATE_NOOP("Account", "Receipts");
    const char* const   ADD_RECEIPTS            = QT_TRANSLATE_NOOP("Account", "Add receipts");
    const char* const   ACCOUNTBOOK_TEXT        = QT_TRANSLATE_NOOP("Account", "Account book");
    const char* const   REVENUES_TEXT           = QT_TRANSLATE_NOOP("Account", "Revenues");
    const char* const   EXPENSES_TEXT           = QT_TRANSLATE_NOOP("Account", "Expenses");
    const char* const   LEDGER                  = QT_TRANSLATE_NOOP("Account", "Ledger");
    const char* const   ASSETS                  = QT_TRANSLATE_NOOP("Account", "Assets");
    const char* const   MOVEMENTS               = QT_TRANSLATE_NOOP("Account", "Movements");
    const char* const   ACCOUNT                 = QT_TRANSLATE_NOOP("Account", "AccountView");

//    approche par les dépenses   n f spending-based approach
//    calendrier des dépenses   n m spending schedule
//    chiffre des dépenses en construction   n m construction spending figure
//    compression des dépenses   n f cut in spending
//    comptabilité recettes/dépenses   n f cash basis accounting
//    compte de recettes et de dépenses   n m income and expenditure account
//    dépenses alimentaires   n pl.f. food spending
//    dépenses budgétaires   n pl.f. budgetary expenditure
//    dépenses courantes   n pl.f. running costs
//    dépenses d'exploitation   n pl.f. operating expenses
//    dépenses d'investissement   n pl.f. investment expenditure
//    dépenses d'équipement   n pl.f. equipment spending
//    dépenses d'établissement   n pl.f. capital outlay
//    dépenses de fonctionnement   n pl.f. operational expenses
//    dépenses de l'Etat   n pl.f. Government expenditure
//    dépenses de la maison   n pl.f. household expenses
//    dépenses des ménages   n pl.f. household expenditure
//    dépenses et les recettes   n pl.f. expenditures and revenue
//    dépenses et recettes   n pl.f. expenditures and revenue
//    dépenses imprévues   n pl.f. contingencies
//    dépenses marketing   n pl.f. marketing expenses
//    dépenses militaires   n pl.f. military expenditures
//    dépenses prévues au budget   n pl.f. budgeted expenses
//    dépenses sociales   n pl.f. welfare expenditure
//    détail des dépenses   n m breakdown of expenses
//    famille de dépenses   n f family of expenses
//    frais et dépenses   n pl.m. costs and expenses
//    habitude de dépenses   n f spending habit
//    habitudes de dépenses   n
//    a f spending habit
//    b f spending habits
//    c pl.m. spending habits
//    d m spending habit
//    e pl.f. spending habits
//    responsable de famille de dépenses   n
//    a m expense family manager
//    b f expense family manager
//
//    Traduction dictionnaire Business Finance Francais - Anglais
//     Dictionnaire Collaboratif     Français-Anglais
//    dépenses	 n.	1) spending (pl.f.), 2) expenses (pl.f.)	 Reverso General Dictionary
//    dépense courante	 n.	running expense	 ~
//    dépense déductible	 n.	allowable expense	 ~
//    dépense d'investissement	 n.	capital spending	 ~
//    dépense publicitaire	 nm.	advertising spending	 Reverso General Dictionary
//    dépense de publicité	 n.	advertising expenditure	 ~
//    dépense en capital	 n.	capital expenditure	 ~
//    dépenses de consommation	 nmpl.	consumer spending	 Reverso General Dictionary
//    habitude de dépenses	 nm.	spending habit	 Reverso General Dictionary
//    plan de dépense	 n.	spending plan	 ~
//    poste de dépense	 n.	expense item	 ~
//    prévision de dépense	 n.	cost estimate	 ~
//    dépenses de consommation des ménages	 nmpl.	household consumer spending	 Reverso General Dictionary

//    const char* const   _TEXT   = QT_TRANSLATE_NOOP("Account", "");
//    const char* const   _TEXT   = QT_TRANSLATE_NOOP("Account", "");
//    const char* const   _TEXT   = QT_TRANSLATE_NOOP("Account", "");
//    const char* const   _TEXT   = QT_TRANSLATE_NOOP("Account", "");
//    const char* const   _TEXT   = QT_TRANSLATE_NOOP("Account", "");
//    const char* const   _TEXT   = QT_TRANSLATE_NOOP("Account", "");
//    const char* const   _TEXT   = QT_TRANSLATE_NOOP("Account", "");


    // Settings names
    const char * const  S_GROUP                  = "Account";                              /*!< \brief Key for settings. */
    const char * const  S_CONFIGURED             = "Account/Configured";                   /*!< \brief Key for settings. */
    const char * const  S_VIEWFONT               = "Account/view/Font";                    /*!< \brief Key for settings. */
    const char * const  S_VIEWFONTSIZE           = "Account/view/FontSize";                /*!< \brief Key for settings. */
    const char * const  S_INTERACTIONVIEW_ZOOM   = "Account/view/InteractionViewer/Zoom";  /*!< \brief Key for settings. */
    const char * const  S_DRUGFONT               = "Account/print/drug/Font";              /*!< \brief Key for settings. */
    const char * const  S_PRESCRIPTIONFONT       = "Account/print/prescription/Font";      /*!< \brief Key for settings. */

    const char * const  S_USER                   = "Account/user/Name";                    /*!< \brief Key for settings. */
    const char * const  S_PASSWORD               = "Account/user/Pass";                    /*!< \brief Key for settings. */
    const char * const  S_SERIAL                 = "Account/user/SN";                      /*!< \brief Key for settings. */
    const char * const  S_USER_HEADER            = "Account/user/htmlHeader";                 /*!< \brief Key for settings. */
    const char * const  S_USER_FOOTER            = "Account/user/htmlFooter";                 /*!< \brief Key for settings. */
    const char * const  S_USER_WATERMARK         = "Account/user/htmlWatermark";                 /*!< \brief Key for settings. */

    const char * const  S_HISTORYSIZE            = "Account/historySize";                  /*!< \brief Key for settings. */
    const char * const  S_DRUGHISTORY            = "Account/drugsHistory";                 /*!< \brief Key for settings. */

    const char * const  S_USERRECORDEDFORMS      = "Account/userRecordedForms";            /*!< \brief Key for settings. */

    const char * const  S_STATEPREFIX            = "Account/";


}  // End namespace Constants
}  // End namespace Account

#endif // ACCOUNT_CONSTANTS_H
