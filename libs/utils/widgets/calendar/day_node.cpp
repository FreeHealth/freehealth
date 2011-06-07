#include "calendar_item.h"
#include "day_node.h"

using namespace Calendar;

DayNode::DayNode(const CalendarItem &item)
	: m_item(item), m_right(0), m_next(0) {
}

DayNode::~DayNode() {
	if (m_right)
		delete m_right;
	if (m_next)
		delete m_next;
}

int DayNode::store(const CalendarItem &item) {
	if (m_item.overlap(item)) {
		if (m_next)
			return m_next->store(item) + 1;
		else {
			m_next = new DayNode(item);
			return 1;
		}
	} else {
		if (m_right)
			return m_right->store(item);
		else {
			m_right = new DayNode(item);
			return 0;
		}
	}
}
