#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include <QWaitCondition>
#include <QMutex>
#include <QQueue>

#include "opencv/cxcore.h"

/*!
 * \class ImageBuffer
 * \brief Buffers images between the capture thread and the processing thread and provides synchronization between the two threads.
 */

class ImageBuffer {
public:
    ImageBuffer(int size);
    void addFrame(const IplImage* image);
    IplImage* getFrame();
    void clear();

private:
    QWaitCondition bufferNotEmpty;
    QWaitCondition bufferNotFull;
    QMutex mutex;
    int bufferSize;

    /*! Queue of IplImage objects to store the data. */
    QQueue<IplImage*> imageQueue;
};

#endif
