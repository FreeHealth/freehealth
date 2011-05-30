#include "abstract_calendar_model.h"

using namespace Calendar;

AbstractCalendarModel::AbstractCalendarModel(QObject *parent) :
	QObject(parent) {
}

CalendarItem AbstractCalendarModel::insertItem(const QDateTime &, const QDateTime &) {
	return CalendarItem();
}

void AbstractCalendarModel::beginInsertItem() {
	// does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endInsertItem(const CalendarItem &newItem) {
	emit itemInserted(newItem);
}

void AbstractCalendarModel::beginModifyItem() {
	// does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endModifyItem(const CalendarItem &oldItem, const CalendarItem &newItem) {
	emit itemModified(oldItem, newItem);
}

void AbstractCalendarModel::beginRemoveItem() {
	// does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endRemoveItem(const CalendarItem &removedItem) {
	emit itemRemoved(removedItem);
}

void AbstractCalendarModel::setItemByUid(const QString &, const CalendarItem &) {}
