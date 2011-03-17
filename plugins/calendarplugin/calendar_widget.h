#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QStackedWidget>

namespace Calendar {
	class CalendarWidget : public QStackedWidget
	{
		Q_OBJECT
	public:
		CalendarWidget(QWidget *parent = 0) : QStackedWidget(parent) {}
	};
}

#endif
