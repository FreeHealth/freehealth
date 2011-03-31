#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QScrollArea>
#include <QAbstractItemModel>
#include <QTimer>

#include "common.h"
#include "view.h"
#include "calendar_navbar.h"

class QVBoxLayout;

namespace Calendar {
	class CalendarWidget : public QWidget
	{
		Q_OBJECT
	public:
		CalendarWidget(QWidget *parent = 0);

		QAbstractItemModel *model() const { return m_model; }
		void setModel(QAbstractItemModel *model);

		Calendar::ViewType viewType() const { return m_navbar->viewType(); }
		void setViewType(Calendar::ViewType viewType);

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

		// timer
		void timeout();

	private:
		QVBoxLayout *m_mainLayout;
		QScrollArea *m_scrollArea;
		CalendarNavbar *m_navbar;
		ViewType m_viewType;
		View *m_view;
		ViewHeader *m_header;
		QAbstractItemModel *m_model;
		QTimer m_timer; // used to refresh every date/time stuffs
	};
}

#endif
