#include "view.h"

using namespace Calendar;

void View::setFirstDate(const QDate &firstDate) {
	if (m_firstDate == firstDate)
		return;

	m_firstDate = firstDate;
	update();
}
