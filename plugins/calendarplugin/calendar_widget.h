#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QScrollArea>
#include <QAbstractItemModel>

#include "common.h"
#include "view.h"
#include "calendar_navbar.h"

namespace Calendar {
	class CalendarWidget : public QScrollArea
	{
		Q_OBJECT
	public:
		CalendarWidget(QWidget *parent = 0);

		QAbstractItemModel *model() const { return m_model; }
		void setModel(QAbstractItemModel *model);

		Calendar::ViewType viewType() const { return m_navbar->viewType(); }
		void setViewType(Calendar::ViewType viewType);

	protected:
		void resizeEvent(QResizeEvent *event);

	private slots:
		// for model
		void dataChanged(const QModelIndex & topLeft, const  QModelIndex & bottomRight);
		void rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
		void rowsAboutToBeInserted(const QModelIndex & parent, int start, int end);
		void rowsRemoved(const QModelIndex & parent, int start, int end);
		void rowsInserted(const QModelIndex & parent, int start, int end);

		// navigation bar slots
		void firstDateChanged();
		void viewTypeChanged();

		// header
		void headerResized(const QSize &size);

	private:
		CalendarNavbar *m_navbar;
		ViewType m_viewType;
		View *m_view;
		ViewHeader *m_header;
		QAbstractItemModel *m_model;
	};
}

#endif
