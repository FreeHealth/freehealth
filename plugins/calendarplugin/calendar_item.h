#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <QWidget>

namespace Calendar {
	class CalendarItem : public QWidget
	{
	public:
		CalendarItem(QWidget *parent = 0);

	protected:
		virtual void paintEvent(QPaintEvent *event);
	};
}

#endif
