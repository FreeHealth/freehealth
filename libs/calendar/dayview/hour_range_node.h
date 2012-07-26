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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef LIBCALENDAR_HOUR_RANGE_NODE_H
#define LIBCALENDAR_HOUR_RANGE_NODE_H

#include <calendar/modelanditem/calendar_item.h>

/**
 * \file hour_range_node.h
 * \author Guillaume Denry <guillaume.denry@gmail.com>
 * \version 0.6.0
 * \date 12 Aug 2011
*/

namespace Calendar {

// this class is used to build a hierarchical structure of items for a day and to manage overlappings
// the algorithm behavior is inspired from google calendar
// A DayItemNode is associated with a calendar item.
// The right item is the item immediately at its right in the visual space and overlaps itself in time. All rights items are forming a chain.
// The next item is the item immediately following in time (so below in visual space)
// The colliding item is the first item which overlaps itself in time but is not a right. It belongs to a previous chain of items.
// Consequently, a node is responsible for freing its right and next nodes but not its colliding one (because it belongs to another chain)
class HourRangeNode
{
public:
    HourRangeNode(const CalendarItem &item, HourRangeNode *colliding = 0, int index = -1) : m_item(item), m_right(0), m_next(0), m_colliding(colliding), m_index(index), m_maxCount(0) {}
    ~HourRangeNode();

    const CalendarItem &item() const { return m_item; }			// the calendar item associates with the node
    int left() const { return m_left; }
    int width() const { return m_width; }

    // store an item at the right place with a recursive method depending on date ranges
    void store(const CalendarItem &item);

    // mandatory when we want to compute widths
    void prepareForWidthsComputing();

    // compute width of the node and all its neighbours and add them into a list
    void computeWidths(int left, int width, QList<HourRangeNode*> &list);

    // those parameters are used to know it two items are overlapping, even if they are not overlapping in time, but taking account of the minimum item height
    static void setHourHeight(int value);
    static void setMinimumItemHeight(int value);

private:
    static int m_hourHeight;
    static int m_minimumItemHeight;

    CalendarItem m_item;
    HourRangeNode *m_right;
    HourRangeNode *m_next;
    HourRangeNode *m_colliding;
    int m_index;
    int m_maxCount;
    int m_maxCountBeforeColliding;
    int m_left;
    int m_width;

    HourRangeNode *right() const { return m_right; }			// the node which is overlapping itself and shares horizontal space with itself and other overlapping sibling nodes
    HourRangeNode *next() const { return m_next; }			// the node which follows itself in the time but non overlapping (will be drawn at the bottom)
    HourRangeNode *colliding() const { return m_colliding; }	// the potential colliding node if it exists. It is always at the right position.
    int index() const { return m_index; }						// the right index of the node. Starts at 0.

    // returns the most bottom node (the most far next node)
    HourRangeNode *mostBottomNode();
    // returns the next colliding node with <item> potentially including <this>
    HourRangeNode *getNextCollidingNode(const CalendarItem &item);

    // returns the maximal count of right nodes
    int computeMaxCount();
    // returns the maximal count of right nodes before the colliding one
    int computeMaxCountBeforeColliding();

    // an overlap test function which takes in account the minimum item height
    bool overlap(const CalendarItem &item1, const CalendarItem &item2) const;
};
} // namespace Calendar

#endif  // LIBCALENDAR_HOUR_RANGE_NODE_H
