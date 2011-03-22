#ifndef WEEK_VIEW_H
#define WEEK_VIEW_H

#include "view.h"

namespace Calendar {
	class WeekView : public View
	{
	public:
		WeekView(QWidget *parent = 0);

		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;

		virtual void paintTopHeader(QPainter *painter);
		virtual void paintLeftHeader(QPainter *painter);
		virtual void paintBody(QPainter *painter, const QRect &visibleRect);

	protected:
		virtual void paintEvent(QPaintEvent *event);

	private:
		static int m_leftScaleWidth;
		static int m_hourHeight;
	};
}

#endif
