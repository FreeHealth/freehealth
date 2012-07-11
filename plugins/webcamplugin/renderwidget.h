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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QWidget>
#include <filter.h>

namespace Webcam {

class HeadState;

/*!
 * \class RenderWidget
 * \brief A Widget that displays frames from a OpenCV video source
 */
class RenderWidget : public QWidget, public Filter {

    Q_OBJECT

public:
    RenderWidget(QWidget* parent);
    void processPoint(HeadState* state);
    QPixmap currentPixmap() const;
public slots:
    void onFrameSizeChanged(int width, int height);
signals:
    void frameSizeChanged(int width, int height);
//    void clicked();

protected:
    void paintEvent(QPaintEvent*);
private:
    void updatePixmap(const IplImage* frame);
    uchar* imageData;
    int imageWidth, imageHeight;
    int time;
    int frames;
};

}

#endif
