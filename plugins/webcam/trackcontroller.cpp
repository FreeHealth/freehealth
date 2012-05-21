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
#include "trackcontroller.h"
#include "processingthread.h"
#include "imagebuffer.h"

#include <QDebug>

TrackController::TrackController() : frameRate(15), frameSize(CaptureThread::Size320) {
    imageBuffer = new ImageBuffer(20);
    captureThread = new CaptureThread(imageBuffer);
    processingThread = new ProcessingThread(imageBuffer);
    processingThread->start();
}

void TrackController::startTracking() {
    captureThread->startCapture(frameRate, frameSize);
    qDebug() << "About to start the capture thread";
    captureThread->start(QThread::IdlePriority);
    qDebug() << "Started the capture thread";
}

bool TrackController::isTracking() {
    return captureThread->isCapturing();
}

void TrackController::stopTracking() {
    captureThread->stopCapture();
}

void TrackController::setRootFilter(Filter* filter) {
    processingThread->setRootFilter(filter);
}

double TrackController::getFPS() {
    return captureThread->getFPS();
}
