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
#include "hour_range_widget.h"
#include <calendar/modelanditem/abstract_calendar_model.h>

#include <QPainter>
#include <QMouseEvent>
#include <QColor>

using namespace Calendar;
using namespace Internal;

/**
 * \class Calendar::Internal::HourRangeWidget
 * This widget represents an hour defined calendar event.
*/

/**
 * Constructor: creates a new hour widget.
 * If the \e uid is empty, this calendar item is considered as temporary and will be drawn with transparence
*/
HourRangeWidget::HourRangeWidget(QWidget *parent, const QString &uid, AbstractCalendarModel *model) :
    CalendarItemWidget(parent, uid, model), m_aboveWidget(0)
{
    m_inMotion = uid.isEmpty();
    setMouseTracking(true);
    connect(model, SIGNAL(dataChanged(Calendar::CalendarItem)), this, SLOT(update()));
}

void HourRangeWidget::paintEvent(QPaintEvent *)
{
    // title
    QPixmap titlePixmap(size().width(), 20);
    titlePixmap.fill(Qt::transparent);
    QPainter titlePainter(&titlePixmap);
    titlePainter.setRenderHint(QPainter::Antialiasing);
    titlePainter.setPen(Qt::NoPen);
    QBrush brush = titlePainter.brush();
    brush.setStyle(Qt::SolidPattern);
    QColor baseColor = model()->data(model()->getItemByUid(uid()), AbstractCalendarModel::Status, Qt::BackgroundRole).value<QColor>();;
    m_inMotion ? baseColor.setAlpha(200) : baseColor.setAlpha(255);
    brush.setColor(baseColor);
    titlePainter.setBrush(brush);
    titlePainter.drawRoundedRect(QRect(0, 0, width(), 20), 5, 5);

    // text
    QRect r = rect();
    r.setHeight(15);
    r.adjust(2, 0, 0, 0);
    QPen fontPen;
    fontPen.setColor(Qt::white);
    titlePainter.setPen(fontPen);
    QFont font = titlePainter.font();
    font.setPixelSize(10);
    font.setBold(true);
    titlePainter.setFont(font);
    titlePainter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, QString("%1 - %2").arg(beginDateTime().time().toString("hh:mm")).arg(endDateTime().time().toString("hh:mm")));

    // body
    QPixmap bodyPixmap(size().width(), rect().height());
    bodyPixmap.fill(Qt::transparent);
    QPainter bodyPainter(&bodyPixmap);
    bodyPainter.setRenderHint(QPainter::Antialiasing);
    QColor lightBase = baseColor;
    lightBase.lighter();
    bodyPainter.setPen(lightBase);
    brush = bodyPainter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(baseColor);
    bodyPainter.setBrush(brush);
    bodyPainter.drawRoundedRect(QRect(0, 0, width(), height()), 5, 5);
    if (model()) {
        CalendarItem item = model()->getItemByUid(uid());
        if (item.isValid()) {
            bodyPainter.setPen(Qt::white);
            QString label = item.title().isEmpty() ? tr("(untitled)") : item.title();
            bodyPainter.drawText(QRect(2, 20, width() - 3, height()), Qt::TextWordWrap | Qt::AlignLeft, label);
        }
    }

    // main
    QPainter painter(this);
    painter.drawPixmap(QPoint(0, 0), titlePixmap, QRect(0, 0, width(), 15));
    painter.drawPixmap(QPoint(0, 15), bodyPixmap, QRect(0, 15, width(), rect().height() - 15));
}

void HourRangeWidget::setInMotion(bool value)
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

void HourRangeWidget::mouseMoveEvent(QMouseEvent *event)
{
    // mouse at bottom edge of widget
    if ((event->pos().y() >= height() - 5 && event->pos().y() < height()) ||
            // or at top edge
            (event->pos().y() >= rect().y() && event->pos().y() < rect ().y() + 5))
    {
        setCursor(Qt::SizeVerCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseMoveEvent(event);
}

/*void HourRangeWidget::mouseReleaseEvent(QMouseEvent *event) {
// display a menu
QMenu menu;
QAction *modifyAction = menu.addAction(tr("modify"));
QAction *removeAction = menu.addAction(tr("remove"));
menu.exec(event->globalPos());
}*/
