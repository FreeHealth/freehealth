#ifndef BASIC_CALENDAR_MODEL_H
#define BASIC_CALENDAR_MODEL_H

#include <QPair>

#include "calendar_item.h"
#include "abstract_calendar_model.h"

namespace Calendar {
	class BasicCalendarModel : public AbstractCalendarModel
	{
		Q_OBJECT
	public:
		BasicCalendarModel(QObject *parent = 0);

		QList<CalendarItem> getItemsBetween(const QDate &from, const QDate &to) const;

		int count() const { return m_items.count(); }

		bool insertItem(const QDateTime &beginning, const QDateTime &ending);

	private:
		QList<CalendarItem> m_items;

		int firstIndexAfter(const QDate &day, const QPair<int,int> &range) const;
		int lastIndexBefore(const QDate &day, const QPair<int,int> &range) const;
	};
}

#endif
