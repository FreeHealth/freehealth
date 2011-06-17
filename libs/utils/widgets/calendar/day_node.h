#ifndef DAY_NODE_H
#define DAY_NODE_H

namespace Calendar {
	class CalendarItem;

	class DayStore
	{
	public:
		/** store an item at the right place with a recursive method depending on date ranges
		 * \return {int} the depth of the node (0 -> N)
		 */
		int store(const CalendarItem &item);

	private:
		QList<QList<CalendarItem> > m_items; // contains all layers of items m_items[0] is the first layer, m_items[1] is the second, etc
	};
}

#endif
