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

		/** return an item by an uid */
		virtual CalendarItem getItemByUid(const QString &uid) const = 0;

		/** returns all calendar items between two days */
		virtual QList<CalendarItem> getItemsBetween(const QDate &from, const QDate &to) const = 0;

		/** returns the total items count */
		virtual int count() const = 0;

		/** \brief insert an item with begin and end dates
		 * The base class function does nothing.
		 * \return false if the insertion occured
		 */
		virtual bool insertItem(const QDateTime &begin, const QDateTime &end);

		/**
		 * Set a new calendar item for an uid
		 */
		virtual void setItemByUid(const QString &uid, const CalendarItem &item);

		/**
		 * Remove a calendar item in function of a uid
		 */
		virtual void removeItem(const QString &uid) = 0;

	signals:
		void itemInserted(const CalendarItem &newItem);
		void itemModified(const CalendarItem &oldItem, const CalendarItem &newItem);
		void itemRemoved(const CalendarItem &removedItem);

	protected:
		/** this function must be called before any item insertion */
		void beginInsertItem();
		/** this function must be called after any item insertion */
		void endInsertItem(const CalendarItem &newItem);
		/** this function must be called before any item modification */
		void beginModifyItem();
		/** this function must be called after any item modification */
		void endModifyItem(const CalendarItem &oldItem, const CalendarItem &newItem);
		/** this function must be called before any item deletion */
		void beginRemoveItem();
		/** this function must be called after any item deletion */
		void endRemoveItem(const CalendarItem &removedItem);
	};
}

 #endif
