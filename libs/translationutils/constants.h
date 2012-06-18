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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_GLOBAL_H
#define CONSTANTTRANSLATIONS_GLOBAL_H

#include <translationutils/translationutils_exporter.h>

#include <QString>
#include <QStringList>
#include <QCoreApplication>

namespace Trans {
namespace Constants {

const char* const CONSTANTS_TR_CONTEXT = "tkConstants";
const char* const CONSTANTS_TRANSLATOR_NAME = "translationutils";

// For multilingual class template
const char* const ALL_LANGUAGE = "xx";
const char* const ALL_LANGUAGE_TEXT = QT_TRANSLATE_NOOP("tkConstants", "All languages");
const char* const ALL_OTHER_LANGUAGES_TEXT = QT_TRANSLATE_NOOP("tkConstants", "All other languages");

// App dateFormat for editors

//TODO: Why is this hardcoded as "MM dd yyyy" which is Canadian, and not taken from the system settings?
//: This date format will be used in every QDateEdit only
const char* const DATEFORMAT_FOR_EDITOR = QT_TRANSLATE_NOOP("tkConstants", "MM dd yyyy");
const char* const DATEFORMAT_FOR_MODEL = QT_TRANSLATE_NOOP("tkConstants", "MM-dd-yyyy");

//: This date time format will be used in every QDateTimeEdit only
const char* const DATETIMEFORMAT_FOR_EDITOR = QT_TRANSLATE_NOOP("tkConstants", "MM dd yyyy hh:mm");

//: This time format will be used in every QTimeEdit only
const char* const TIMEFORMAT_FOR_EDITOR = QT_TRANSLATE_NOOP("tkConstants", "hh:mm");

} // end Constants


namespace ConstantTranslations {

TRUTILS_EXPORT QString tkTr(const char* toTr, const int plurials=1);

TRUTILS_EXPORT QString checkUpdateLabel(const int index);
TRUTILS_EXPORT QStringList checkUpdateLabels();

TRUTILS_EXPORT QStringList periods();
TRUTILS_EXPORT QString period(int id);
TRUTILS_EXPORT QString periodPlurialForm(int id, int nb = 1, const QString &defaultValue = QString::null);
TRUTILS_EXPORT QStringList dailySchemeList();
TRUTILS_EXPORT QStringList dailySchemeXmlTagList();
TRUTILS_EXPORT QString dailyScheme(const int dailySchemeFlag);
TRUTILS_EXPORT QStringList preDeterminedAges();
TRUTILS_EXPORT QStringList mealTime();
TRUTILS_EXPORT QString mealTime(const int index);

TRUTILS_EXPORT QStringList genders();
TRUTILS_EXPORT QStringList titles();

} // end  ConstantTranslations

} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_GLOBAL_H
