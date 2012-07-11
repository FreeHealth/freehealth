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
#include <trackcontroller.h>
#include <processingthread.h>
#include <imagebuffer.h>

#include <QDebug>

using namespace Webcam;

TrackController::TrackController() :
    m_frameRate(15),
    m_frameSize(CaptureThread::Size320)
{
    imageBuffer = new ImageBuffer(20);
    m_captureThread = new CaptureThread(imageBuffer);
    m_processingThread = new ProcessingThread(imageBuffer);
    m_processingThread->start();
}

/*!
 * \brief Start capturing the webcam.
 */
void TrackController::startTracking()
{
    m_captureThread->startCapture(m_frameRate, m_frameSize);
    qDebug() << "About to start the capture thread";
    m_captureThread->start(QThread::IdlePriority);
    qDebug() << "Started the capture thread";
}

/*!
 * \brief Determine wether webcam is capturing at the moment.
 * @return bool True if webcam is tracked now.
 */
bool TrackController::isTracking()
{
    return m_captureThread->isCapturing();
}

/*!
 * \brief Stop tracking of the webcam.
 */
void TrackController::stopTracking()
{
    m_captureThread->stopCapture();
}

void TrackController::setRootFilter(Filter* filter)
{
    m_processingThread->setRootFilter(filter);
}

double TrackController::getFPS()
{
    return m_captureThread->getFPS();
}
