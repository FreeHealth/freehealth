#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QAbstractScrollArea>

#include "view.h"

namespace Calendar {
	class CalendarWidget : public QAbstractScrollArea
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
		virtual void paintEvent(QPaintEvent *event);
		virtual void resizeEvent(QResizeEvent *event);

	private:
		ViewType m_viewType;
		View *m_view;

		void computeGeometries();
	};
}

#endif
