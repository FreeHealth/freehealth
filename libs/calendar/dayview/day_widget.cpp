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
#include "day_widget.h"
#include <calendar/modelanditem/abstract_calendar_model.h>

#include <QPainter>
#include <QMouseEvent>

using namespace Calendar;
using namespace Internal;

DayWidget::DayWidget(QWidget *parent, const QString &uid, AbstractCalendarModel *model)
    : CalendarItemWidget(parent, uid, model), m_aboveWidget(0)
{
	m_titleFont = getTitleFont();
	m_inMotion = uid.isEmpty();
	setMouseTracking(true);
//	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize DayWidget::sizeHint() const
{
	return QSize(0, QFontMetrics(m_titleFont).height() + 4);
}

QFont DayWidget::getTitleFont()
{
	return QFont();
}

QSize DayWidget::staticSizeHint()
{
	return QSize(0, QFontMetrics(getTitleFont()).height() + 4);
}

void DayWidget::paintEvent(QPaintEvent *)
{
        QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	QBrush brush = painter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 150, 0, m_inMotion ? 200 : 255));
	painter.setBrush(brush);

	painter.drawRoundedRect(rect(), 4, 4);
	painter.setPen(Qt::white);
	if (model()) {
		CalendarItem item = model()->getItemByUid(uid());
		painter.drawText(rect().adjusted(2, 0, -2, 0), Qt::AlignVCenter | Qt::AlignLeft, item.title().isEmpty() ? tr("(untitled)") : item.title());
	}
}

void DayWidget::setInMotion(bool value)
{
	if (m_inMotion == value)
		return;

	m_inMotion = value;

	if (m_inMotion) { // record the above widget and put itself in the stack top
		QWidget *parent = parentWidget();
		m_aboveWidget = 0;
		for (int index = parent->children().indexOf(this) + 1; index < parent->children().count(); index++) {
			QWidget *widget = qobject_cast<QWidget*>(parent->children()[index]);
			if (widget) {
				m_aboveWidget = widget;
				break;
			}
		}
		raise();
	} else { // restore the widget in the stack
		if (m_aboveWidget)
			stackUnder(m_aboveWidget);
	}
}
