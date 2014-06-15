/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "month_day_widget.h"
#include <calendar/modelanditem/basic_item_edition_dialog.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QTimer>

using namespace Calendar;

MonthDayWidget::MonthDayWidget(AbstractCalendarModel *model, const QDate &day, QWidget *parent)
    : QWidget(parent),
      m_model(model),
      m_day(day)
{
}

void MonthDayWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    refreshItems();
}

void MonthDayWidget::refreshItems()
{
    // delete all
    QList<QWidget*> list;
    foreach (QObject *obj, children()) {
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget)
            list << widget;
    }
    qDeleteAll(list);

    m_items = m_model->getItemsBetween(m_day, m_day);
    qSort(m_items.begin(), m_items.end(), calendarItemLessThan);

    if (!m_items.count())
        return;

    QFont itemFont = QLabel().font(); // the font is a QLabel font classical => todo : change it and choose a better one
    itemFont.setPixelSize(10);
    QFont linkFont = itemFont;
    linkFont.setUnderline(true);
    int itemHeight = QFontMetrics(itemFont).height();
    int visibleItemsCount = this->height() / itemHeight;
    int top = 0;
    int count = visibleItemsCount < m_items.count() ? visibleItemsCount - 1 : m_items.count();

    for (int i = 0; i < count; i++) {
        const CalendarItem &item = m_items[i];
        QLabel *label = new QLabel(QString("<b>%1</b> %2").arg(item.beginning().time().toString("hh:mm")).arg(item.title().isEmpty() ? "(untitled)" : item.title()), this);
        label->setFont(itemFont);
        label->setCursor(Qt::PointingHandCursor);
        label->move(0, top);
        label->show();
        m_uidByWidget.insert(label, item.uid());
        label->installEventFilter(this);
        top += itemHeight;
    }

    if (count != m_items.count()) { // add a link
        QLabel *label = new QLabel(tr("see others"), this);
        label->setFont(linkFont);
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, Qt::darkBlue);
        label->setPalette(palette);
        label->setCursor(Qt::PointingHandCursor);
        label->move(0, top);
        label->show();
    }
}

bool MonthDayWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel *label = qobject_cast<QLabel*>(obj);
        QString uid = m_uidByWidget[label];
        CalendarItem *item = getItemByUid(uid);
        BasicItemEditorDialog dialog(m_model, this);
        dialog.init(*item);
        if (dialog.exec() == QDialog::Accepted) {
            // refresh cell
            QTimer::singleShot(0, this, SLOT(refreshItems()));

            // TODO: potentially refresh other cells which contains this modified item
        }
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

CalendarItem *MonthDayWidget::getItemByUid(const QString &uid)
{
    for (int i = 0; i < m_items.count(); i++) {
        CalendarItem &item = m_items[i];
        if (item.uid() == uid)
            return &item;
    }
    return 0;
}
