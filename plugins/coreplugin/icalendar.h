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
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 ***************************************************************************/

#ifndef ICALENDAR_H
#define ICALENDAR_H

#include <QObject>
#include <QString>
#include <QWidget>

#include <coreplugin/core_exporter.h>

class QTextEdit;

/**
 * \file icalendar.h
 * \author Guillaume DENRY <guillaume.denry@gmail.com>
 * \version 0.5.0
 * \date 17 Mar 2011
 */

namespace Core {
	/**
       \brief Use this class to avoid any plugin dependencies (other than Core), when needing to access the \e current \e pad data.
	*/
	class CORE_EXPORT ICalendar : public QObject
	{
        Q_OBJECT

	public:
		ICalendar(QObject * parent = 0) : QObject(parent) {}
		virtual ~ICalendar() {}

		/**
		 * \brief returns a new calendar widget
		 */
		virtual QWidget *createCalendarWidget(QWidget *parent = 0) = 0;
	};
}

#endif
