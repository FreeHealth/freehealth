#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QScrollArea>
#include <QTimer>

#include "common.h"
#include "view.h"
#include "abstract_calendar_model.h"

class QVBoxLayout;

namespace Calendar {
	struct CalendarWidgetPrivate;

	class CalendarWidget : public QWidget
	{
		Q_OBJECT
	public:
		CalendarWidget(QWidget *parent = 0);

		AbstractCalendarModel *model() const { return m_model; }
		void setModel(AbstractCalendarModel *model);

		ViewType viewType() const;
		void setViewType(Calendar::ViewType viewType);

	private slots:
		// navigation bar slots
		void firstDateChanged();
		void viewTypeChanged();

		// timer
		void timeout();

	private:
		CalendarWidgetPrivate *m_d;
		ViewType m_viewType;
		AbstractCalendarModel *m_model;
	};
}

#endif
