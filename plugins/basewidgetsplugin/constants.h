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
 *       Jerome Pinguet <jerome@jerome.cc>                                 *
 ***************************************************************************/
#ifndef BASEWIDGETS_CONSTANTS_H
#define BASEWIDGETS_CONSTANTS_H

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>

/**
 * \file baseformwidgets.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 07 Dec 2012
*/

namespace BaseWidgets {
namespace Constants {

// Extra-keys must be lowered +++
// See FormItem::addExtraData()
const char * const  EXTRAS_COUNTRY          = "country";
const char * const  EXTRAS_KEY              = "option";
const char * const  EXTRAS_KEY2             = "options";
const char * const  EXTRAS_KEY_COLUMN       = "column";
const char * const  EXTRAS_COMPACT_VIEW     = "compact";
const char * const  EXTRAS_GROUP_CHECKABLE  = "checkable";
const char * const  EXTRAS_GROUP_COLLAPSIBLE  = "collapsible";
const char * const  EXTRAS_GROUP_EXPANDED   = "expanded";
const char * const  EXTRAS_GROUP_CHECKED    = "checked";
const char * const  EXTRAS_ALIGN_VERTICAL   = "vertical";
const char * const  EXTRAS_ALIGN_HORIZONTAL = "horizontal";
const char * const  EXTRAS_LINEEDIT_VALIDATOR = "validator";
const char * const  EXTRAS_LINEEDIT_MASK      = "mask";

// Date options
const char * const  DATE_EXTRAS_KEY         = "dateformat";
const char * const  DATE_NOW                = "now";
const char * const  DATE_PATIENTLIMITS      = "patientLimits";

// Time options
const char * const  DATETIME_EXTRAS_KEY         = "datetimeformat";

// Default date
const char * const DEFAULT_DATE = "01011900";
const char * const DEFAULT_DATE_FORMAT = "ddMMyyyy";

// Default datetime
const char * const DEFAULT_DATETIME = "1900-01-01 00:00:00";
const char * const DEFAULT_DATETIME_FORMAT = "yyyy-MM-dd hh:mm:ss"; // default format of the default time
const char * const DATETIME_DEFAULT_FORMAT = "yyyy-MM-dd hh:mm:ss"; // default printing format for all DateTime widgets

// Spins
const char * const  SPIN_EXTRAS_KEY_MIN              = "min";
const char * const  SPIN_EXTRAS_KEY_MAX              = "max";
const char * const  SPIN_EXTRAS_KEY_STEP             = "step";
const char * const  SPIN_EXTRAS_KEY_SUFFIX           = "suffix";
const char * const  SPIN_EXTRAS_KEY_DECIMALS         = "decimals";
const char * const  SPIN_EXTRAS_KEY_SPECIALVALUETEXT = "special";

// General options
const char * const  LABEL_ALIGN_TOP      = "labelontop";
const char * const  LABEL_ALIGN_LEFT     = "labelonleft";
const char * const  DONTPRINTEMPTYVALUES = "DontPrintEmptyValues";
const char * const  DONTPRINTLABEL       = "DontPrintLabel";
const char * const  HIDEHEADERONUNIQUEEPISODE = "HideHeaderOnUniqueEpisode";
const char * const  NOT_PRINTABLE        = "notprintable";
const char * const  SUMMARY_FONT_BOLD    = "SummaryFontBold";

// Input validation limit
const int EPISODELABELMAXCHAR = 200; // maximum length of label string varchar(200)

Form::IFormWidget::LabelOptions labelAlignement(Form::FormItem *item, Form::IFormWidget::LabelOptions defaultValue = Form::IFormWidget::OnLeft);

QStringList getCountries(Form::FormItem *item);

int getNumberOfColumns(Form::FormItem *item, int defaultValue = 1);

int isCompactView(Form::FormItem *item, bool defaultValue = false);

int isGroupCheckable(Form::FormItem *item, bool defaultValue = false);

int isGroupCollapsible(Form::FormItem *item, bool defaultValue = false);

int isGroupExpanded(Form::FormItem *item, bool defaultValue = false);

int isGroupChecked(Form::FormItem *item, bool defaultValue = false);

int isRadioHorizontalAlign(Form::FormItem *item, bool defaultValue = true);

QString getDateFormat(Form::FormItem *item, const QString &defaultValue = "dd MM yyyy");
QString getDateTimeFormat(Form::FormItem *item, const QString &defaultValue = "yyyy-MM-dd HH:mm:ss");

bool dontPrintEmptyValues(Form::FormItem *item);

bool hideHeaderOnUniqueEpisode(Form::FormItem *item);

void executeOnValueChangedScript(Form::FormItem *item);

void executeOnToggledScript(Form::FormItem *item);

QLabel *findLabel(Form::FormItem *item);

bool hasOption(Form::FormItem *item, const QString &option);

}  // namespace Constants
}  // namespace BaseWidgets

#endif // BASEWIDGETS_CONSTANTS_H
