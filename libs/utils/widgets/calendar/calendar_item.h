#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <QString>
#include <QDateTime>

#include "common.h"

namespace Calendar {
	class CalendarItem
	{
	public:
		/** constructs an invalid item */
		CalendarItem() {}
		CalendarItem(const QString &uid, const QDateTime &beginning, const QDateTime &ending) :
			m_uid(uid), m_beginning(beginning), m_beginningType(Date_DateTime), m_ending(ending), m_endingType(Date_DateTime) {
			m_created = QDateTime::currentDateTime();
			m_lastModified = m_created;
		}

		bool isValid() const { return m_beginning.isValid(); }

		/** compute an intersection value with a day range
		 * returns:
		 * -1 if item is entirely before first day
		 * 0 if item intersects [firstDay, lastDay]
		 * 1 if item is entirely after lastDay
		 */
		int intersects(const QDate &firstDay, const QDate &lastDay) const;

		/** compute an overlap value with another item
		 * returns:
		 * false if items do not overlap
		 * true if items overlap
		 */
		bool overlap(const CalendarItem &item) const;

		// unicity
		const QString &uid() const { return m_uid; }

		const QString &title() const { return m_title; }
		void setTitle(const QString &value);

		const QDateTime &beginning() const { return m_beginning; }
		void setBeginning(const QDateTime &value);

		DateType beginningType() const { return m_beginningType; }
		void setBeginningType(DateType value);

		const QDateTime &ending() const { return m_ending; }
		void setEnding(const QDateTime &value);

		DateType endingType() const { return m_endingType; }
		void setEndingType(DateType value);

		const QString &description() const { return m_description; }
		void setDescription(const QString &value);

		const QDateTime &created() const { return m_created; }
		void setCreated(const QDateTime &value); // CAUTION: should not be called in normal times, only use it for hacking purpose

		const QDateTime &lastModified() const { return m_lastModified; }
		void setLastModified(const QDateTime &value); // CAUTION: should not be called in normal times, only use it for hacking purpose

		/** if true, beginning and ending date types are set to Date_Date. If false, beginning and ending date types are set to Date_DateTime */
		void setDaily(bool value);

	private:
		QString m_uid;
		QString m_title;
		QDateTime m_beginning;
		DateType m_beginningType;
		QDateTime m_ending;
		DateType m_endingType;
		QString m_description;
		QDateTime m_created;
		QDateTime m_lastModified;

		void touchLastModified();
	};

	/** a sort function for calendar items
	 */
	bool calendarItemLessThan(const CalendarItem &item1, const CalendarItem &item2);
}

#endif
