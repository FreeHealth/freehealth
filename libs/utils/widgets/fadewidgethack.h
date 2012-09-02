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
#ifndef FADEWIDGETHACK_H
#define FADEWIDGETHACK_H

#include <utils/global_exporter.h>
#include <utils/stylehelper.h>

#include <QWidget>
#include <QColor>

namespace Utils {

class UTILS_EXPORT FadeWidgetHack : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    FadeWidgetHack(QWidget *parent = 0):QWidget(parent), m_opacity(0)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    void paintEvent(QPaintEvent *);

    void setOpacity(float o) { m_opacity = o; update(); }
    float opacity() const { return m_opacity; }

private:
    float m_opacity;
};

class FaderWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor fadeColor READ fadeColor WRITE setFadeColor)
    Q_PROPERTY(int fadeDuration READ fadeDuration WRITE setFadeDuration)

public:
    enum FadeType {
        FadeOutParentWidget,
        FadeInParentWidget
    };

    FaderWidget(QWidget *parent);

    void setFadeType(FadeType type) {_type = type;}

    QColor fadeColor() const { return startColor; }
    void setFadeColor(const QColor &newColor) { startColor = newColor; }

    int fadeDuration() const { return duration; }
    void setFadeDuration(int milliseconds) { duration = milliseconds; }

    void start();

protected:
    void paintEvent(QPaintEvent *event);

Q_SIGNALS:
    void fadeDone();

private:
    QTimer *timer;
    QColor startColor;
    int currentAlpha;
    int duration;
    FadeType _type;
};
}  // namespace Utils

#endif // FADEWIDGETHACK_H
