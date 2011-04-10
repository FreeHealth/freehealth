#ifndef DAY_RANGE_VIEW_H
#define DAY_RANGE_VIEW_H

#include "view.h"

namespace Calendar {
	class DayRangeHeader : public ViewHeader
	{
		Q_OBJECT
	public:
		DayRangeHeader(QWidget *parent = 0, int rangeWidth = 7) : ViewHeader(parent), m_rangeWidth(rangeWidth) {}

		virtual QSize sizeHint() const;

		/** for a week, the range width is 7 */
		int rangeWidth() const { return m_rangeWidth; }
		void setRangeWidth(int width);

	protected:
		void paintEvent(QPaintEvent *event);

	private:
		int m_rangeWidth;
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

	class DayRangeView : public View
	{
		Q_OBJECT
	public:
		DayRangeView(QWidget *parent = 0, int rangeWidth = 7);

		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;

		/** for a week, the range width is 7 */
		int rangeWidth() const { return m_rangeWidth; }
		void setRangeWidth(int width);

		virtual ViewHeader *createHeaderWidget(QWidget *parent = 0);

		virtual QSize sizeHint() const;

	protected:
		virtual void paintBody(QPainter *painter, const QRect &visibleRect);
		virtual void refreshItemSizeAndPosition(CalendarItemWidget *item);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void resetItemWidgets();

	protected slots:
		void itemInserted(const CalendarItem &item);

	private slots:
		void firstDateChanged();

	private:
		enum MouseMode {
			MouseMode_Move,
			MouseMode_Resize,
			MouseMode_Creation
		};

		HourWidget *m_hourWidget;
		static int m_leftScaleWidth;
		static int m_hourHeight;
		int m_rangeWidth;
		QDateTime m_pressDateTime;
		QPoint m_pressPos;
		CalendarItemWidget *m_pressItem;
		MouseMode m_mouseMode;

		/* if end < begin, the end time will be considered as midnight */
		QRect getTimeIntervalRect(int day, const QTime &begin, const QTime &end) const;
		QDateTime getDateTime(const QPoint &pos) const;
	};
}

#endif
