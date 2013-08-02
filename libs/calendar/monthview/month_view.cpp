/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <QPainter>

#include "month_view.h"
#include "month_day_widget.h"
#include <calendar/common.h>

using namespace Calendar;
using namespace Internal;

MonthHeader::MonthHeader(QWidget *parent) : ViewWidget(parent) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize MonthHeader::sizeHint() const {
	return QSize(0, 20);
}

void MonthHeader::paintEvent(QPaintEvent *) {
	// fill all in light blue
	QPainter painter(this);
	painter.fillRect(rect(), QColor(220, 220, 255));

	// bottom line
	QPen pen = painter.pen();
	pen.setColor(QColor(200, 200, 255));
	painter.setPen(pen);
	painter.drawLine(0, rect().bottom(), rect().right(), rect().bottom());

	// text
	pen.setColor(QColor(150, 150, 255));
	painter.setPen(pen);

	int containWidth = rect().width();
	for (int i = 0; i < 7; ++i) {
        QRect r(QPoint((i * containWidth) / 7, 0), QPoint(((i + 1) * containWidth) / 7 - 1 + 2, rect().height())); // +2 is a vertical correction to not be stucked to the top line
        painter.drawText(r, Qt::AlignHCenter | Qt::AlignTop, QLocale().toString(QDate(2012, 05, 07 + i), "ddd"));
	}
}

/////////////////////////////////////////////////////////////

MonthBody::MonthBody(QWidget *parent) : ViewWidget(parent) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	connect(this, SIGNAL(firstDateChanged()), this, SLOT(firstDateChanged()));
}

int MonthBody::topHeaderHeight() const {
	return 0;
}

int MonthBody::leftHeaderWidth() const {
	return 0;
}

void MonthBody::paintBody(QPainter *painter, const QRect &visibleRect) {
	painter->fillRect(visibleRect, Qt::white);
	QPen pen = painter->pen();
	pen.setColor(QColor(200, 200, 200));
	pen.setCapStyle(Qt::FlatCap);
	painter->setPen(pen);

	int horiAmount = visibleRect.width() - 6; // total width without lines width (7 days => 6 lines)
	int vertiAmount = visibleRect.height() - m_weekCount + 1; // total height without lines height

	// draw current day?
	QDate now = QDate::currentDate();
	if (now >= m_monthBoundingDays.first && now <= m_monthBoundingDays.second)
		painter->fillRect(getDayRect(now), QColor(255, 255, 200));

	// vertical lines
	for (int i = 1; i < 7; ++i)
		painter->drawLine((i * horiAmount) / 7 + i - 1, 0,
						  (i * horiAmount) / 7 + i - 1, visibleRect.height());

	// horizontal lines
	for (int i = 1; i < m_weekCount; ++i)
		painter->drawLine(0, (i * vertiAmount) / m_weekCount + i - 1,
						  visibleRect.width(), (i * vertiAmount) / m_weekCount + i - 1);

	// day texts
	for (QDate day = m_monthBoundingDays.first; day <= m_monthBoundingDays.second; day = day.addDays(1)) {
		QRect dayRect = getDayRect(day);

		QString text;
		if (day.day() == 1)
            text = QLocale().toString(day, tr("d MMM"));
		else
            text = QLocale().toString(day, tr("d"));

		if (day.month() != firstDate().month())
			pen.setColor(QColor(180, 180, 180));
		else
			pen.setColor(QColor(100, 100, 100));
		painter->setPen(pen);

		painter->drawText(dayRect.adjusted(0, 2, -2, 0), Qt::AlignRight | Qt::AlignTop, text); // correction to not be stucked to the top/right lines
//		paintEvents(*painter, day, dayRect.adjusted(0, 2 + QFontMetrics(painter->font()).height(), 0, 0));
	}
}

/*void MonthBody::paintEvents(QPainter &painter, const QDate &day, const QRect &dayRect) {
	QList<CalendarItem> items = model()->getItemsBetween(day, day);
	if (!items.count())
		return;

	qSort(items.begin(), items.end(), calendarItemLessThan);

	int itemHeight = QFontMetrics(painter.font()).height();
	int visibleItemsCount = dayRect.height() / itemHeight - 1;
	int top = 0;
	int count = visibleItemsCount < items.count() ? visibleItemsCount - 1 : items.count();

	for (int i = 0; i < count; i++) {
		const CalendarItem &item = items[i];
		painter.drawText(dayRect.adjusted(2, top, -2, 0), Qt::AlignLeft, QString("%1 %2").arg(item.beginning().time().toString("hh:mm")).arg(item.title().isEmpty() ? "(untitled)" : item.title()));
		top += itemHeight;
	}

	if (count != items.count()) { // add a link
		painter.drawText(dayRect.adjusted(2, top, -2, 0), Qt::AlignLeft, "more elements");
	}
	}*/

void MonthBody::resetItemWidgets() {
	// re-create all widgets
	QList<MonthDayWidget*> list;
	foreach (QObject *obj, children()) {
		MonthDayWidget *widget = qobject_cast<MonthDayWidget*>(obj);
		if (widget)
			list << widget;
	}
	qDeleteAll(list);

    if (!model() || !firstDate().isValid())
		return;

	for (QDate day = m_monthBoundingDays.first; day <= m_monthBoundingDays.second; day = day.addDays(1)) {
		if (!model()->getItemsBetween(day, day).count())
			continue;

		QRect dayWithoutHeaderRect = getDayRect(day).adjusted(0, 2 + QFontMetrics(QFont()).height(), 0, 0); // TODO: take the real header font
		MonthDayWidget *widget = new MonthDayWidget(model(), day, this);
		widget->move(dayWithoutHeaderRect.topLeft());
		widget->resize(dayWithoutHeaderRect.size());
		widget->show();
	}
}

QRect MonthBody::getDayRect(const QDate &day) const {
	int i = day.dayOfWeek() - 1;
	int j = m_monthBoundingDays.first.daysTo(day) / 7;

	int horiAmount = rect().width() - 6; // total width without lines width
	int vertiAmount = rect().height() - m_weekCount + 1; // total height without lines height
	return QRect((i * horiAmount) / 7 + i, (j * vertiAmount) / m_weekCount + j,
				 ((i + 1) * horiAmount) / 7 - (i * horiAmount) / 7,
				 ((j + 1) * vertiAmount) / m_weekCount - (j * vertiAmount) / m_weekCount);
}

void MonthBody::firstDateChanged() {
	// refresh some internal variables

	// compute week counts
	m_weekCount = 0;
	m_monthBoundingDays = Calendar::getBoundingMonthDaysInterval(firstDate());
	m_weekCount = (m_monthBoundingDays.first.daysTo(m_monthBoundingDays.second) + 1) / 7;
}

void MonthBody::refreshItemsSizesAndPositions() {
	foreach (QObject *obj, children()) {
		MonthDayWidget *widget = qobject_cast<MonthDayWidget*>(obj);
		if (widget) {
			QRect dayWithoutHeaderRect = getDayRect(widget->day()).adjusted(0, 2 + QFontMetrics(QFont()).height(), 0, 0); // TODO: take the real header font
			widget->move(dayWithoutHeaderRect.topLeft());
			widget->resize(dayWithoutHeaderRect.size());
		}
	}
}
