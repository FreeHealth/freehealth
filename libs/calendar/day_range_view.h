/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DAY_RANGE_VIEW_H
#define DAY_RANGE_VIEW_H

#include "view.h"

namespace Calendar {
	class HourRangeWidget;
	class DayWidget;

	class DayRangeHeader : public ViewWidget
	{
		Q_OBJECT
	public:
		DayRangeHeader(QWidget *parent = 0, int rangeWidth = 7);

		QSize sizeHint() const;

		/** for a week, the range width is 7 */
		int rangeWidth() const { return m_rangeWidth; }
		void setRangeWidth(int width);

	protected:
		void paintEvent(QPaintEvent *event);
		void resetItemWidgets();
		void refreshItemsSizesAndPositions();
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

	private:
		enum MouseMode {
			MouseMode_None,
			MouseMode_Move,
			MouseMode_Resize,
			MouseMode_Creation
		};
		int m_rangeWidth;
		int m_maxDepth;
		QFont m_scaleFont; // TODO: choose a better font
		QDate m_pressDate;
		QDate m_previousDate;
		QPoint m_pressPos;
		MouseMode m_mouseMode;
		DayWidget *m_pressItemWidget;
		CalendarItem m_pressItem;
		QPair<QDate,QDate> m_pressDayInterval;

		int getScaleHeight() const;
		int getContainWidth() const;

		QDate getDate(int x) const;
		QList<CalendarItem> getItems() const;
		QRect computeWidgetRect(const QDate &firstDay, const QDate &lastDay, int depth) const;
		void computeWidgets();
		void computeWidget(const CalendarItem &item, int depth);
		int getLastWidgetBottom() const;

	private slots:
		void modifyPressItem();
		void removePressItem();
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

		/** returns a minutes count for the items creation/resize granularity. 30 by default. */
		int granularity() const { return m_granularity; }

		/** set the granularity. Can only be a divider of 24 * 60 */
		void setGranularity(int value);

		/** returns the minutes count for an item creation. 30 by default. */
		int itemDefaultDuration() const { return m_itemDefaultDuration; }

		/** set the default duration of an item */
		void setItemDefaultDuration(int value);

        void setDayScaleHourDivider(int value);

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
        void itemInserted(const Calendar::CalendarItem &item);
        void itemModified(const Calendar::CalendarItem &oldItem, const Calendar::CalendarItem &newItem);
        void itemRemoved(const Calendar::CalendarItem &removedItem);

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
		static int m_minimumItemHeight;
		int m_rangeWidth;
		QDateTime m_pressDateTime;
		QDateTime m_previousDateTime;
		QPoint m_pressPos;
		HourRangeWidget *m_pressItemWidget;
		CalendarItem m_pressItem;
		MouseMode m_mouseMode;
		int m_granularity;
		int m_itemDefaultDuration;
        int m_dayScaleHourDivider;

		// if end < begin, the end time will be considered as midnight
		QRect getTimeIntervalRect(int day, const QTime &begin, const QTime &end) const;
		// returns the left position and width of the day band
		QPair<int, int> getBand(const QDate &date) const;
		QPair<int, int> getItemVerticalData(const QTime &begin, const QTime &end) const;
		QDateTime quantized(const QDateTime &dateTime) const; // constrains the date to the nearest granular value
		QDateTime getDateTime(const QPoint &pos) const;

		// refresh all widgets of a day
		void refreshDayWidgets(const QDate &dayDate);

	private slots:
		void modifyPressItem();
		void removePressItem();
	};
}

#endif
