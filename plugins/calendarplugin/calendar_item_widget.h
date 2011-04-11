#ifndef CALENDAR_ITEM_WIDGET_H
#define CALENDAR_ITEM_WIDGET_H

#include <QWidget>
#include <QDateTime>

namespace Calendar {
	class CalendarItemWidget : public QWidget
	{
		Q_OBJECT
	public:
		/** if uid is empty, this calendar item is considered as temporary and will be drawn with transparence
		 */
		CalendarItemWidget(QWidget *parent = 0, const QString &uid = "");

		const QString &uid() const { return m_uid; }
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
		QString m_uid;
	};
}

#endif
