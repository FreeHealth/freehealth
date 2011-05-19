#ifndef MONTH_VIEW_H
#define MONTH_VIEW_H

#include "view.h"

namespace Calendar {
	class MonthHeader : public ViewHeader
	{
		Q_OBJECT
	public:
		MonthHeader(QWidget *parent = 0) : ViewHeader(parent) {}

		virtual QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent *event);
	};

	class MonthView : public View
	{
		Q_OBJECT
	public:
		MonthView(QWidget *parent = 0);

		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;

		virtual ViewHeader *createHeaderWidget(QWidget *parent = 0);

	protected:
		virtual void paintBody(QPainter *painter, const QRect &visibleRect);
		virtual void resetItemWidgets();
		virtual void refreshItemsSizesAndPositions();
		QRect getDayRect(const QDate &day) const;

	private:
		int m_weekCount;
		QPair<QDate,QDate> m_monthBoundingDays;
//		void paintEvents(QPainter &painter, const QDate &day, const QRect &dayRect);

	private slots:
		void firstDateChanged();
	};
}

#endif
