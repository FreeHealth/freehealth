#include "view.h"

using namespace Calendar;

void View::setFirstDate(const QDate &date) {
	if (m_firstDate == date)
		return;

	m_firstDate = date;

	emit firstDateChanged();
}
