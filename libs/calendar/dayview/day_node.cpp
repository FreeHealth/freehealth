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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "day_node.h"
#include <calendar/modelanditem/calendar_item.h>

using namespace Calendar;
using namespace Internal;

int DayStore::store(const CalendarItem &item)
{
	int depth = 0;
	for (; depth < m_items.count(); depth++) {
		QList<CalendarItem> &layer = m_items[depth];

		if (layer.isEmpty()) { // no items in the layer => the argument will be the first
			layer << item;
			return depth;
		}

		const CalendarItem &last = layer.last();
		if (!last.overlap(item)) { // no overlapping => just append it to the queue
			layer << item;
			return depth;
		}
	}

	depth  = m_items.isEmpty() ? 0 : depth;
	QList<CalendarItem> layer;
	layer << item;
	m_items << layer;
	return depth;
}
