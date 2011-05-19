#ifndef MONTH_DAY_WIDGET_H
#define MONTH_DAY_WIDGET_H

#include <QWidget>
#include <QDate>

#include "abstract_calendar_model.h"
#include "calendar_item.h"

namespace Calendar {
	class MonthDayWidget : public QWidget {
		Q_OBJECT
	public:
		MonthDayWidget(AbstractCalendarModel *model, const QDate &day, QWidget *parent = 0);

		const QDate &day() const { return m_day; }

	protected:
		virtual void resizeEvent(QResizeEvent *event);

	private:
		AbstractCalendarModel *m_model;
		QDate m_day;
		QList<CalendarItem> m_items;

		void refreshItems();
	};
}

#endif
