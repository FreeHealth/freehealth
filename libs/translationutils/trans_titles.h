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
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_TITLES_H
#define CONSTANTTRANSLATIONS_TITLES_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

enum TitleInterpret {
    NoTitle = 0,
    Mister,
    Miss,
    Madam,
    Doctor,
    Professor,
    Captain
};

// Genders
const char* const GENDER        = QT_TRANSLATE_NOOP("tkConstants", "Gender");
const char* const MALE          = QT_TRANSLATE_NOOP("tkConstants", "Male");
const char* const FEMALE        = QT_TRANSLATE_NOOP("tkConstants", "Female");
const char* const OTHER         = QT_TRANSLATE_NOOP("tkConstants", "Other");

// Titles && professions
const char* const MISTER      = QT_TRANSLATE_NOOP("tkConstants", "Mister");
const char* const MISS        = QT_TRANSLATE_NOOP("tkConstants", "Miss");
const char* const MADAM       = QT_TRANSLATE_NOOP("tkConstants", "Madam");
const char* const DOCTOR      = QT_TRANSLATE_NOOP("tkConstants", "Doctor");
const char* const PROFESSOR   = QT_TRANSLATE_NOOP("tkConstants", "Professor");
const char* const CAPTAIN     = QT_TRANSLATE_NOOP("tkConstants", "Captain");
const char* const NURSE       = QT_TRANSLATE_NOOP("tkConstants", "Nurse");
const char* const SECRETARY   = QT_TRANSLATE_NOOP("tkConstants", "Secretary");
const char* const CAREGIVER   = QT_TRANSLATE_NOOP("tkConstants", "Caregiver");
const char* const MEDICAL_STUDENT = QT_TRANSLATE_NOOP("tkConstants", "Medical student");
const char* const SOFT_ADMIN  = QT_TRANSLATE_NOOP("tkConstants", "Software administrator");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_TITLES_H
