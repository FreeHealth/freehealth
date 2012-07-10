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
#include "renderwidget.h"
#include <QPainter>
#include <QDebug>
#include <QTime>

using namespace Webcam;

RenderWidget::RenderWidget(QWidget* parent) :
    QWidget(parent),
    Filter(),
    imageData(0),
    imageWidth(0),
    imageHeight(0)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true); // don't clear the area before the paintEvent
    setAttribute(Qt::WA_PaintOnScreen, true); // disable double buffering
    //setFixedSize(imageWidth, imageHeight);
    connect(this, SIGNAL(frameSizeChanged(int, int)), this, SLOT(onFrameSizeChanged(int, int)), Qt::QueuedConnection);
    time = frames = 0;
}

void RenderWidget::onFrameSizeChanged(int width, int height)
{
    setFixedSize(width, height);
} 

/*!
 * \brief updates the Pixmap in the Widget
 *
 * This function checks if the image size is still the same (if not it adapts the widget's pixmap),
 * then updates the pixmap with the given IplImage.
 * @param frame const pointer to an IplImage.
 */
void RenderWidget::updatePixmap(const IplImage* frame)
{
    QTime t;
    t.start();
    bool start = false;
    // check if the frame dimensions have changed
    if(frame->width != imageWidth || frame->height != imageHeight) {
        if(imageData) {
            delete[] imageData;
        }
        start = true;
        imageWidth = frame->width;
        imageHeight = frame->height;
        emit(frameSizeChanged(imageWidth, imageHeight));
        imageData = new unsigned char[4*imageWidth*imageHeight];
        for(int i = 0; i < imageWidth*imageHeight; i++) {
            imageData[i*4+3] = 0xFF;
        }
    }

    int pixels = imageWidth * imageHeight;
    uchar* src = (uchar*)(frame->imageData);
    uchar* srcEnd = src + (3*pixels);
    uchar* dest = imageData;

    do {
        memcpy(dest, src, 3);
        dest += 4;
        src += 3;
    } while(src < srcEnd);

    if(!start) {
        ++frames;
        time += t.elapsed();
    }
}

void RenderWidget::processPoint(HeadState* state)
{
    // copy the image to the local pixmap and update the display
    updatePixmap(state->frame);
    update();

    notifyListener(state);
}

void RenderWidget::paintEvent(QPaintEvent*)
{

    QPainter painter(this);
    if(imageData) {
        QImage tImg(imageData, imageWidth, imageHeight, QImage::Format_RGB32);
        painter.drawImage(QPoint(0,0), tImg);
    }
    else {
        painter.setBrush(Qt::black);
        painter.drawRect(rect());
    }
}
