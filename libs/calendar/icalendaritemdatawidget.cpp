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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "icalendaritemdatawidget.h"

/*!
 * \class Calendar::ICalendarItemDataWidget
 * \brief generic interface to describe a calendar data widget.
 *
 * Widgets implementing this interface can be plugged into the calendar item editor dialog
 * and provide additional data to any calendar item.
 */


/*!
 * \fn virtual int Calendar::ICalendarItemDataWidget::insertionPlace() const = 0;
 * \returns a \sa ICalendarItemDataWidget::InsertionPlaces which describes where the
 * widget was inserted into the ui of the calendar item editor dialog.
*/

/*!
 * \fn virtual QWidget *Calendar::ICalendarItemDataWidget::createWidget(QWidget *parent = 0) = 0;
 * \brief Factory method to create the widget.
 */

/*!
 * \fn virtual bool Calendar::ICalendarItemDataWidget::setCalendarItem(const Calendar::CalendarItem &item) = 0;
 */

/*!
 * \fn virtual bool Calendar::ICalendarItemDataWidget::submitChangesToCalendarItem(Calendar::CalendarItem &item) = 0;
 */
