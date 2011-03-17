#include <QGraphicsView>

#include "calendarimpl.h"
#include "calendar_widget.h"

using namespace Calendar;

QWidget *CalendarImpl::createCalendarWidget(QWidget *parent) {
	return new CalendarWidget(parent);
}
