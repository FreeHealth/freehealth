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
	/**
	 * \brief a view is an abstract class which defines common things about calendar views (headers, body, etc)
	 */
	class ViewWidget : public QWidget
	{
		Q_OBJECT
	public:
		ViewWidget(QWidget *parent = 0) : QWidget(parent), masterScrollArea(0), m_refreshGrid(false), m_model(0) {}

		/** returns the first date of the view */
		const QDate &firstDate() const { return m_firstDate; }

		/** sets a date for the view
		 */
		void setFirstDate(const QDate &firstDate);

		/** used to refresh all current date time stuffs */
		virtual void refreshCurrentDateTimeStuff();

		/** returns the current model */
		AbstractCalendarModel *model() const { return m_model; }

		/** called when a new model has been defined */
		void setModel(AbstractCalendarModel *model);

		/** set the master widget scrollarea used to paint (generally used for header to know how the body is painted) */
		void setMasterScrollArea(QScrollArea *scrollArea);

	signals:
		void firstDateChanged();

	protected:
		QScrollArea *masterScrollArea;

		QPixmap generatePixmap();
		void forceUpdate() { m_refreshGrid = true; update(); }

                virtual void paintBody(QPainter *, const QRect &) {}
		virtual void paintEvent(QPaintEvent *event);
		virtual void resizeEvent(QResizeEvent *event);
		virtual void refreshItemSizeAndPosition(CalendarItemWidget *) {}
		virtual void refreshItemsSizesAndPositions();
		virtual void resetItemWidgets() {}
		bool eventFilter(QObject *obj, QEvent *event);

		CalendarItemWidget *getWidgetByUid(const QString &uid) const;
		QList<CalendarItemWidget*> getWidgetsByDate(const QDate &dayDate) const;
		void deleteAllWidgets();

	protected slots:
		virtual void itemInserted(const Calendar::CalendarItem &item) { Q_UNUSED(item); }
		virtual void itemModified(const Calendar::CalendarItem &oldItem, const Calendar::CalendarItem &newItem) { Q_UNUSED(oldItem); Q_UNUSED(newItem); }
		virtual void itemRemoved(const Calendar::CalendarItem &removedItem) { Q_UNUSED(removedItem); }
		virtual void reset();

	private:
		QDate m_firstDate;
		bool m_refreshGrid;
		AbstractCalendarModel *m_model;
	};
}

#endif
