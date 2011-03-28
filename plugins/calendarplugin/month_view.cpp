#include "month_view.h"
#include "week_view.h" // TMP

using namespace Calendar;

int MonthView::topHeaderHeight() const {
	return 0;
}

int MonthView::leftHeaderWidth() const {
	return 0;
}

QWidget *MonthView::createHeaderWidget(QWidget *parent) {
	WeekHeader *widget = new WeekHeader(parent);
	widget->setFirstDate(m_firstDate);
	return widget;
}
