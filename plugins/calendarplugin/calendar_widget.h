#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QScrollArea>

#include "view.h"
#include "calendar_model.h"

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

		CalendarModel *model() const { return m_model; }
		void setModel(CalendarModel *model);

	protected:
		void resizeEvent(QResizeEvent *event);

	private slots:
		void dataChanged(const QModelIndex & topLeft, const  QModelIndex & bottomRight);
		void rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
		void rowsAboutToBeInserted(const QModelIndex & parent, int start, int end);
		void rowsRemoved(const QModelIndex & parent, int start, int end);
		void rowsInserted(const QModelIndex & parent, int start, int end);

	private:
		ViewType m_viewType;
		View *m_view;
		QWidget *m_header;
		CalendarModel *m_model;
	};
}

#endif
