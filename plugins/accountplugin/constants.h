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
#ifndef ACCOUNT_CONSTANTS_H
#define ACCOUNT_CONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.1.0
 * \date 18 Mar 2010
 * \brief Some constants for Accountancy management
 * \ingroup freeaccount account
*/
namespace Account {
namespace Constants {

    // Menu and action for ActionManager
    const char * const  DRUGCONSTANTS_TR_CONTEXT = "Account";
    const char * const  M_PLUGINS_DRUGS         =  "Acc.menuDrugs";
    const char * const  M_PLUGINS_SEARCH        =  "Acc.menuSearch";
    const char * const  M_PLUGINS_MODES         =  "Acc.menuModes";

    const char * const  G_PLUGINS_DRUGS         =  "Acc.groupDrugs";
    const char * const  G_PLUGINS_SEARCH        =  "Acc.groupSearch";
    const char * const  G_PLUGINS_MODES         =  "Acc.groupModes";

    const char * const  C_DRUGS_PLUGINS         =  "Acc.ContextDrugs";
    const char * const  C_DRUGS_SEARCH          =  "Acc.ContextSearch";

    const char* const   A_ACCOUNT_BOOK          = "a.Acc.AccBook";
    const char* const   A_REVENUES              = "a.Acc.Revenues";
    const char* const   A_EXPENSES              = "a.Acc.Expenses";

    const char* const   ACCOUNTBOOK_TEXT        = QT_TRANSLATE_NOOP("Account", "Account book");
    const char* const   REVENUES_TEXT           = QT_TRANSLATE_NOOP("Account", "Revenues");
    const char* const   EXPENSES_TEXT           = QT_TRANSLATE_NOOP("Account", "Expenses");


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
