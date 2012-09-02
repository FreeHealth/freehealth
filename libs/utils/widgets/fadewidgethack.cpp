/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/
#include "fadewidgethack.h"

#include <QPainter>
#include <QTimer>

#include <QDebug>

namespace Utils {
void FadeWidgetHack::paintEvent(QPaintEvent *)
{
    if (m_opacity == 0)
        return;

    QPainter p(this);
    p.setOpacity(m_opacity);
    if (m_opacity > 0)
        Utils::StyleHelper::verticalGradient(&p, rect(), rect());
}


FaderWidget::FaderWidget(QWidget *parent)
    : QWidget(parent)
{
    if (parent)
        startColor = parent->palette().window().color();
    else
        startColor = Qt::white;

    currentAlpha = 0;
    duration = 333;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    setAttribute(Qt::WA_DeleteOnClose);
    resize(parent->size());
}

void FaderWidget::start()
{
    if (_type == FadeInParentWidget) {
        currentAlpha = 255;
    } else if (_type == FadeOutParentWidget) {
        currentAlpha = 0;
    }
    int cycle = duration/5;
    while (cycle>100) cycle /= 5;
    timer->start(cycle);
    show();
}

void FaderWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QColor semiTransparentColor = startColor;
    semiTransparentColor.setAlpha(currentAlpha);
    painter.fillRect(rect(), semiTransparentColor);

    if (_type == FadeInParentWidget) {
        currentAlpha -= 255 * timer->interval() / duration;
        if (currentAlpha <= 0) {
            timer->stop();
            hide();
            close();
            Q_EMIT fadeDone();
        }
    } else if (_type == FadeOutParentWidget) {
        currentAlpha += 255 * timer->interval() / duration;
        if (currentAlpha >= 255) {
            timer->stop();
            hide();
            close();
            Q_EMIT fadeDone();
        }
    }
}


}  // namespace Utils
