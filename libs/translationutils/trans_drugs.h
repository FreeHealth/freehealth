/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_DRUGS_H
#define CONSTANTTRANSLATIONS_DRUGS_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// Drugs
const char* const DRUGS_DATABASE                = QT_TRANSLATE_NOOP("tkConstants", "Drugs database");
const char* const INTERACTIONS_DATABASE         = QT_TRANSLATE_NOOP("tkConstants", "Interactions database");
const char* const DRUGS                         = QT_TRANSLATE_NOOP("tkConstants", "Drugs");
const char* const PROTOCOLS                     = QT_TRANSLATE_NOOP("tkConstants", "Protocols");
const char* const INN                           = QT_TRANSLATE_NOOP("tkConstants", "INN");
const char* const ATC                           = QT_TRANSLATE_NOOP("tkConstants", "ATC");
const char* const INTAKES                       = QT_TRANSLATE_NOOP("tkConstants", "intake(s)");
const char* const VIEWINTERACTIONS_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "View synthetic interactions");
const char* const ALLERGY_TO_1                  = QT_TRANSLATE_NOOP("tkConstants", "Allergy to %1");
const char* const INTOLERANCE_TO_1              = QT_TRANSLATE_NOOP("tkConstants", "Intolerance to %1");
const char* const NO_ALLERGIES_INTOLERANCES     = QT_TRANSLATE_NOOP("tkConstants", "No known allergies / intolerances");
const char* const KNOWN_ALLERGIES               = QT_TRANSLATE_NOOP("tkConstants", "Known allergies");
const char* const KNOWN_INTOLERANCES            = QT_TRANSLATE_NOOP("tkConstants", "Known intolerances");

// Drugs Interactions
const char* const INTERACTION                   = QT_TRANSLATE_NOOP("tkConstants", "Interaction");
const char* const TAKE_INTO_ACCOUNT             = QT_TRANSLATE_NOOP("tkConstants", "Take into account");
const char* const DISCOURAGED                   = QT_TRANSLATE_NOOP("tkConstants", "Discouraged");
const char* const CONTRAINDICATION              = QT_TRANSLATE_NOOP("tkConstants", "Contraindication");
const char* const P450_IAM                      = QT_TRANSLATE_NOOP("tkConstants", "Cytochrome P450 interaction");
const char* const GPG_IAM                       = QT_TRANSLATE_NOOP("tkConstants", "Glycoprotein-P interaction");
const char* const PRECAUTION_FOR_USE            = QT_TRANSLATE_NOOP("tkConstants", "Precaution for use");
const char* const DRUG_DUPLICATION              = QT_TRANSLATE_NOOP("tkConstants", "Drug duplication");
const char* const INN_DUPLICATION               = QT_TRANSLATE_NOOP("tkConstants", "Molecule duplication");
const char* const CLASS_DUPLICATION             = QT_TRANSLATE_NOOP("tkConstants", "Therapeutic class duplication");

// For datapack types
const char* const DRUGS_WITH_INTERACTIONS       = QT_TRANSLATE_NOOP("tkConstants", "Drug database with interaction database");
const char* const DRUGS_WITHOUT_INTERACTIONS    = QT_TRANSLATE_NOOP("tkConstants", "Drug database without interaction database");

} // end Constants
} // end TranslationUtils

#endif // TKCONSTANTTRANSLATIONS_H
