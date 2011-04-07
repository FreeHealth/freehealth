#ifndef ABSTRACT_CALENDAR_MODEL_H
#define ABSTRACT_CALENDAR_MODEL_H

#include <QObject>

#include "calendar_item.h"

namespace Calendar {
	class AbstractCalendarModel : public QObject
	{
		Q_OBJECT
	public:
		AbstractCalendarModel(QObject *parent = 0);

		/** returns all calendar items between two days */
		virtual QList<CalendarItem> getItemsBetween(const QDate &from, const QDate &to) const = 0;

		/** returns the total items count */
		virtual int count() const = 0;

		/** \brief insert an item with begin and end dates
		 * The base class function does nothing.
		 * \return false if the insertion occured
		 */
		virtual bool insertItem(const QDateTime &begin, const QDateTime &end);

	signals:
		void itemInserted(const CalendarItem &newItem);

	protected:
		/** this function must be called before any item insertion */
		void beginInsertItem();
		/** this function must be called after any item insertion */
		void endInsertItem(const CalendarItem &newItem);
		/**
		 * returns :
		 * -1 if item is entirely before first day
		 * 0 if item intersects [firstDay, lastDay]
		 * 1 if item is entirely after lastDay
		 */
		int intersects(const CalendarItem &item, const QDate &firstDay, const QDate &lastDay) const;

	private:
	};
}

 #endif
