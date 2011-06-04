#ifndef MONTH_VIEW_H
#define MONTH_VIEW_H

#include "view.h"

namespace Calendar {
	class MonthHeader : public ViewWidget
	{
		Q_OBJECT
	public:
		MonthHeader(QWidget *parent = 0);

		QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent *event);
	};

	class MonthBody : public ViewWidget
	{
		Q_OBJECT
	public:
		MonthBody(QWidget *parent = 0);

		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;

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
