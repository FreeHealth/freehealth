#include "calendar_item.h"
#include "day_item_node.h"

using namespace Calendar;

DayItemNode::~DayItemNode() {
	// destroy recursively all the structure (siblings and children)
	if (m_right)
		delete m_right;
	if (m_next)
		delete m_next;
}

DayItemNode *DayItemNode::mostBottomNode() {
	DayItemNode *node = this;
	while (node->next())
		node = node->next();
	return node;
}

DayItemNode *DayItemNode::getNextCollidingNode(const CalendarItem &item) {
	DayItemNode *node = mostBottomNode();
	if (node->item().overlap(item))
		return node;

	if (node->right())
		return node->right()->getNextCollidingNode(item);

	if (node->colliding())
		return node->colliding()->getNextCollidingNode(item);

	return 0;
}

void DayItemNode::store(const CalendarItem &item) {
	DayItemNode *current = mostBottomNode();
	if (current->item().overlap(item)) {
		if (current->right())
			current->right()->store(item);
		else {
			if (current->colliding()) {
				if (current->colliding()->item().overlap(item)) {
					if (current->m_index + 1 >= current->colliding()->m_index) { // we reached the insertion count of node before the colliding one
						current->colliding()->store(item);
					} else // insert it
						current->m_right = new DayItemNode(item, current->m_colliding, current->m_index + 1);
				} else
					current->m_right = new DayItemNode(item, current->colliding()->getNextCollidingNode(item), current->m_index + 1);
			} else
				current->m_right = new DayItemNode(item, 0, current->m_index + 1);
		}
		return;
	}

	// non overlapping => will be the next item
	current->m_next = new DayItemNode(item, current->getNextCollidingNode(item), current->m_index);
}

int DayItemNode::computeMaxCount() {
	m_maxCount = 1 + (m_right ? m_right->computeMaxCount() : 0);
	return qMax(m_maxCount, m_next ? m_next->computeMaxCount() : 0);
}

int DayItemNode::computeMaxCountBeforeColliding() {
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

void DayItemNode::prepareForWidthsComputing() {
	computeMaxCount();
	computeMaxCountBeforeColliding();
}

void DayItemNode::computeWidths(int left, int width, QList<DayItemNode*> &list) {
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

