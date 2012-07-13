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

/*!
 * \brief Creates an QOpenCVWidget and initializes the camera capturing
 * \param parent
 */
OpenCVWidget::OpenCVWidget(QWidget *parent) :
    QLabel(parent),
    m_frozen(false),
    m_updateFreq(defaultUpdateFrequency()),
    m_rubberBand(0),
    m_Mode(Create)
{
    m_camera = cvCreateCameraCapture(CV_CAP_ANY);
    Q_ASSERT(m_camera); //TODO: don't assert here, do error handling while runnning

    // Log some info about webcam image
    IplImage *image = cvQueryFrame(m_camera);
    Q_ASSERT(image);

//    qDebug() << "Image depth=%i" << image->depth;
//    qDebug() <<"Image nChannels=%i" << image->nChannels;

    // Create the QImage
    m_image = QImage(100,100,QImage::Format_RGB32);
    for (int x = 0; x < 100; x ++) {
        for (int y =0; y < 100; y++) {
            m_image.setPixel(x,y,qRgb(x, y, y));
        }
    }
    setPixmap(QPixmap::fromImage(m_image));
    m_timerId = startTimer(m_updateFreq);
}

/*!
 * \brief Releases the openCV camera capturing.
 */
OpenCVWidget::~OpenCVWidget()
{
    cvReleaseCapture(&m_camera);
}

void OpenCVWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    IplImage *cvimage = cvQueryFrame(m_camera);

    int cvIndex, cvLineStart;
    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3:
            if ( (cvimage->width != m_image.width()) || (cvimage->height != m_image.height()) ) {
                QImage temp(cvimage->width, cvimage->height, QImage::Format_RGB32);
                m_image = temp;
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

                    m_image.setPixel(x,y,qRgb(red, green, blue));
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
    setPixmap(QPixmap::fromImage(m_image));
}

/*!
 * \brief Takes care of mouse clicks in the capturing phase.
 * 
 * Starts the rubberband, checks where the user has clicked.
 * \param event
 */
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
        m_Mode = Create;
    }

    // remember the position where the user clicked and the QRubberBand was then
    m_clickOrigin = event->pos();
    m_rubberOrigin = m_rubberBand->pos();

    // determine if the user has clicked *into* the QRubberBand boundaries
    if (m_rubberBand->rect().translated(m_rubberBand->pos()).contains(m_clickOrigin, true)) {
        m_Mode = Move;
    } else {
        m_Mode = Create;
        // create an empty frame and show it
        m_rubberBand->setGeometry(QRect(m_clickOrigin, QSize()));
        m_rubberBand->show();
    }
}

/*!
 * \brief takes care of the mouse movement when creating or dragging the rubberband frame.
 * \param event 
 */
void OpenCVWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_frozen || !m_rubberBand)
        return;

    switch (m_Mode) {
    
    case Create: {
        // calculate the rectangle
        QRect rect = QRect(m_clickOrigin, event->pos());
        rect.setHeight(rect.height() > 0? abs(rect.width()) : -abs(rect.width()));
        
        // get rid of negative coordinates
        m_rubberBand->setGeometry(rect.normalized());
        restrictRubberBandConstraints();
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

/*!
 * \brief checks if there is a valid rubberband and fires the imageReady(true) signal then.
 * \param event
 */
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

/*!
 * \brief Catches the mouse wheel events and resizes the rubberband.
 * \param event QWheelEvent - method delta() is used to determind if mouse wheel is scrolled up/down
 */
void OpenCVWidget::wheelEvent(QWheelEvent *event)
{
    // only working when frozen and with active rubberband!
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

/*!
 * \brief Crops the RubberBand if not inside the parent widget.
 */
void OpenCVWidget::restrictRubberBandConstraints()
{
    // restrict new RubberBand to constraints of parent
    
    // is too big
    QRect rect = m_rubberBand->geometry().normalized();
    if (rect.height() > this->rect().height()) {
        rect.setHeight(this->rect().height()-2);
        rect.setWidth(this->rect().height()-2);
    }
    // check if possibly width < height, we have to crop again then
    // and take the width as smallest distance
    if (    this->rect().width() < this->rect().height() && 
            rect.width() > this->rect().width()) 
    {
        rect.setHeight(this->rect().width()-2);
        rect.setWidth(this->rect().width()-2);
    }        
    
    m_rubberBand->setGeometry(rect);    
    
    // is too far left
    if (m_rubberBand->x() < 0)
        m_rubberBand->move(1, m_rubberBand->y()-1);
    // is too far right
    if (m_rubberBand->geometry().right() > this->rect().right())
        m_rubberBand->move(this->rect().right() - m_rubberBand->width(), m_rubberBand->y());

    // ist too far to the top
    if (m_rubberBand->y() < 0)
        m_rubberBand->move(m_rubberBand->x()-1, 1);
    // is too far to the bottom
    if (m_rubberBand->geometry().bottom() > this->rect().bottom())
        m_rubberBand->move(m_rubberBand->x(), this->rect().bottom() - m_rubberBand->height());
    

}

/*!
 * \brief When called, stops the camera and freezes the picture. In the next call it continues 
 * the camera again.
 */
void OpenCVWidget::toggleFreezeMode()
{
    if (m_frozen) {
        // Unfreeze
        if (m_rubberBand)
            m_rubberBand->hide();
        m_timerId = startTimer(m_updateFreq);  // 0.1-second timer
    } else {
        // Freeze
        if (m_timerId > 0)
            killTimer(m_timerId);
    }
    m_frozen = !m_frozen;
    Q_EMIT frozen(m_frozen);
}

void OpenCVWidget::setImageUpdateFrequency(const int ms)
{
    Q_ASSERT(ms > 0);
    if (m_timerId > 0)
        killTimer(m_timerId);
    m_updateFreq = ms;
    startTimer(m_updateFreq);
}

int OpenCVWidget::defaultUpdateFrequency() const
{
    return 50;
}

/*!
 * \brief returns a QRect with the coordinates of the current selection
 */
QRect OpenCVWidget::frame() const
{
    if (!m_frozen || !m_rubberBand || !m_rubberBand->geometry().isValid())
        return QRect();
    return m_rubberBand->geometry();
}
