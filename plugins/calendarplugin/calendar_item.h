#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <QWidget>
#include <QDateTime>

namespace Calendar {
	class CalendarItem : public QWidget
	{
		Q_OBJECT
	public:
		CalendarItem(QWidget *parent = 0);

		const QDateTime &beginDateTime() const { return m_beginDateTime; }
		void setBeginDateTime(const QDateTime &dateTime);
		const QDateTime &endDateTime() const { return m_endDateTime; }
		void setEndDateTime(const QDateTime &dateTime);

	protected:
		virtual void paintEvent(QPaintEvent *event);

	private:
		// TMP : all date will probably be moved into a pure data class for events/tasks, etc
		QDateTime m_beginDateTime;
		QDateTime m_endDateTime;
	};
}

#endif
