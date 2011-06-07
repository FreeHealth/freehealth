#ifndef DAY_NODE_H
#define DAY_NODE_H

namespace Calendar {
	class CalendarItem;

	class DayNode
	{
	public:
		DayNode(const CalendarItem &item);
		~DayNode();

		/** store an item at the right place with a recursive method depending on date ranges
		 * \return {int} the depth of the node (0 -> N)
		 */
		int store(const CalendarItem &item);

	private:
		CalendarItem m_item;
		DayNode *m_right;
		DayNode *m_next;
	};
}

#endif
