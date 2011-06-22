#ifndef DAY_WIDGET_H
#define DAY_WIDGET_H

#include "calendar_item_widget.h"

namespace Calendar {
	class AbstractCalendarModel;

	/**
	 * This class represents a day event without hour range specification
	 */
	class DayWidget : public CalendarItemWidget
	{
		Q_OBJECT
	public:
		/** if uid is empty, this calendar item is considered as temporary and will be drawn with transparence
		 */
		DayWidget(QWidget *parent = 0, const QString &uid = "", AbstractCalendarModel *model = 0);

		bool inMotion() const { return m_inMotion; }
		void setInMotion(bool value);

		QSize sizeHint() const;
		static QSize staticSizeHint();

	private:
		bool m_inMotion;
		QWidget *m_aboveWidget;
		QFont m_titleFont; // TODO: choose a better font than the default one

		static QFont getTitleFont();

	protected:
		virtual void paintEvent(QPaintEvent *event);
	};
}


#endif
