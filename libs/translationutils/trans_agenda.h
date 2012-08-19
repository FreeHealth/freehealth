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
#ifndef CONSTANTTRANSLATIONS_AGENDA_H
#define CONSTANTTRANSLATIONS_AGENDA_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// Agenda related
const char * const AGENDA = QT_TRANSLATE_NOOP("tkConstants", "Agenda");
const char * const AGENDA_EVENT = QT_TRANSLATE_NOOP("tkConstants", "Agenda event");
const char * const AGENDA_EVENTS = QT_TRANSLATE_NOOP("tkConstants", "Agenda events");
const char * const AGENDA_DATABASE_INFORMATION = QT_TRANSLATE_NOOP("tkConstants", "Agenda database information");
const char * const NO_AVAILABILITY = QT_TRANSLATE_NOOP("tkConstants", "No availability");
const char * const ATTENDEE = QT_TRANSLATE_NOOP("tkConstants", "Attendee");
const char * const ATTENDEES = QT_TRANSLATE_NOOP("tkConstants", "Attendees");
const char * const USER_DELEGATES = QT_TRANSLATE_NOOP("tkConstants", "User delegate");
const char * const OWNER = QT_TRANSLATE_NOOP("tkConstants", "Owner");
const char * const TODAY = QT_TRANSLATE_NOOP("tkConstants", "Today");
const char * const TOMORROW = QT_TRANSLATE_NOOP("tkConstants", "Tomorrow");
const char * const YESTERDAY = QT_TRANSLATE_NOOP("tkConstants", "Yesterday");
const char * const CURRENT_WEEK = QT_TRANSLATE_NOOP("tkConstants", "Current week");
const char * const NEXT_WEEK = QT_TRANSLATE_NOOP("tkConstants", "Next week");
const char * const PREVIOUS_WEEK = QT_TRANSLATE_NOOP("tkConstants", "Previous week");
const char * const CURRENT_MONTH = QT_TRANSLATE_NOOP("tkConstants", "Current month");
const char * const NEXT_MONTH = QT_TRANSLATE_NOOP("tkConstants", "Next month");
const char * const PREVIOUS_MONTH = QT_TRANSLATE_NOOP("tkConstants", "Previous month");

// Status
const char * const ARRIVED = QT_TRANSLATE_NOOP("tkConstants", "Arrived");
const char * const CANCELLED = QT_TRANSLATE_NOOP("tkConstants", "Cancelled");
const char * const MISSED = QT_TRANSLATE_NOOP("tkConstants", "Missed");
const char * const WAITING = QT_TRANSLATE_NOOP("tkConstants", "Waiting");
const char * const CHANGED = QT_TRANSLATE_NOOP("tkConstants", "Changed");
const char * const APPROVED = QT_TRANSLATE_NOOP("tkConstants", "Approved");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_AGENDA_H
