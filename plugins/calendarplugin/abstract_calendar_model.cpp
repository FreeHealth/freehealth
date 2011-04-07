#include "abstract_calendar_model.h"

using namespace Calendar;

AbstractCalendarModel::AbstractCalendarModel(QObject *parent) :
	QObject(parent) {
}

int AbstractCalendarModel::intersects(const CalendarItem &item, const QDate &firstDay, const QDate &lastDay) const {
	if (item.ending().date() < firstDay ||
		item.ending().date() == firstDay && item.ending().time() == QTime(0, 0))
		return -1;
	if (item.beginning().date() > lastDay)
		return 1;

	return 0;
}

bool AbstractCalendarModel::insertItem(const QDateTime &, const QDateTime &) {
	return false;
}

void AbstractCalendarModel::beginInsertItem() {
	// does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endInsertItem(const CalendarItem &newItem) {
	emit itemInserted(newItem);
}
