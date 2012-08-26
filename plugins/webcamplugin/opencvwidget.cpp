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
 *   Main Developers:                                                      *
 *       Christian A. Reiter <christian.a.reiter@gmail.com>                *
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "opencvwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>

#include <QMouseEvent>
#include <QProgressDialog>
#include <QImage>
#include <QPixmap>
#include <QTime>

enum { WarnCameraProperties = false };

using namespace Webcam::Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

/*!
 * \brief Creates an QOpenCVWidget and initializes the camera capturing
 * \param parent
 */
OpenCVWidget::OpenCVWidget(QWidget *parent) :
    QLabel(parent),
    m_frozen(false),
    m_updateFreq(defaultUpdateFrequency()),
    m_rubberBand(0),
    m_Mode(Create),
    m_counter(0),
    m_frames(0)
{
    setObjectName("OpenCVWidget");

    QTime time;
    time.start();

    {
        QProgressDialog dlg(this);
        dlg.setRange(0, 0);
        dlg.setValue(0);
        dlg.setLabelText(tr("Acquiring webcam..."));
        dlg.show();

        m_camera = cvCreateCameraCapture(CV_CAP_ANY);
        Q_ASSERT(m_camera); //TODO: don't assert here, do error handling while runnning
    }
    LOG(tr("Acquiring WebCam (%1 ms)").arg(time.elapsed()));

    if (WarnCameraProperties) {
        qWarning()
                << "\nCV_CAP_PROP_POS_MSEC"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_POS_MSEC)
                << "\nCV_CAP_PROP_POS_FRAMES"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_POS_FRAMES)
                << "\nCV_CAP_PROP_POS_AVI_RATIO"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_POS_AVI_RATIO)
                << "\nCV_CAP_PROP_FRAME_WIDTH"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_FRAME_WIDTH)
                << "\nCV_CAP_PROP_FRAME_HEIGHT"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_FRAME_HEIGHT)
                << "\nCV_CAP_PROP_FPS"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_FPS)
                << "\nCV_CAP_PROP_FOURCC"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_FOURCC)
                << "\nCV_CAP_PROP_FRAME_COUNT"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_FRAME_COUNT)
                << "\nCV_CAP_PROP_FORMAT"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_FORMAT)
                << "\nCV_CAP_PROP_MODE"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_MODE)
                << "\nCV_CAP_PROP_BRIGHTNESS"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_BRIGHTNESS)
                << "\nCV_CAP_PROP_CONTRAST"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_CONTRAST)
                << "\nCV_CAP_PROP_SATURATION"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_SATURATION)
                << "\nCV_CAP_PROP_HUE"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_HUE)
                << "\nCV_CAP_PROP_GAIN"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_GAIN)
                << "\nCV_CAP_PROP_EXPOSURE"
                << cvGetCaptureProperty(m_camera, CV_CAP_PROP_EXPOSURE) ;
    }


    // face recognition initialisation
    QString filename = settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/haarcascade_frontalface_alt2.xml";
    _cascade =  (CvHaarClassifierCascade*)cvLoad(filename.toUtf8());

    _storage = cvCreateMemStorage(0);
    _colors << cvScalar(0.0,0.0,255.0) << cvScalar(0.0,128.0,255.0)
            << cvScalar(0.0,255.0,255.0) << cvScalar(0.0,255.0,0.0)
            << cvScalar(255.0,128.0,0.0) << cvScalar(255.0,255.0,0.0)
            << cvScalar(255.0,0.0,0.0) << cvScalar(255.0,0.0,255.0);

    // create a model that contains the captured images
    // 8 images, 1 column (image)
    m_imageModel = new QStandardItemModel(this);

    m_timerId = startTimer(m_updateFreq);
}

/*!
 * \brief Releases the openCV camera capturing.
 */
OpenCVWidget::~OpenCVWidget()
{
    cvReleaseHaarClassifierCascade(&_cascade);
    cvReleaseCapture(&m_camera);
}

void OpenCVWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    IplImage *cvimage = cvQueryFrame(m_camera);
    if (!cvimage)
        return;

    // now convert IplImage into a QImage
    int cvIndex, cvLineStart;
    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3: {
            // resize QImage
            if ((cvimage->width != m_image.width()) || (cvimage->height != m_image.height())) {
                QImage temp(cvimage->width, cvimage->height, QImage::Format_RGB32);
                m_image = temp;
            }
            // draw inside the QImage
            cvIndex = 0; cvLineStart = 0;
            for (int y = 0; y < cvimage->height; ++y) {
                unsigned char red,green,blue;
                cvIndex = cvLineStart;
                for (int x = 0; x < cvimage->width; ++x) {
                    // DO it
                    red = cvimage->imageData[cvIndex+2];
                    green = cvimage->imageData[cvIndex+1];
                    blue = cvimage->imageData[cvIndex+0];

                    m_image.setPixel(x,y,qRgb(red, green, blue));
                    cvIndex += 3;
                }
                cvLineStart += cvimage->widthStep;
            }

            // Find the face
            if (true) { //m_counter < 4) {
                ++m_frames; // count frames between last autoshot
                // one shot every 1000ms
                if (m_frames >= (1000/m_updateFreq)) {
                    m_frames = 0;  // reset frame counter
                    QRect rect = getFaceRect(cvimage);
                    if (rect.isEmpty() || !rect.isValid() || rect.width() < 80) {
                        return;
                    }
                    // If found -> get the QImage and cut the face
                    QImage face = m_image.copy(rect);
                    QPixmap pix(face.size());
                    pix = QPixmap::fromImage(face);
                    ++m_counter;
                    Q_EMIT autoFaceShot(pix);
                }
            }
            break;
        }
        default:
            printf("This number of channels is not supported\n");
            break;
        }
        break;
    default:
        printf("This type of IplImage is not implemented in OpenCVWidget\n");
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
    // restrict to left mouse button
    if (event->button() != Qt::LeftButton) {
        QLabel::mousePressEvent(event);
        return;
    }

    // save click pos for later, also needed by mousereleaseEvent() to
    // determine wether this is a click or drag start
    m_clickOrigin = event->pos();

    // only when in frozen state
    if (!m_frozen) {
        QLabel::mousePressEvent(event);
        return;
    }

    //TODO:  mousepress on a border of the rubberband -> resize rubberband

    // if there is no QRubberBand, create one
    if (!m_rubberBand) {
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    }

    // remember the position where the user clicked and the QRubberBand was then
    m_rubberOrigin = m_rubberBand->pos();

    // determine if the user has clicked *into* the QRubberBand boundaries
    if (m_rubberBand->rect().translated(m_rubberBand->pos()).contains(m_clickOrigin, true)) {
        m_Mode = Move;
    } else {
        m_Mode = Create;
        delete m_rubberBand;
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        // create an empty frame and show it
        m_rubberBand->setGeometry(QRect(m_clickOrigin, QSize()));
        m_rubberBand->show();
    }
    QLabel::mousePressEvent(event);
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

        m_rubberBand->setGeometry(rect.normalized());
        // get rid of negative coordinates
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
    // wrong button?
    if (event->button() != Qt::LeftButton) {
        QLabel::mouseReleaseEvent(event);
        return;
    }
    // left button, mouse was not moved since press - fire a click event and return
    if (m_clickOrigin == event->pos()) {
        Q_EMIT clicked();
        QLabel::mouseReleaseEvent(event);
        return;
    }

    // here we can be sure the mouse has been moved

    // drag while capturing? no rubberband? exit!
    if (!m_frozen | !m_rubberBand) {
        QLabel::mouseReleaseEvent(event);
        return;
    }

    QRect rect(m_rubberBand->geometry().normalized());
    qDebug() << m_rubberBand->geometry() << rect;
    if (rect.isValid()) {
        if (rect.height() < 64 || rect.width() < 64) {
            rect.setHeight(64);
            rect.setWidth(64);
            m_rubberBand->setGeometry(rect);
        }

        // we have a valid rubberband square
        Q_EMIT imageReady(true);
    } else {
        m_rubberBand->hide();
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
        // selection must not be smaller than 64x64
        if (m_rubberBand->width() >= 64 + 4) {
            m_rubberBand->move(m_rubberBand->geometry().x()+2, m_rubberBand->y()+2);
            m_rubberBand->resize(m_rubberBand->geometry().width()-4, m_rubberBand->height()-4);
        }
        restrictRubberBandConstraints();
    }
}

void OpenCVWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_frozen) {
        QLabel::mouseDoubleClickEvent(event);
        return;
    }
    setFrozen(false);
}

/** Find the rect around the face. Increase is surface of 20%. */
QRect OpenCVWidget::getFaceRect(IplImage *cvimage)
{
    if (!cvimage)
        return QRect();

    // thanks for some code snippets from Stef van den Elzen
    // taken from http://sj.vdelzen.net/2011/05/31/realtime-face-detection-using-opencv-and-cqt/

    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3: {
            // Detect face objects
            cvClearMemStorage(_storage);
            CvSeq* objects = cvHaarDetectObjects(cvimage, _cascade, _storage, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize( 64, 64 ));
            int n = (objects ? objects->total : 0);
            if (n <= 0)
                return QRect();
            CvRect* rect;
            // we only work with one face
            rect = (CvRect*)cvGetSeqElem(objects, 0);
            QRect qRect(rect->x, rect->y, rect->width, rect->height);
            QPoint center = qRect.center();
            qRect.setWidth(qRect.width() * 1.2);
            qRect.setHeight(qRect.width() * 1.2);
            qRect.moveCenter(center);
            return qRect;
        }
        default:
            printf("This number of channels is not supported\n");
            break;
        }
        break;
    default:
        printf("This type of IplImage is not implemented in QOpenCVWidget\n");
        break;
    }
    return QRect();
}

void OpenCVWidget::drawFaceDetectionFrame(IplImage *cvimage)
{
    if (!cvimage)
        return;

    // thanks for some code snippets from Stef van den Elzen
    // taken from http://sj.vdelzen.net/2011/05/31/realtime-face-detection-using-opencv-and-cqt/

    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3: {

            // Detect face objects
            cvClearMemStorage( _storage );

            //            qDebug() << _cascade;
            CvSeq* objects = cvHaarDetectObjects(cvimage, _cascade, _storage, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize( 64, 64 ));

            int n = (objects ? objects->total : 0);

            //if more than one objects, ignore this sequence
            if (n > 1) {
                n = 0;
            }
            CvRect* rect;
            // Loop through objects and draw boxes
            for( int i = 0; i < n; i++ )
            {
                rect = (CvRect*)cvGetSeqElem(objects, i);
                cvRectangle( cvimage,
                             cvPoint(rect->x - 30, rect->y - 50),
                             cvPoint(rect->x + rect->width + 30, rect->y + rect->height + 50),
                             _colors[i%8]
                             );
            }
            break;
        }
        default:
            printf("This number of channels is not supported\n");
            break;
        }
        break;
    default:
        printf("This type of IplImage is not implemented in QOpenCVWidget\n");
        break;
    }
}

/*!
 * \brief Crops the RubberBand if not inside the parent widget.
 */
void OpenCVWidget::restrictRubberBandConstraints()
{
    // restrict new RubberBand to constraints of parent

    QRect rect = m_rubberBand->geometry().normalized();

    // is too big
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
    m_frozen = !m_frozen;
    setFrozen(m_frozen);
}

void OpenCVWidget::setImageUpdateFrequency(const int ms)
{
    Q_ASSERT(ms > 0); //TODO: don't assert here, do error handling while runnning
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

/*!
 * \brief Sets the frozen state to the given value and (un)freezes the capture stream.
 *
 * The widget then emits the frozen signal.
 * \param aFrozen state that should be set.
 */
void OpenCVWidget::setFrozen(bool aFreeze)
{
    if (aFreeze) {
        // Freeze
        if (m_timerId > 0)
            killTimer(m_timerId);
    } else {
        // remove QRubberBand
        if (m_rubberBand)
            m_rubberBand->hide();
        // Unfreeze
        m_timerId = startTimer(m_updateFreq);  // 0.1-second timer
    }
    m_frozen = aFreeze;
    Q_EMIT frozen(aFreeze);
}

/*! \brief Convenience slot for freezing the widget */
void OpenCVWidget::freeze()
{
    setFrozen(true);
}

/*! \brief Convenience slot for unfreezing the widget */
void OpenCVWidget::unFreeze()
{
    setFrozen(false);
}

void OpenCVWidget::onActionCaptureTriggered()
{

}
