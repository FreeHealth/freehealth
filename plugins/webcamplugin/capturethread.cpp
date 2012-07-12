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
#include <capturethread.h>
#include <imageBuffer.h>
#include <QDebug>
#include <QTime>

using namespace Webcam;

CaptureThread::CaptureThread(ImageBuffer* buffer) :
    QThread(),
    _imageBuffer(buffer),
    _captureActive(false),
    _fps(0.0)
{
    _capture = cvCaptureFromCAM(-1);
}

CaptureThread::~CaptureThread()
{
    qDebug() << "~CaptureThread: Releasing cvCapture...";
    cvReleaseCapture(&_capture);
}

/*!
 * \brief Overloaded function that starts the thread.
 */
void CaptureThread::run() {	
    QTime time;
    time.start();
//    int numFrames = 0;
    while(true) {
        if(!_captureActive) {
            _captureLock.lock();
            qDebug() << "Waiting on capture start...";
            fps = 0;
            frameTimes.clear();
            _captureWait.wait(&_captureLock);
            time.restart();
            updateFPS(time.elapsed());
            qDebug() << "Starting capture";
            _captureLock.unlock();
        }
        //sleep(1);
        _imageBuffer->addFrame(cvQueryFrame(capture));
        updateFPS(time.elapsed());
//        numFrames++;

//        fps = numFrames/((double)time.elapsed())*1000.0;
//        qDebug() << "FPS:" << fps;
    }
}


void CaptureThread::updateFPS(int time) {
    _frameTimes.enqueue(time);
    if(_frameTimes.size() > 15) {
        _frameTimes.dequeue();
    }
    if(_frameTimes.size() > 1) {
        _fps = _frameTimes.size()/((double)time-_frameTimes.head())*1000.0;
    }
    else {
        _fps = 0;
    }
}

/*!
 * \brief Start the capture process
 */
bool CaptureThread::startCapture(int framerate, FrameSize size) {
    if(!_captureActive) {
        if(!_capture || !_imageBuffer){
            qDebug() << "E: Capture not initialized or invalid buffer";
            return false;
        }
        qDebug() << "Listing capture properties...";
        qDebug() << "CV_CAP_PROP_FRAME_WIDTH" << cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH);
        qDebug() << "CV_CAP_PROP_FRAME_HEIGHT" << cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT);
        qDebug() << "CV_CAP_PROP_FPS" << cvGetCaptureProperty(_capture, CV_CAP_PROP_FPS);
        qDebug() << "CV_CAP_PROP_FOURCC" << cvGetCaptureProperty(_capture, CV_CAP_PROP_FOURCC);
        qDebug() << "CV_CAP_PROP_BRIGHTNESS" << cvGetCaptureProperty(_capture, CV_CAP_PROP_BRIGHTNESS);
        qDebug() << "CV_CAP_PROP_CONTRAST" << cvGetCaptureProperty(_capture, CV_CAP_PROP_CONTRAST);
        qDebug() << "CV_CAP_PROP_SATURATION" << cvGetCaptureProperty(_capture, CV_CAP_PROP_SATURATION);
        qDebug() << "CV_CAP_PROP_HUE" << cvGetCaptureProperty(_capture, CV_CAP_PROP_HUE);
        qDebug() << "Done";
        if(size == Size640) {
            qDebug() << "Setting 640x480" << cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, 640);
            cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, 480);
        }
        else if(size == Size320) {
            qDebug() << "Settings 320x240:" << cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, 320);
            cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
        }
        qDebug() << "Attempting to set frame rate...";
        qDebug() << "Error:" << cvSetCaptureProperty(_capture, CV_CAP_PROP_FPS, framerate);
        //qDebug() << "Sleeping for 2 seconds";
        //sleep(2);
        //qDebug() << "done";
        qDebug() << "Starting to track";
        _captureActive = true;
        _captureWait.wakeAll();
        return true;
    }
    return false;
}

/*!
 * \brief Stop the capture process
 */
void CaptureThread::stopCapture() {
    qDebug() << "Stop capture requested.";
    _captureActive = false;
}
