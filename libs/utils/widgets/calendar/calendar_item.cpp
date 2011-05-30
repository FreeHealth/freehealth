#include "calendar_item.h"

using namespace Calendar;

void CalendarItem::setTitle(const QString &value) {
	if (value == m_title)
		return;

	m_title = value;
	touchLastModified();
}

void CalendarItem::touchLastModified() {
	m_lastModified = QDateTime::currentDateTime();
}

void CalendarItem::setBeginning(const QDateTime &value) {
	if (value == m_beginning)
		return;

	m_beginning = value;
	touchLastModified();
}

void CalendarItem::setBeginningType(DateType value) {
	if (value == m_beginningType)
		return;

	m_beginningType = value;
	if (m_beginningType == Date_Date)
		m_beginning.setTime(QTime(0, 0));
	touchLastModified();
}

void CalendarItem::setEnding(const QDateTime &value) {
	if (value == m_ending)
		return;

	m_ending = value;
	touchLastModified();
}

void CalendarItem::setEndingType(DateType value) {
	if (value == m_endingType)
		return;

	m_endingType = value;
	if (m_endingType == Date_Date)
		m_ending.setTime(QTime(0, 0));
	touchLastModified();
}

void CalendarItem::setDescription(const QString &value) {
	if (value == m_description)
		return;

	m_description = value;
	touchLastModified();
}

void CalendarItem::setCreated(const QDateTime &value) {
	if (value == m_created)
		return;

	m_created = value;
}

void CalendarItem::setLastModified(const QDateTime &value) {
	if (value == m_lastModified)
		return;

	m_lastModified = value;
}

void CalendarItem::setDaily(bool value) {
	DateType dateType = value ? Date_Date : Date_DateTime;

	if (m_beginningType == dateType && m_endingType == dateType)
		return;

	m_beginningType = dateType;
	m_endingType = dateType;
	touchLastModified();
}

int CalendarItem::intersects(const QDate &firstDay, const QDate &lastDay) const {
	if (m_ending.date() < firstDay ||
		(m_ending.date() == firstDay && m_ending.time() == QTime(0, 0)))
		return -1;
	if (m_beginning.date() > lastDay)
		return 1;

	return 0;
}

bool CalendarItem::overlap(const CalendarItem &item) const {
	return m_ending > item.beginning() && m_beginning < item.ending();
}

// at first compare with begin dates. If they're equals, compare by end dates.
bool Calendar::calendarItemLessThan(const CalendarItem &item1, const CalendarItem &item2) {
	// at first, compare the beginnings
	if (item1.beginning() < item2.beginning())
		return true;
	else if (item1.beginning() > item2.beginning())
		return false;
	// beginnings are the same => compare the endings
	else if (item1.ending() > item2.ending())
		return true;
	else if (item1.ending() < item2.ending())
		return false;
	// beginnings and endings are the same, compare the creation date time
	else if (item1.created() < item2.created())
		return true;
	else if (item1.created() > item2.created())
		return false;
	// create date time are the same => finally compare uid
	else
		return item1.uid() > item2.uid();
}
