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
#include "imagebuffer.h"

#include <opencv2/core/core_c.h>

#include <QTime>
#include <QDebug>

using namespace Webcam;

ImageBuffer::ImageBuffer(int size) :
    bufferSize(size)
{
}


/**
 * \brief Adds a frame to the buffer.
 * The data in the frame is copied to the internal image buffer.  
 * If there are no images in the buffer, the clients of the wait condition are woken to notify them of a new frame.
 * If the buffer is full, the function blocks until it there is space
 * @param image The image to add
 */
void ImageBuffer::addFrame(const IplImage* image)
{
	//QTime time;
	//time.start();
	if(!image) {
		qDebug() << "E: Imagebuffer received a null image";
		return;
	}
	//qDebug() << "Adding a frame";
	mutex.lock();
	if(imageQueue.size() == bufferSize) {
		bufferNotFull.wait(&mutex);
	}
	mutex.unlock();
	
	// copy the image
	IplImage* temp = cvCloneImage(image);
	imageQueue.enqueue(temp);
	//qDebug() << "AF" << time.elapsed();
	mutex.lock();
	bufferNotEmpty.wakeAll();
	mutex.unlock();
}

/*!
 * \brief Return the oldest frame from the buffer.
 * This is a blocking operation, so if there are no available images, this function will block until one is available or image acquisition is stopped.
 * @returns IplImage pointer to the next available frame. Ownership of the data is passed to the callee, so the image must be released there.
 */
IplImage* ImageBuffer::getFrame()
{
	mutex.lock();
	if(imageQueue.isEmpty()) {
	//	qDebug() << "Get frame waiting on frame";
		bufferNotEmpty.wait(&mutex);
	//	qDebug() << "Get frame has been received";
	}
	mutex.unlock();

	IplImage* temp = 0;

	mutex.lock();
	if(!imageQueue.isEmpty()) {
		temp = imageQueue.dequeue();
	}
	bufferNotFull.wakeAll();
	mutex.unlock();
	return temp;
}

/*! \brief Clear the buffer and release clients */
void ImageBuffer::clear()
{
	imageQueue.clear();
	bufferNotEmpty.wakeAll();
}
