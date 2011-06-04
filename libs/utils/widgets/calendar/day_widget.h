#ifndef DAY_WIDGET_H
#define DAY_WIDGET_H

#include "calendar_item_widget.h"

namespace Calendar {
	class AbstractCalendarModel;

	class DayWidget : public CalendarItemWidget
	{
		Q_OBJECT
	public:
		/** if uid is empty, this calendar item is considered as temporary and will be drawn with transparence
		 */
		DayWidget(QWidget *parent = 0, const QString &uid = "", AbstractCalendarModel *model = 0);

		bool inMotion() const { return m_inMotion; }
		void setInMotion(bool value);

	private:
		bool m_inMotion;
		QWidget *m_aboveWidget;

	protected:
		virtual void paintEvent(QPaintEvent *event);
	};
}


#endif
