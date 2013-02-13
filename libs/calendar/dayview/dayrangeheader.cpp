/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "dayrangeheader.h"
#include "hour_range_widget.h"
#include "hour_range_node.h"
#include "hourwidget.h"
#include "day_widget.h"
#include "day_node.h"
#include <calendar/common.h>
#include <calendar/modelanditem/abstract_calendar_model.h>
#include <calendar/modelanditem/basic_item_edition_dialog.h>
#include <calendar/calendar_widget.h>

#include <utils/log.h>

#include <QRect>
#include <QPainter>
#include <QDate>
#include <QPixmapCache>
#include <QScrollArea>
#include <QMouseEvent>
#include <QMenu>

#include <QDebug>

enum { WarnBodyMouseEvents = true, WarnHeaderMouseEvents = false, WarnDragDropFromBody = true};

using namespace Calendar;
using namespace Internal;

namespace Calendar {
namespace Internal {
class DayRangeHeaderPrivate
{
public:
    DayRangeHeaderPrivate(DayRangeHeader *parent) :
        q(parent)
    {}

public:
    // FIXME: remove MouseMode enum here as it's not used??
    enum MouseMode {
        MouseMode_None,
        MouseMode_Move,
        MouseMode_ResizeTop,
        MouseMode_ResizeBottom,
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

    int getScaleHeight() const
    {
        return QFontMetrics(m_scaleFont).height() + 5;
    }

    int getContainWidth() const
    {
        return (q->masterScrollArea ? q->masterScrollArea->viewport()->width() : q->width()) - 60;
    }

    QDate getDate(int x) const {
        // get day
        int containWidth = (q->masterScrollArea ? q->masterScrollArea->viewport()->width() : q->width()) - 60;
        int day = 0;
        for (int i = 0; i < m_rangeWidth; ++i) {
            if (x >= (i * containWidth) / m_rangeWidth + 60 && x < ((i + 1) * containWidth) / m_rangeWidth + 60){
                break;
            }
            day++;
        }
        return q->firstDate().addDays(day);
    }

    QList<CalendarItem> getItems() const
    {
        if (!q->model() || !q->firstDate().isValid())
            return QList<CalendarItem>();
        // optimization : do not compute items every time...
        QList<CalendarItem> items = q->model()->getItemsBetween(q->firstDate(), q->firstDate().addDays(m_rangeWidth - 1));
        // filter only day items
        for (int i = items.count() - 1; i >= 0; i--) {
            const CalendarItem &item = items[i];
            if (item.beginningType() == Date_DateTime && item.endingType() == Date_DateTime)
                items.removeAt(i);
        }
        return items;
    }

    QRect computeWidgetRect(const QDate &firstDay, const QDate &lastDay, int depth) const
    {
        int containWidth = getContainWidth();
        int scaleHeight = getScaleHeight();
        int widgetHeight = DayWidget::staticSizeHint().height();

#if QT_VERSION >= 0x050000
        // Qt5 QDate::daysTo() returns a qint64
        int firstIndex = qMax(qint64(0), q->firstDate().daysTo(firstDay));
        int lastIndex = qMin(qint64(m_rangeWidth - 1), q->firstDate().daysTo(lastDay));
#else
        // Qt4 QDate::daysTo() returns a int
        int firstIndex = qMax(0, q->firstDate().daysTo(firstDay));
        int lastIndex = qMin(m_rangeWidth - 1, q->firstDate().daysTo(lastDay));
#endif
        int w = ((lastIndex + 1) * containWidth) / m_rangeWidth - (firstIndex * containWidth) / m_rangeWidth - 2;
        return QRect(QPoint(60 + (firstIndex * containWidth) / m_rangeWidth + 1, scaleHeight + depth * (widgetHeight + 1)), QSize(w, widgetHeight));
    }

    void computeWidget(const CalendarItem &item, int depth)
    {
        QPair<QDate,QDate> dayInterval = getIntersectDayRange(item.beginning(), item.ending());
        QRect r = computeWidgetRect(dayInterval.first, dayInterval.second, depth);

        DayWidget *widget = new DayWidget(q, item.uid(), q->model());
        widget->move(r.topLeft());
        widget->resize(r.size());
        widget->show();
    }

    int getLastWidgetBottom() const
    {
        int maxBottom = -1;
        foreach (QObject *obj, q->children()) {
            DayWidget *widget = qobject_cast<DayWidget*>(obj);
            if (!widget)
                continue;
            int bottom = widget->y() + widget->height();
            if (widget && bottom > maxBottom)
                maxBottom = bottom;
        }
        return maxBottom;
    }

    void computeWidgets() {
        // 1. remove old day widgets
        foreach (QObject *object, q->children()) {
            DayWidget *widget = qobject_cast<DayWidget*>(object);
            if (widget)
                delete widget;
        }

        m_maxDepth = -1; // if no widgets => convention is to set the max depth to -1

        // 2. create new widgets
        QList<CalendarItem> items = getItems();
        if (!items.count())
            return;

        qSort(items.begin(), items.end(), calendarItemLessThan);

        m_maxDepth = 0;

        DayStore store;
        for (int i = 0; i < items.count(); i++) {
            int depth = store.store(items[i]);
            if (depth > m_maxDepth)
                m_maxDepth = depth;
            computeWidget(items[i], depth);
        }
    }

    void paintWidget(QPainter &painter)
    {
        // bottom line
        QPen pen = painter.pen();
        pen.setColor(QColor(200, 200, 255));
        painter.setPen(pen);
        painter.drawLine(0, q->rect().bottom(), q->rect().right(), q->rect().bottom());

        pen.setColor(QColor(150, 150, 255));
        painter.setPen(pen);

        // vertical lines
        int containWidth = (q->masterScrollArea ? q->masterScrollArea->viewport()->width() : q->width()) - 60;
        QPen oldPen = painter.pen();
        QDate date = q->firstDate();
        QDate now = QDate::currentDate();
        int fontHeight = QFontMetrics(QFont()).height();
        for (int i = 0; i < m_rangeWidth; ++i) {
            QRect br(QPoint(60 + (i * containWidth) / m_rangeWidth, 0), QPoint(60 + ((i + 1) * containWidth) / m_rangeWidth, q->rect().height() - 2));
            if (m_mouseMode == MouseMode_Creation && date >= m_pressDayInterval.first && date <= m_pressDayInterval.second)
                painter.fillRect(br, QColor(240, 240, 240));
            else
                painter.fillRect(br, Qt::white);

            // vertical lines
            if (i > 0) {
                QPen pen = painter.pen();
                pen.setColor(QColor(200, 200, 200));
                pen.setCapStyle(Qt::FlatCap);
                painter.setPen(pen);
                painter.drawLine(60 + (i * containWidth) / m_rangeWidth, 0,
                                 60 + (i * containWidth) / m_rangeWidth, q->rect().height());
            }
            br = QRect(QPoint(60 + (i * containWidth) / m_rangeWidth, q->rect().height() - 5), QPoint(60 + ((i + 1) * containWidth) / m_rangeWidth, q->rect().height() - 2));
            painter.fillRect(br, QColor(220, 220, 255));

            QRect r(QPoint(60 + (i * containWidth) / m_rangeWidth, 0), QPoint(60 + ((i + 1) * containWidth) / m_rangeWidth, fontHeight + 4));
            if (date == now){
                painter.fillRect(r, QColor(200,200,255));
                QPen pen = painter.pen();;
                pen.setColor(QColor(0, 0, 255));
                painter.setPen(pen);
            } else {
                painter.fillRect(r, QColor(220, 220, 255));
                QPen pen = painter.pen();;
                pen.setColor(QColor(0, 0, 255));
                painter.setPen(pen);
            }
            r.adjust(0, 2, 0, 0);  // +2 is a vertical correction to not be stucked to the top line
            if (m_rangeWidth == 1)
                painter.drawText(r, Qt::AlignHCenter | Qt::AlignTop, QLocale().toString(date, "dddd d/M")); //TODO: tr("dddd d/M", "short date format in agenda's calendar header")).toLower());
            else
                painter.drawText(r, Qt::AlignHCenter | Qt::AlignTop, QLocale().toString(date, "ddd d/M")); //TODO: tr("ddd d/M", "short date format in agenda's calendar header")).toLower());
            painter.setPen(oldPen);
            date = date.addDays(1);
        }
    }


private:
    DayRangeHeader *q;
};
}  // namespace Internal
}  // namespace Calendar

/**
  \class Calendar::DayRangeHeader
  \todo documentation
*/
DayRangeHeader::DayRangeHeader(QWidget *parent, int rangeWidth) :
    ViewWidget(parent),
    d_header(new Internal::DayRangeHeaderPrivate(this))
{
    d_header->m_rangeWidth = rangeWidth;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void DayRangeHeader::resetItemWidgets()
{
    d_header->computeWidgets();
	updateGeometry();
}

QSize DayRangeHeader::sizeHint() const
{
    int maxBottom = d_header->getLastWidgetBottom();
    return QSize(0, maxBottom == -1 ? d_header->getScaleHeight() + DayWidget::staticSizeHint().height() + 5 : maxBottom + DayWidget::staticSizeHint().height() + 5);
}

/** For a week, the range width is 7. */
int DayRangeHeader::rangeWidth() const
{
    return d_header->m_rangeWidth;
}

void DayRangeHeader::refreshItemsSizesAndPositions()
{
	// TEMP: reconstruct all at resize time
    d_header->computeWidgets();
	updateGeometry();
}

void DayRangeHeader::setRangeWidth(int width)
{
    if (width == d_header->m_rangeWidth)
		return;

    d_header->m_rangeWidth = width;
    d_header->computeWidgets();
	updateGeometry();
	update();
}

void DayRangeHeader::paintEvent(QPaintEvent *)
{
	// fill all in light blue
	QPainter painter(this);
	painter.fillRect(rect(), QColor(220, 220, 255));
    d_header->paintWidget(painter);
}

void DayRangeHeader::mousePressEvent(QMouseEvent *event) {

    if (event->pos().x() < 60 || event->pos().y() < d_header->getScaleHeight()) {
		QWidget::mousePressEvent(event);
		return;
	}

    d_header->m_pressDate = d_header->getDate(event->pos().x());
    d_header->m_previousDate = d_header->m_pressDate;
    d_header->m_pressPos = event->pos();

	// item under mouse?
    d_header->m_pressItemWidget = qobject_cast<DayWidget*>(childAt(event->pos()));
    if (d_header->m_pressItemWidget) {
        d_header->m_pressItem = model()->getItemByUid(d_header->m_pressItemWidget->uid());
        d_header->m_pressDayInterval = getIntersectDayRange(d_header->m_pressItem.beginning(), d_header->m_pressItem.ending());
        d_header->m_mouseMode = DayRangeHeaderPrivate::MouseMode_Move;
	} else {
        d_header->m_mouseMode = DayRangeHeaderPrivate::MouseMode_Creation;

        d_header->m_pressDayInterval.first = d_header->m_pressDate;
        d_header->m_pressDayInterval.second = d_header->m_pressDate;
		update();
	}
}

void DayRangeHeader::mouseMoveEvent(QMouseEvent *event) {

    if (d_header->m_mouseMode == DayRangeHeaderPrivate::MouseMode_None) {
		QWidget::mouseMoveEvent(event);
		return;
	}

    QDate date = d_header->getDate(event->pos().x());
    if (date == d_header->m_previousDate)
		return;

    d_header->m_previousDate = date;

    switch (d_header->m_mouseMode) {
    case DayRangeHeaderPrivate::MouseMode_Creation:
        if (d_header->m_previousDate < d_header->m_pressDate) {
            d_header->m_pressDayInterval.first = d_header->m_previousDate;
            d_header->m_pressDayInterval.second = d_header->m_pressDate;
		} else {
            d_header->m_pressDayInterval.first = d_header->m_pressDate;
            d_header->m_pressDayInterval.second = d_header->m_previousDate;
		}
		update();
		break;
    case DayRangeHeaderPrivate::MouseMode_Move:
	{
        if (!d_header->m_pressItemWidget->inMotion()) {
            d_header->m_pressItemWidget->setInMotion(true);
		}
        int dayCount = d_header->m_pressDayInterval.first.daysTo(d_header->m_pressDayInterval.second);
        QRect r = d_header->computeWidgetRect(date, date.addDays(dayCount - 1), d_header->m_maxDepth + 1);
        d_header->m_pressItemWidget->move(r.topLeft());
	}
	break;
	default:;
	}
}

void DayRangeHeader::mouseReleaseEvent(QMouseEvent *event) {
	QWidget::mouseReleaseEvent(event);

    QDate date = d_header->getDate(event->pos().x());
    if (d_header->m_mouseMode == DayRangeHeaderPrivate::MouseMode_Move) {
        if (!d_header->m_pressItemWidget->inMotion()) {
			// display a menu
			QMenu menu;
			QAction *modifyAction = menu.addAction(tr("modify"));
			connect(modifyAction, SIGNAL(triggered()), this, SLOT(modifyPressItem()));
			QAction *removeAction = menu.addAction(tr("remove"));
			connect(removeAction, SIGNAL(triggered()), this, SLOT(removePressItem()));
			menu.exec(event->globalPos());
		} else {
            int daysAdded = d_header->m_pressDayInterval.first.daysTo(date);
			if (daysAdded) {
                d_header->m_pressItem.setBeginning(d_header->m_pressItem.beginning().addDays(daysAdded));
                d_header->m_pressItem.setEnding(d_header->m_pressItem.ending().addDays(daysAdded));
			}
            d_header->computeWidgets();
			updateGeometry();
		}
    } else if (d_header->m_mouseMode == DayRangeHeaderPrivate::MouseMode_Creation) {
//		CalendarItem item = model()->insertItem(QDateTime(m_pressDayInterval.first, QTime(0, 0)),
//												QDateTime(m_pressDayInterval.second.addDays(1), QTime(0, 0)));
        CalendarItem item = CalendarItem(QDateTime(d_header->m_pressDayInterval.first, QTime(0, 0)),
                                         QDateTime(d_header->m_pressDayInterval.second.addDays(1), QTime(0, 0)));
		item.setDaily(true);
		model()->addCalendarItem(item);

        d_header->computeWidgets();
		updateGeometry();
	}

    d_header->m_mouseMode = DayRangeHeaderPrivate::MouseMode_None;
}

void DayRangeHeader::modifyPressItem()
{
    BasicItemEditorDialog dialog(model(), this);
    dialog.init(d_header->m_pressItem);
    if (dialog.exec() == QDialog::Accepted) {
        d_header->computeWidgets();
        updateGeometry();
    }
}

void DayRangeHeader::removePressItem()
{
    model()->removeItem(d_header->m_pressItem.uid());
    d_header->computeWidgets();
    updateGeometry();
}
