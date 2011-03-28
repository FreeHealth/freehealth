#include "view.h"

using namespace Calendar;

void View::setDate(const QDate &date) {
	QDate firstDate = getFirstDateByRandomDate(date);
	if (m_firstDate == firstDate)
		return;

	m_firstDate = firstDate;
	update();

	emit firstDateChanged();
}

void View::todayPage() {
	setDate(QDate::currentDate());
}
