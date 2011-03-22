#ifndef CALENDAR_IMPL_H
#define CALENDAR_IMPL_H

#include <coreplugin/icalendar.h>

namespace Calendar {

	class CalendarImpl : public Core::ICalendar
	{
		Q_OBJECT
	public:
		CalendarImpl(QObject *parent = 0) : Core::ICalendar(parent) {}
		~CalendarImpl() {}

		QWidget *createCalendarWidget(QWidget *parent = 0);
	};

}

#endif
