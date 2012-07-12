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
#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <opencv/highgui.h>

namespace Webcam {

class ImageBuffer;

class CaptureThread : public QThread {
public: 
    enum FrameSize { Size640, Size320 };

    CaptureThread(ImageBuffer* buffer);
    ~CaptureThread();
    void run();

    bool startCapture(int framerate, FrameSize size);
    void stopCapture();
    double getFPS() { return fps; }
    bool isCapturing() { return captureActive; }

private:
    void updateFPS(int time);

private:
    QMutex _captureLock;
    QWaitCondition _captureWait;
    ImageBuffer* _imageBuffer;
    bool _captureActive;
    CvCapture* _capture;
    double _fps;
    QQueue<int> _frameTimes;
};

}
#endif
