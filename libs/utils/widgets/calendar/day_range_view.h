#ifndef DAY_RANGE_VIEW_H
#define DAY_RANGE_VIEW_H

#include "view.h"

namespace Calendar {
	// this class is used to build a hierarchical structure of items for a day and to manage overlappings
	// the algorithm behavior is inspired from google calendar
	class CalendarItemNode
	{
	public:
		CalendarItemNode(const CalendarItem &item, CalendarItemNode *colliding = 0, int index = -1) : m_item(item), m_right(0), m_next(0), m_colliding(colliding), m_index(index), m_maxCount(0) {}
		~CalendarItemNode();

		const CalendarItem &item() const { return m_item; }			// the calendar item associates with the node
		CalendarItemNode *right() const { return m_right; }			// the node which is overlapping itself and shares horizontal space with itself and other overlapping sibling nodes
		CalendarItemNode *next() const { return m_next; }			// the node which follows itself in the time but non overlapping (will be drawn at the bottom)
		CalendarItemNode *colliding() const { return m_colliding; }	// the potential colliding node if it exists. It is always at the right position.
		int index() const { return m_index; }						// the right index of the node. Starts at 0.
		int left() const { return m_left; }
		int width() const { return m_width; }

		// store an item at the right place with a recursive method depending on date ranges
		void store(const CalendarItem &item);

		// mandatory when we want to compute widths
		void prepareForWidthsComputing();

		// compute width of the node and all its neighbours and add them into a list
		void computeWidths(int left, int width, QList<CalendarItemNode*> &list);

	private:
		CalendarItem m_item;
		CalendarItemNode *m_right;
		CalendarItemNode *m_next;
		CalendarItemNode *m_colliding;
		int m_index;
		int m_maxCount;
		int m_maxCountBeforeColliding;
		int m_left;
		int m_width;

		// returns the most bottom node (the most far next node)
		CalendarItemNode *mostBottomNode();
		// returns the next colliding node with <item> potentially including <this>
		CalendarItemNode *getNextCollidingNode(const CalendarItem &item);

		// returns the maximal count of right nodes
		int computeMaxCount();
		// returns the maximal count of right nodes before the colliding one
		int computeMaxCountBeforeColliding();
	};

	class DayItemWidget;

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
		DayItemWidget *m_pressItemWidget;
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
