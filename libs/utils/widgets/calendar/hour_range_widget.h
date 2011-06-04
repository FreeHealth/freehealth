#ifndef HOUR_RANGE_WIDGET_H
#define HOUR_RANGE_WIDGET_H

#include "calendar_item_widget.h"

namespace Calendar {
	class AbstractCalendarModel;

	class HourRangeWidget : public CalendarItemWidget
	{
		Q_OBJECT
	public:
		/** if uid is empty, this calendar item is considered as temporary and will be drawn with transparence
		 */
		HourRangeWidget(QWidget *parent = 0, const QString &uid = "", AbstractCalendarModel *model = 0);

		bool inMotion() const { return m_inMotion; }
		void setInMotion(bool value);

	private:
		bool m_inMotion;
		QWidget *m_aboveWidget;

	protected:
		virtual void paintEvent(QPaintEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
//		virtual void mouseReleaseEvent(QMouseEvent *event);
	};
}

#endif
