/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#ifndef TKSCROLLINGWIDGET_H
#define TKSCROLLINGWIDGET_H

#include <tkExporter.h>

class tkScrollingWidgetPrivate;

#include <QWidget>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QShowEvent>
#include <QHideEvent>
/**
 * \file tkScrollingWidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.3
 * \date 10 June 2009
*/

class Q_TK_EXPORT tkScrollingWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QString text READ text WRITE setText)
    Q_PROPERTY( int timerDelay READ timerDelay WRITE setTimerDelay )

public:

    /** */
    enum Direction {
        LeftToRight = 0,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };

    /** */
    tkScrollingWidget( QWidget *parent = 0 );
    ~tkScrollingWidget();

    /** */
    void setText( const QString &text);
    /** */
    QString text();
    /** */
    QSize sizeHint() const;

    /** */
    void setDirection( int direction );

    /** */
    void setTimerDelay( const int delay ); // ms
    /** */
    int timerDelay(); // ms

private:
    void paintEvent( QPaintEvent *event);
    void timerEvent( QTimerEvent *event);
    void showEvent( QShowEvent *event);
    void hideEvent( QHideEvent *event);

private:
    tkScrollingWidgetPrivate *d;
};

#endif // TKSCROLLINGWIDGET_H
