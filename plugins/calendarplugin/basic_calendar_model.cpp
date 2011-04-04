#include "basic_calendar_model.h"

using namespace Calendar;

BasicCalendarModel::BasicCalendarModel(QObject *parent) :
	AbstractCalendarModel(parent) {

	// TEMP just for tests
	QDate now = QDate::currentDate();
	m_items << CalendarItem(QDateTime(now, QTime(7, 0, 0)), QDateTime(now, QTime(8, 0, 0)));
	m_items << CalendarItem(QDateTime(now, QTime(10, 0, 0)), QDateTime(now, QTime(15, 45, 0)));
	m_items << CalendarItem(QDateTime(now.addDays(1), QTime(2, 0, 0)), QDateTime(now.addDays(1), QTime(13, 15, 0)));
}

QList<CalendarItem> BasicCalendarModel::getItemsBetween(const QDate &from, const QDate &to) const {
	Q_ASSERT_X(from <= to, "BasicCalendarModel::getItemsBetween", "<from> is strictly greater than <to>");

	QList<CalendarItem> list;

	// go through ALL items
	// WARNING: will be SLOW with a large number of items
	// There is maybe a way to optimize using dichotomy but it would needs a new organized data structure than a simple list
	foreach (const CalendarItem &item, m_items)
		if (!intersects(item, from, to))
			list << item;

	return list;
}

bool BasicCalendarModel::insertItem(const QDateTime &beginning, const QDateTime &ending) {
	beginInsertItem();

	CalendarItem item(beginning, ending);

	m_items << item;

	endInsertItem(item);
}
