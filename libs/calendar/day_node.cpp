#include "calendar_item.h"
#include "day_node.h"

using namespace Calendar;

int DayStore::store(const CalendarItem &item) {
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
