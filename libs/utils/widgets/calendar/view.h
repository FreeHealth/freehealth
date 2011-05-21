#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QDate>

#include "calendar_item_widget.h"
#include "abstract_calendar_model.h"

class QPainter;
class QRect;
class QScrollArea;

namespace Calendar {
	class ViewHeader : public QWidget {
		Q_OBJECT
	public:
		ViewHeader(QWidget *parent = 0);

		const QDate &firstDate() const { return m_firstDate; }
		void setFirstDate(const QDate &date);

		void setScrollArea(QScrollArea *scrollArea);

	signals:
		void resized(const QSize &size);

	protected:
		QScrollArea *m_scrollArea;

		virtual void resizeEvent(QResizeEvent *event);

	private:
		QDate m_firstDate;
	};

	/**
	 * \brief a view is an abstract class which defines common things about calendar views (headers, body, etc)
	 */
	class View : public QWidget
	{
		Q_OBJECT
	public:
		View(QWidget *parent = 0) : QWidget(parent), m_refreshGrid(false), m_model(0) {}

		/** returns the top header height for the view */
		virtual int topHeaderHeight() const = 0;
		/** returns the left height width for the view */
		virtual int leftHeaderWidth() const = 0;

		virtual ViewHeader *createHeaderWidget(QWidget *parent = 0) { Q_UNUSED(parent); return 0; }

		/** sets a date for the view
		 */
		void setFirstDate(const QDate &firstDate);

		const QDate &firstDate() const { return m_firstDate; }

		/** used to refresh all current date time stuffs */
		virtual void refreshCurrentDateTimeStuff();

		/** returns the current model */
		AbstractCalendarModel *model() const { return m_model; }

		/** called when a new model has been defined */
		void setModel(AbstractCalendarModel *model);

	signals:
		void firstDateChanged();

	protected:
		QDate m_firstDate;

		QPixmap generatePixmap();
		void forceUpdate() { m_refreshGrid = true; update(); }

		virtual void paintBody(QPainter *, const QRect &) {};
		virtual void paintEvent(QPaintEvent *event);
		virtual void resizeEvent(QResizeEvent *event);
		virtual void refreshItemSizeAndPosition(CalendarItemWidget *) {}
		virtual void refreshItemsSizesAndPositions();
		virtual void resetItemWidgets() {}

		CalendarItemWidget *getWidgetByUid(const QString &uid) const;
		QList<CalendarItemWidget*> getWidgetsByDate(const QDate &dayDate) const;
		void deleteAllWidgets();

	protected slots:
		virtual void itemInserted(const CalendarItem &item) { Q_UNUSED(item); }
		virtual void itemModified(const CalendarItem &oldItem, const CalendarItem &newItem) { Q_UNUSED(oldItem); Q_UNUSED(newItem); }
		virtual void itemRemoved(const CalendarItem &removedItem) { Q_UNUSED(removedItem); }

	private:
		bool m_refreshGrid;
		AbstractCalendarModel *m_model;
	};
}

#endif
