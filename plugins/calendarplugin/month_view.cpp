#include "month_view.h"

using namespace Calendar;

int MonthView::topHeaderHeight() const {
	return 0;
}

int MonthView::leftHeaderWidth() const {
	return 0;
}

QDate MonthView::getFirstDateByRandomDate(const QDate &randomDate) {
	QDate date = QDate::currentDate();
	return date.addDays(- date.day() + 1);
}
