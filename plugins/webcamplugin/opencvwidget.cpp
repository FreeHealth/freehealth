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
#include "opencvwidget.h"
#include <QMouseEvent>
#include <QtGui>

using namespace Webcam::Internal;

OpenCVWidget::OpenCVWidget(QWidget *parent) :
    QLabel(parent),
    m_frozen(false),
    _updateFreq(defaultUpdateFrequency()),
    m_rubberBand(0),
    rbMode(Create)
{
    _camera = cvCreateCameraCapture(CV_CAP_ANY);
    Q_ASSERT(_camera);

    // Log some info about webcam image
    IplImage *image = cvQueryFrame(_camera);
    Q_ASSERT(image);
    printf("Image depth=%i\n", image->depth);
    printf("Image nChannels=%i\n", image->nChannels);

    // Create the QImage
    _image = QImage(100,100,QImage::Format_RGB32);
    for (int x = 0; x < 100; x ++) {
        for (int y =0; y < 100; y++) {
            _image.setPixel(x,y,qRgb(x, y, y));
        }
    }
    setPixmap(QPixmap::fromImage(_image));
    _timerId = startTimer(_updateFreq);
}

OpenCVWidget::~OpenCVWidget()
{
    cvReleaseCapture(&_camera);
}

void OpenCVWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    IplImage *cvimage = cvQueryFrame(_camera);

    int cvIndex, cvLineStart;
    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3:
            if ( (cvimage->width != _image.width()) || (cvimage->height != _image.height()) ) {
                QImage temp(cvimage->width, cvimage->height, QImage::Format_RGB32);
                _image = temp;
            }
            cvIndex = 0; cvLineStart = 0;
            for (int y = 0; y < cvimage->height; y++) {
                unsigned char red,green,blue;
                cvIndex = cvLineStart;
                for (int x = 0; x < cvimage->width; x++) {
                    // DO it
                    red = cvimage->imageData[cvIndex+2];
                    green = cvimage->imageData[cvIndex+1];
                    blue = cvimage->imageData[cvIndex+0];

                    _image.setPixel(x,y,qRgb(red, green, blue));
                    cvIndex += 3;
                }
                cvLineStart += cvimage->widthStep;
            }
            break;
        default:
            printf("This number of channels is not supported\n");
            break;
        }
        break;
    default:
        printf("This type of IplImage is not implemented in QOpenCVWidget\n");
        break;
    }
    setPixmap(QPixmap::fromImage(_image));
}

void OpenCVWidget::mousePressEvent(QMouseEvent *event)
{
    // restrict to left mouse button and frozen state
    if (event->button() != Qt::LeftButton)
        return;
    
    if(!m_frozen) {
//        toggleFreezeMode();
        return;
    }
    
    // if there is no QRubberBand, create one
    if (!m_rubberBand) {
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        m_rubberBand->setWindowFlags(Qt::ToolTip);
        rbMode = Create;
    }

    // remember the position where the user clicked and the QRubberBand was then
    m_clickOrigin = event->pos();
    m_rubberOrigin = m_rubberBand->pos();

    // determine if the user has clicked *into* the QRubberBand boundaries
    if (m_rubberBand->rect().translated(m_rubberBand->pos()).contains(m_clickOrigin, true)) {
        rbMode = Move;
    } else {
        rbMode = Create;
        // create an empty frame and show it
        m_rubberBand->setGeometry(QRect(m_clickOrigin, QSize()));
        m_rubberBand->show();
    }
}

void OpenCVWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_frozen || !m_rubberBand)
        return;

    switch (rbMode) {
    
    case Create: {
        // calculate the rectangle
        QRect rect = QRect(m_clickOrigin, event->pos());
        rect.setHeight(rect.height() > 0? abs(rect.width()) : -abs(rect.width()));
        
        // get rid of negative coordinates
        m_rubberBand->setGeometry(rect.normalized());
        break;
    }

    case Move: {
        // remember the distance vector between the actual mouse pos and the original click
        QPoint relativePosFromClick = event->pos() - m_clickOrigin;
        // now relatively move the rectangle to that vector
        m_rubberBand->move(m_rubberOrigin + relativePosFromClick);

        // check if we are not out of the parent widget boundaries
        restrictRubberBandConstraints();
        break;
    }
    }
}


void OpenCVWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!m_frozen | !m_rubberBand) {
        return;
    }
    if (m_rubberBand->rect().isValid()) {
        // we have a valid rubberband square
        Q_EMIT imageReady(true);
    } else {
        Q_EMIT imageReady(false);
    }
}

void OpenCVWidget::wheelEvent(QWheelEvent *event)
{
    //only working when frozen and with active rubberband!
    if (!m_frozen || !m_rubberBand)
        return;

    if (event->delta() > 0 && 
            m_rubberBand->width()+4 < this->rect().width() &&
            m_rubberBand->height()+4 < this->rect().height()) { // WheelUp
        m_rubberBand->move(m_rubberBand->geometry().x()-2, m_rubberBand->y()-2);
        m_rubberBand->resize(m_rubberBand->geometry().width()+4, m_rubberBand->height()+4);
        restrictRubberBandConstraints();        
    } else { // WheelDown
        if (m_rubberBand->width() >= 24) {
            m_rubberBand->move(m_rubberBand->geometry().x()+2, m_rubberBand->y()+2);
            m_rubberBand->resize(m_rubberBand->geometry().width()-4, m_rubberBand->height()-4);
        }
        restrictRubberBandConstraints();
    }
}

void OpenCVWidget::restrictRubberBandConstraints()
{
    // restrict new RubberBand to constraints of parent
    if (m_rubberBand->x() < 0)
        m_rubberBand->move(0, m_rubberBand->y());
    if (m_rubberBand->geometry().right() > this->rect().right())
        m_rubberBand->move(this->rect().right() - m_rubberBand->width(), m_rubberBand->y());

    if (m_rubberBand->y() < 0)
        m_rubberBand->move(m_rubberBand->x(), 0);
    if (m_rubberBand->geometry().bottom() > this->rect().bottom())
        m_rubberBand->move(m_rubberBand->x(), this->rect().bottom() - m_rubberBand->height());
    
}

void OpenCVWidget::toggleFreezeMode()
{
    if (m_frozen) {
        // Unfreeze
        if (m_rubberBand)
            m_rubberBand->hide();
        _timerId = startTimer(_updateFreq);  // 0.1-second timer
    } else {
        // Freeze
        if (_timerId > 0)
            killTimer(_timerId);
    }
    m_frozen = !m_frozen;
//    Q_EMIT frozen(m_frozen);
}

void OpenCVWidget::setImageUpdateFrequency(const int ms)
{
    Q_ASSERT(ms > 0);
    if (_timerId > 0)
        killTimer(_timerId);
    _updateFreq = ms;
    startTimer(_updateFreq);
}

int OpenCVWidget::defaultUpdateFrequency() const
{
    return 50;
}
