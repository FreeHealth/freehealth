#ifndef BASIC_CALENDAR_MODEL_H
#define BASIC_CALENDAR_MODEL_H

#include <QPair>

#include "calendar_item.h"
#include "abstract_calendar_model.h"

namespace Calendar {
	/**
	 * This model offers an optimized version of <getItemsBetween()> based on dichotomy method and double lists.
	 */
	class BasicCalendarModel : public AbstractCalendarModel
	{
		Q_OBJECT
	public:
		BasicCalendarModel(QObject *parent = 0);
		virtual ~BasicCalendarModel();

		QList<CalendarItem> getItemsBetween(const QDate &from, const QDate &to) const;

		int count() const { return m_sortedByBeginList.count(); }

		const CalendarItem &insertItem(const QDateTime &beginning, const QDateTime &ending);

		void setItemByUid(const QString &uid, const CalendarItem &item);

		CalendarItem getItemByUid(const QString &uid) const;

		void removeItem(const QString &uid);

		void clearAll();

	private:
		QList<CalendarItem*> m_sortedByBeginList;
		QList<CalendarItem*> m_sortedByEndList;

		// returns an insertion index for a datetime in <list> from <first> to <last> (dichotomy method)
		int getInsertionIndex(bool begin, const QDateTime &dateTime, const QList<CalendarItem*> &list, int first, int last) const;

		// search for an intersected item, the first found item is enough
		int searchForIntersectedItem(const QList<CalendarItem*> &list, const QDate &from, const QDate &to, int first, int last) const;

		// create a uniq uid
		QString createUid() const;

		CalendarItem *getItemPointerByUid(const QString &uid) const;
	};
}

#endif
