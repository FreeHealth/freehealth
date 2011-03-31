#ifndef WEEK_VIEW_H
#define WEEK_VIEW_H

#include "view.h"

namespace Calendar {
	class WeekHeader : public ViewHeader
	{
		Q_OBJECT
	public:
		WeekHeader(QWidget *parent = 0) : ViewHeader(parent) {}

		virtual QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent *event);
	};

	class HourWidget : public QWidget
	{
		Q_OBJECT
	public:
		HourWidget(QWidget *parent = 0) : QWidget(parent) {
			setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		}

		virtual QSize sizeHint() const { return QSize(0, 2); }

	protected:
		void paintEvent(QPaintEvent *event);
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
		virtual void refreshItemSizeAndPosition(CalendarItem *item);

	private slots:
		void firstDateChanged();

	private:
		HourWidget *m_hourWidget;
		static int m_leftScaleWidth;
		static int m_hourHeight;

		QRect getTimeIntervalRect(int day, const QTime &begin, const QTime &end) const;
	};
}

#endif
