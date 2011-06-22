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
	CalendarItem item;
/*	qsrand(time(0));
	QTime t;
	t.start();
	stopEvents();
	for (int i = 0; i < 300; i++) {
		QDateTime start(now, QTime(qrand() % 24, qrand() % 60));
		QDateTime end = start.addSecs(3600 * (1 + qrand() % 5));
		insertItem(start, end);
	}
	resumeEvents();
	qDebug("elapsed: %d", t.elapsed());*/

	item = insertItem(QDateTime(now), QDateTime(now.addDays(1)));
	item.setDaily(true);
	item.setTitle("Another brick in the wall");
	setItemByUid(item.uid(), item);
	item = insertItem(QDateTime(now), QDateTime(now.addDays(2)));
	item.setDaily(true);
	setItemByUid(item.uid(), item);
	item = insertItem(QDateTime(now.addDays(1)), QDateTime(now.addDays(3)));
	item.setDaily(true);
	setItemByUid(item.uid(), item);
	item = insertItem(QDateTime(now.addDays(2)), QDateTime(now.addDays(5)));
	item.setDaily(true);
	item.setTitle("Ceci est un exemple");
        setItemByUid(item.uid(), item);

        QDateTime cd = QDateTime::currentDateTime();
        cd.setTime(QTime(cd.time().hour(), 00, 00));
        for(int i = 0 ; i< 300; ++i) {
            insertItem(cd, cd.addSecs(15*60));
            cd = cd.addSecs(15*60);
        }
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

const CalendarItem &BasicCalendarModel::insertItem(const QDateTime &beginning, const QDateTime &ending) {
	if (m_propagateEvents)
		beginInsertItem();

	// create the item once but insert it in two lists
	CalendarItem *item = new CalendarItem(createUid(), beginning, ending);

	m_sortedByBeginList.insert(getInsertionIndex(true, beginning, m_sortedByBeginList, 0, m_sortedByBeginList.count() - 1), item);
	m_sortedByEndList.insert(getInsertionIndex(false, ending, m_sortedByEndList, 0, m_sortedByEndList.count() - 1), item);

	if (m_propagateEvents)
		endInsertItem(*item);

	return *item;
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
		return -1;

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

/* TODO: optimize this function */
QString BasicCalendarModel::createUid() const {
	// at first, get the date
	QString nowStr = QDateTime::currentDateTime().toString("yyyyMMddThhmmsszz");
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

void BasicCalendarModel::removeItem(const QString &uid) {
	// remove the old item
	CalendarItem *oldItem = getItemPointerByUid(uid);
	if (!oldItem)
		return;

	beginRemoveItem();

	m_sortedByBeginList.removeAt(m_sortedByBeginList.indexOf(oldItem));
	m_sortedByEndList.removeAt(m_sortedByEndList.indexOf(oldItem));

	endRemoveItem(*oldItem);

	delete oldItem;
}

void BasicCalendarModel::clearAll() {
	qDeleteAll(m_sortedByBeginList);
	m_sortedByBeginList.clear();
	m_sortedByEndList.clear();
	if (m_propagateEvents)
		emit reset();
}
