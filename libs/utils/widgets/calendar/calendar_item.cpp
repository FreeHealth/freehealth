#include "calendar_item.h"

using namespace Calendar;

void CalendarItem::setTitle(const QString &value) {
	if (value == m_title)
		return;

	m_title = value;
}

void CalendarItem::setBeginning(const QDateTime &value) {
	if (value == m_beginning)
		return;

	m_beginning = value;
}

void CalendarItem::setEnding(const QDateTime &value) {
	if (value == m_ending)
		return;

	m_ending = value;
}

void CalendarItem::setDescription(const QString &value) {
	if (value == m_description)
		return;

	m_description = value;
}

int CalendarItem::intersects(const QDate &firstDay, const QDate &lastDay) const {
	if (m_ending.date() < firstDay ||
		(m_ending.date() == firstDay && m_ending.time() == QTime(0, 0)))
		return -1;
	if (m_beginning.date() > lastDay)
		return 1;

	return 0;
}
