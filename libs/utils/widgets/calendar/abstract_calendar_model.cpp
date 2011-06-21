#include "abstract_calendar_model.h"

using namespace Calendar;

AbstractCalendarModel::AbstractCalendarModel(QObject *parent) :
	QObject(parent), m_propagateEvents(true) {
}

CalendarItem AbstractCalendarModel::insertItem(const QDateTime &, const QDateTime &) {
	return CalendarItem();
}

void AbstractCalendarModel::beginInsertItem() {
	// does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endInsertItem(const CalendarItem &newItem) {
	if (m_propagateEvents)
		emit itemInserted(newItem);
}

void AbstractCalendarModel::beginModifyItem() {
	// does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endModifyItem(const CalendarItem &oldItem, const CalendarItem &newItem) {
	if (m_propagateEvents)
		emit itemModified(oldItem, newItem);
}

void AbstractCalendarModel::beginRemoveItem() {
	// does nothing for now (maybe emit a signal in the future)
}

void AbstractCalendarModel::endRemoveItem(const CalendarItem &removedItem) {
	if (m_propagateEvents)
		emit itemRemoved(removedItem);
}

void AbstractCalendarModel::setItemByUid(const QString &, const CalendarItem &) {}

void AbstractCalendarModel::stopEvents() {
	m_propagateEvents = false;
}

void AbstractCalendarModel::resumeEvents() {
	m_propagateEvents = true;
	emit reset();
}

