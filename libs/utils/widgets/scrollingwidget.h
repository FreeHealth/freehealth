/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef SCROLLINGWIDGET_H
#define SCROLLINGWIDGET_H

#include <utils/global_exporter.h>

#include <QWidget>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QShowEvent>
#include <QHideEvent>
/**
 * \file ScrollingWidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.3
 * \date 10 June 2009
*/

namespace Utils {
namespace Internal {
class ScrollingWidgetPrivate;
}

class UTILS_EXPORT ScrollingWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QString text READ text WRITE setText)
    Q_PROPERTY( int timerDelay READ timerDelay WRITE setTimerDelay )

public:
    enum Direction {
        LeftToRight = 0,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };

    ScrollingWidget( QWidget *parent = 0 );
    ~ScrollingWidget();

    void setText( const QString &text);
    QString text();
    QSize sizeHint() const;

    void setDirection( int direction );

    void setTimerDelay( const int delay ); // ms
    int timerDelay(); // ms

private:
    void paintEvent( QPaintEvent *event);
    void timerEvent( QTimerEvent *event);
    void showEvent( QShowEvent *event);
    void hideEvent( QHideEvent *event);

private:
    Internal::ScrollingWidgetPrivate *d;
};

}  // End Utils

#endif // SCROLLINGWIDGET_H
