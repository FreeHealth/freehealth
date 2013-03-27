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
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "hour_range_node.h"
#include <calendar/modelanditem/calendar_item.h>

#include <QRect>

using namespace Calendar;

int HourRangeNode::m_hourHeight = 40;
int HourRangeNode::m_minimumItemHeight = 20;

void HourRangeNode::setHourHeight(int value) {
	if (m_hourHeight == value)
		return;

	m_hourHeight = value;
}

void HourRangeNode::setMinimumItemHeight(int value) {
	if (m_minimumItemHeight == value)
		return;

	m_minimumItemHeight = value;
}

HourRangeNode::~HourRangeNode() {
	// destroy recursively all the structure (siblings and children)
	if (m_right)
		delete m_right;
	if (m_next)
		delete m_next;
}

HourRangeNode *HourRangeNode::mostBottomNode() {
	HourRangeNode *node = this;
	while (node->next())
		node = node->next();
	return node;
}

HourRangeNode *HourRangeNode::getNextCollidingNode(const CalendarItem &item) {
	HourRangeNode *node = mostBottomNode();
//	if (node->item().overlap(item))
	if (overlap(node->item(), item))
		return node;

	if (node->right())
		return node->right()->getNextCollidingNode(item);

	if (node->colliding())
		return node->colliding()->getNextCollidingNode(item);

	return 0;
}

void HourRangeNode::store(const CalendarItem &item) {
	HourRangeNode *current = mostBottomNode();
//	if (current->item().overlap(item)) {
	if (overlap(current->item(), item)) {
		if (current->right())
			current->right()->store(item);
		else {
			if (current->colliding()) {
				if (overlap(current->colliding()->item(), item)) {
					if (current->m_index + 1 >= current->colliding()->m_index) { // we reached the insertion count of node before the colliding one
						current->colliding()->store(item);
					} else // insert it
						current->m_right = new HourRangeNode(item, current->m_colliding, current->m_index + 1);
				} else
					current->m_right = new HourRangeNode(item, current->colliding()->getNextCollidingNode(item), current->m_index + 1);
			} else
				current->m_right = new HourRangeNode(item, 0, current->m_index + 1);
		}
		return;
	}

	// non overlapping => will be the next item
	current->m_next = new HourRangeNode(item, current->getNextCollidingNode(item), current->m_index);
}

int HourRangeNode::computeMaxCount() {
	m_maxCount = 1 + (m_right ? m_right->computeMaxCount() : 0);
	return qMax(m_maxCount, m_next ? m_next->computeMaxCount() : 0);
}

int HourRangeNode::computeMaxCountBeforeColliding() {
	m_maxCountBeforeColliding = 1;

	if (m_right){
		int rightCount = m_right->computeMaxCountBeforeColliding();
		if (m_right->m_colliding == m_colliding)
			m_maxCountBeforeColliding += rightCount;
	}

	if (m_next) {
		int nextMaxCountBeforeColliding = m_next->computeMaxCountBeforeColliding();
		if (m_next->m_colliding == m_colliding)
			return qMax(m_maxCountBeforeColliding, nextMaxCountBeforeColliding);
	}
	return m_maxCountBeforeColliding;
}

void HourRangeNode::prepareForWidthsComputing() {
	computeMaxCount();
	computeMaxCountBeforeColliding();
}

void HourRangeNode::computeWidths(int left, int width, QList<HourRangeNode*> &list) {
	m_left = left;
	list << this;

	int collidingWidth = -1;

	// compute colliding width
	if (m_colliding)
		collidingWidth = (m_colliding->m_left - left) / m_maxCountBeforeColliding;
	m_width = width / m_maxCount;
	if (collidingWidth != -1 && collidingWidth < m_width)
		m_width = collidingWidth;
	if (m_right)
		m_right->computeWidths(m_left + m_width, width - m_width, list);
	if (m_next)
		m_next->computeWidths(m_left, width, list);
}

bool HourRangeNode::overlap(const CalendarItem &item1, const CalendarItem &item2) const {
	QPair<int, int> verti1 = getItemTopAndHeight(item1.beginning().time(), item1.ending().time(), m_hourHeight, m_minimumItemHeight);
	QPair<int, int> verti2 = getItemTopAndHeight(item2.beginning().time(), item2.ending().time(), m_hourHeight, m_minimumItemHeight);

	QRect rect1(0, verti1.first, 10, verti1.second);
	QRect rect2(0, verti2.first, 10, verti2.second);
	return rect1.intersects(rect2);
}
