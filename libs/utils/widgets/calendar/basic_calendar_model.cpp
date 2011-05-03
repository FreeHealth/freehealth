#include <QMap>

#include "basic_calendar_model.h"

using namespace Calendar;

BasicCalendarModel::~BasicCalendarModel() {
	// pointers are duplicated between m_sortedByBeginList and m_sortedByEndList so delete them once
	qDeleteAll(m_sortedByBeginList);
}

BasicCalendarModel::BasicCalendarModel(QObject *parent) :
	AbstractCalendarModel(parent) {

	// TEMP just for tests
	QDate now = QDate::currentDate();
	insertItem(QDateTime(now, QTime(10, 0)), QDateTime(now, QTime(12, 0)));
	insertItem(QDateTime(now, QTime(10, 0)), QDateTime(now, QTime(12, 0)));
	insertItem(QDateTime(now, QTime(10, 0)), QDateTime(now, QTime(12, 0)));
	insertItem(QDateTime(now, QTime(10, 30)), QDateTime(now, QTime(13, 0)));
	insertItem(QDateTime(now, QTime(12, 0)), QDateTime(now, QTime(14, 0)));
	insertItem(QDateTime(now, QTime(14, 0)), QDateTime(now, QTime(15, 0)));
	insertItem(QDateTime(now, QTime(14, 0)), QDateTime(now, QTime(15, 0)));
	insertItem(QDateTime(now, QTime(14, 0)), QDateTime(now, QTime(15, 0)));
	insertItem(QDateTime(now, QTime(14, 0)), QDateTime(now, QTime(15, 0)));
}

QList<CalendarItem> BasicCalendarModel::getItemsBetween(const QDate &from, const QDate &to) const {
	Q_ASSERT_X(from <= to, "BasicCalendarModel::getItemsBetween", "<from> is strictly greater than <to>");

	QList<CalendarItem> list;
	QMap<CalendarItem*, bool> added;

	int pivot = searchForIntersectedItem(m_sortedByBeginList, from, to, 0, m_sortedByBeginList.count() - 1);

	if (pivot == -1)
		return list;

	list << *m_sortedByBeginList[pivot];
	added.insert(m_sortedByBeginList[pivot], true);

	// search backward...
	for (int i = pivot - 1; i >= 0; i--) {
		if (m_sortedByBeginList[i]->intersects(from, to))
			break;
		list << *m_sortedByBeginList[i];
		added.insert(m_sortedByBeginList[i], true);
	}

	// ... and forward
	for (int i = pivot + 1; i < m_sortedByBeginList.count(); i++) {
		if (m_sortedByBeginList[i]->intersects(from, to))
			break;
		list << *m_sortedByBeginList[i];
		added.insert(m_sortedByBeginList[i], true);
	}

	// now, recompute the pivot and search in sorted by ending
	pivot = m_sortedByEndList.indexOf(m_sortedByBeginList[pivot]);

	// search backward...
	for (int i = pivot - 1; i >= 0; i--) {
		if (added[m_sortedByEndList[i]])
			continue;
		if (m_sortedByEndList[i]->intersects(from, to))
			break;
		list << *m_sortedByEndList[i];
	}

	// ... and forward
	for (int i = pivot + 1; i < m_sortedByEndList.count(); i++) {
		if (added[m_sortedByEndList[i]])
			continue;
		if (m_sortedByEndList[i]->intersects(from, to))
			break;
		list << *m_sortedByEndList[i];
	}


	return list;
}

bool BasicCalendarModel::insertItem(const QDateTime &beginning, const QDateTime &ending) {
	beginInsertItem();

	// create the item once but insert it in two lists
	CalendarItem *item = new CalendarItem(createUid(), beginning, ending);

	m_sortedByBeginList.insert(getInsertionIndex(true, beginning, m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), item);
	m_sortedByEndList.insert(getInsertionIndex(false, ending, m_sortedByEndList, 0, m_sortedByEndList.count() - 1), item);

	endInsertItem(*item);

	return true;
}

int BasicCalendarModel::getInsertionIndex(bool begin, const QDateTime &dateTime, const QList<CalendarItem*> &list, int first, int last) const {
	if (last == - 1) // 0 items
		return 0;

	if (first == last) { // only one item, left or right?
		const QDateTime &curDateTime = begin ? list[first]->beginning() : list[first]->ending();
		if (dateTime < curDateTime)
			return first;
		else
			return first + 1;
	}

	// at least two items
	int middle = first + (last - first) / 2;
	const QDateTime &curDateTime = begin ? list[middle]->beginning() : list[middle]->ending();
	if (dateTime < curDateTime)
		return getInsertionIndex(begin, dateTime, list, first, middle); // insertion index is in left part
	return getInsertionIndex(begin, dateTime, list, middle + 1, last); // insertion index is in right part
}

int BasicCalendarModel::searchForIntersectedItem(const QList<CalendarItem*> &list, const QDate &from, const QDate &to, int first, int last) const {
	if (last == -1) // 0 items
		return 0;

	if (first == last) { // only one item, left or right?
		if (list[first]->intersects(from, to))
			return -1;
		return first;
	}

	// at least two items
	int middle = first + (last - first) / 2;
	int v = list[middle]->intersects(from, to);
	if (v > 0)
		return searchForIntersectedItem(list, from, to, first, middle); // if an intersecting item really exists, it will be in the left part
	else if (v < 0)
		return searchForIntersectedItem(list, from, to, middle + 1, last); // if an intersecting item really exists, it will be in the right part

	return middle; // intersection => found!
}

QString BasicCalendarModel::createUid() const {
	// at first, get the date
	QString nowStr = QDate::currentDate().toString("yyyyMMdd");
	int index = 0;
	QString propal;
	do {
		if (!index)
			propal = nowStr;
		else
			propal = nowStr + QString("-%1").arg(index);
		index++;
	} while (getItemByUid(propal).isValid());
	return propal;
}

CalendarItem BasicCalendarModel::getItemByUid(const QString &uid) const {
	CalendarItem *item = getItemPointerByUid(uid);
	return item ? *item : CalendarItem();
}

CalendarItem *BasicCalendarModel::getItemPointerByUid(const QString &uid) const {
	// TODO: optimize it
	foreach (CalendarItem *item, m_sortedByBeginList)
		if (item->uid() == uid)
			return item;
	return 0;
}

void BasicCalendarModel::setItemByUid(const QString &uid, const CalendarItem &item) {
	// remove the old item
	CalendarItem *oldItem = getItemPointerByUid(uid);
	if (!oldItem)
		return;

	beginModifyItem();

	m_sortedByBeginList.removeAt(m_sortedByBeginList.indexOf(oldItem));
	m_sortedByEndList.removeAt(m_sortedByEndList.indexOf(oldItem));

	// create the item once but insert it in two lists
	CalendarItem *pItem = new CalendarItem(item);

	m_sortedByBeginList.insert(getInsertionIndex(true, item.beginning(), m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), pItem);
	m_sortedByEndList.insert(getInsertionIndex(false, item.ending(), m_sortedByEndList, 0, m_sortedByEndList.count() - 1), pItem);

	endModifyItem(*oldItem, *pItem);

	delete oldItem;
}
