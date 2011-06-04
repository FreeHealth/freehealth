#ifndef DAY_RANGE_VIEW_H
#define DAY_RANGE_VIEW_H

#include "view.h"

namespace Calendar {
	class HourRangeWidget;

	class DayRangeHeader : public ViewWidget
	{
		Q_OBJECT
	public:
		DayRangeHeader(QWidget *parent = 0, int rangeWidth = 7);

		virtual QSize sizeHint() const;

		/** for a week, the range width is 7 */
		int rangeWidth() const { return m_rangeWidth; }
		void setRangeWidth(int width);

	protected:
		void paintEvent(QPaintEvent *event);

	private:
		int m_rangeWidth;

		QList<CalendarItem> getItems() const;
	};

	class HourWidget : public QWidget
	{
		Q_OBJECT
	public:
		HourWidget(QWidget *parent = 0);

		virtual QSize sizeHint() const { return QSize(0, 2); }

	protected:
		void paintEvent(QPaintEvent *event);
	};

	class DayRangeBody : public ViewWidget
	{
		Q_OBJECT
	public:
		DayRangeBody(QWidget *parent = 0, int rangeWidth = 7);

		/** for a week, the range width is 7 */
		int rangeWidth() const { return m_rangeWidth; }
		void setRangeWidth(int width);

		virtual QSize sizeHint() const;

	protected:
		virtual void paintBody(QPainter *painter, const QRect &visibleRect);
		virtual void refreshItemsSizesAndPositions();
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void mouseDoubleClickEvent(QMouseEvent *event);
		virtual void resetItemWidgets();

	protected slots:
		void itemInserted(const CalendarItem &item);
		void itemModified(const CalendarItem &oldItem, const CalendarItem &newItem);
		void itemRemoved(const CalendarItem &removedItem);

	private:
		enum MouseMode {
			MouseMode_None,
			MouseMode_Move,
			MouseMode_Resize,
			MouseMode_Creation
		};

		HourWidget *m_hourWidget;
		static int m_leftScaleWidth;
		static int m_hourHeight;
		int m_rangeWidth;
		QDateTime m_pressDateTime;
		QDateTime m_previousDateTime;
		QPoint m_pressPos;
		HourRangeWidget *m_pressItemWidget;
		CalendarItem m_pressItem;
		MouseMode m_mouseMode;

		// if end < begin, the end time will be considered as midnight
		QRect getTimeIntervalRect(int day, const QTime &begin, const QTime &end) const;
		// returns the left position and width of the day band
		QPair<int, int> getBand(const QDate &date) const;
		QPair<int, int> getItemVerticalData(const QTime &begin, const QTime &end) const;
		QDateTime getDateTime(const QPoint &pos) const;

		// refresh all widgets of a day
		void refreshDayWidgets(const QDate &dayDate);

	private slots:
		void modifyPressItem();
		void removePressItem();
	};
}

#endif
