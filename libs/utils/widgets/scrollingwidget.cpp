/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/**
  \class ScrollingWidget
  Simple QWidget with scrolling text content.
*/

#include "scrollingwidget.h"

#include <QPainter>
#include <QDebug>

using namespace Utils;
using namespace Utils::Internal;

namespace Utils {
namespace Internal {
class ScrollingWidgetPrivate
{
public:
    QString m_Text;
    int m_XOffset,m_YOffset;
    int m_TimerId;
    int m_TimerDelay;
    int m_Direction;
    QSize m_TextSize;
};
}
}

ScrollingWidget::ScrollingWidget( QWidget *parent ) :
    QWidget(parent), d(0)
{
    d = new ScrollingWidgetPrivate();
    d->m_XOffset = 0;
    d->m_YOffset = 0;
    d->m_TimerId = 0;
    d->m_Direction = LeftToRight;
    d->m_Text.clear();
    d->m_TimerDelay = 30;
}

ScrollingWidget::~ScrollingWidget()
{
    if (d) {
        delete d;
        d=0;
    }
}

void ScrollingWidget::setDirection( int direction )
{
    d->m_Direction = direction;
}

void ScrollingWidget::setText( const QString &text)
{
    d->m_Text = text;
    d->m_TextSize = fontMetrics().size(0, text );
    update();
    updateGeometry();
}

QString ScrollingWidget::text()
{
    return d->m_Text;
}

QSize ScrollingWidget::sizeHint() const
{
    return fontMetrics().size(0, d->m_Text);
}

void ScrollingWidget::setTimerDelay( const int delay )
{
    d->m_TimerDelay = delay;
    if (d->m_TimerId != 0)
        d->m_TimerId = startTimer(d->m_TimerDelay);
}

int ScrollingWidget::timerDelay()
{
    return d->m_TimerDelay;
}

void ScrollingWidget::paintEvent( QPaintEvent *event)
{
    Q_UNUSED(event);
    if (d->m_Text.isEmpty())
        return;
    QPainter painter(this);
    if (d->m_TextSize.width() < 1)
        return;

    switch (d->m_Direction)
    {
    case LeftToRight :
    case RightToLeft :
    {
        int x = -d->m_XOffset;
        while (x < width()) {
            painter.drawText( x, 0, d->m_TextSize.width(), height(), Qt::AlignLeft | Qt::AlignVCenter, d->m_Text );
            x += d->m_TextSize.width() + 10;
        }
        break;
    }
    case TopToBottom :
    case BottomToTop :
    {
        int y = -d->m_YOffset -d->m_TextSize.height();
        while (y < (height() + d->m_TextSize.height())) {
            painter.drawText( 0, y, d->m_TextSize.width(), d->m_TextSize.height(), Qt::AlignLeft | Qt::AlignVCenter, d->m_Text );
            y += d->m_TextSize.height();
        }
        break;
    }
    }

}

void ScrollingWidget::timerEvent( QTimerEvent *event)
{
    if (event->timerId() == d->m_TimerId) {
        switch (d->m_Direction)
        {
        case LeftToRight :
        {
            ++d->m_XOffset;
            if (d->m_XOffset >= d->m_TextSize.width())
                d->m_XOffset = 0;
            scroll(-1,0);
            break;
        }
        case RightToLeft :
        {
            ++d->m_XOffset;
            if (d->m_XOffset >= d->m_TextSize.width())
                d->m_XOffset = 0;
            scroll(1,0);
            break;
        }
        case TopToBottom :
        {
            ++d->m_YOffset;
            if (d->m_YOffset >= d->m_TextSize.height())
                d->m_YOffset = 0;
            scroll(0,1);
            break;
        }
        case BottomToTop :
        {
            ++d->m_YOffset;
            if (d->m_YOffset >= d->m_TextSize.height())
                d->m_YOffset = 0;
            scroll(0,-1);
            break;
        }
        } // End switch

    } else {
        QWidget::timerEvent(event);
    }
}

void ScrollingWidget::showEvent( QShowEvent *event)
{
    Q_UNUSED(event);
    d->m_TimerId = startTimer(d->m_TimerDelay);
}

void ScrollingWidget::hideEvent( QHideEvent *event)
{
    Q_UNUSED(event);
    killTimer(d->m_TimerId);
}
