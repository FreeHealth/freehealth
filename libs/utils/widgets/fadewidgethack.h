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
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_FADERWIDGET_H
#define UTILS_FADERWIDGET_H

#include <utils/global_exporter.h>

#include <QWidget>
#include <QColor>

/**
 * \file fadewidgethack.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 02 Sept 2012
*/

namespace Utils {

class UTILS_EXPORT FaderWidget : public QWidget
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

#endif // UTILS_FADERWIDGET_H
