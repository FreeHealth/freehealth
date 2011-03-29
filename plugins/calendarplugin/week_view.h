#ifndef WEEK_VIEW_H
#define WEEK_VIEW_H

#include "view.h"

namespace Calendar {

	class WeekHeader : public QWidget {
		Q_OBJECT
	public:
		WeekHeader(QWidget *parent = 0) : QWidget(parent) {}

		const QDate &firstDate() const { return m_firstDate; }
		void setFirstDate(const QDate &date);

	protected:
		void paintEvent(QPaintEvent *event);

	private:
		QDate m_firstDate;
	};

	class WeekView : public View
	{
		Q_OBJECT
	public:
		WeekView(QWidget *parent = 0);

		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;

		virtual QWidget *createHeaderWidget(QWidget *parent = 0);

		virtual QSize sizeHint() const;

	protected:
		virtual void paintBody(QPainter *painter, const QRect &visibleRect);

	private slots:
		void firstDateChanged();

	private:
		static int m_leftScaleWidth;
		static int m_hourHeight;
	};
}

#endif
