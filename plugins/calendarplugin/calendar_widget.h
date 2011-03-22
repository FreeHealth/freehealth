#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QScrollArea>

#include "view.h"

namespace Calendar {
	class CalendarWidget : public QScrollArea
	{
		Q_OBJECT
	public:
		enum ViewType {
			View_Day,
			View_Week,
			View_Month
		};

		CalendarWidget(QWidget *parent = 0);

		ViewType viewType() const { return m_viewType; }
		void setViewType(ViewType value);

	protected:
		void resizeEvent(QResizeEvent *event);

	private:
		ViewType m_viewType;
		View *m_view;
		QWidget *m_header;
	};
}

#endif
