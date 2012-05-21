#include "processingthread.h"
#include "headstate.h"
#include "filter.h"
#include "imagebuffer.h"
#include <QDebug>

ProcessingThread::ProcessingThread(ImageBuffer* buffer) : QThread(), imageBuffer(buffer), rootFilter(0), flipVertical(false) {
    currState = new HeadState();
}

void ProcessingThread::run() {
    while(true) {
        IplImage* currentFrame = imageBuffer->getFrame();
        //qDebug() << "Processing thread has received a frame";
        if(currentFrame) {
            if(flipVertical) {
                cvFlip(currentFrame, 0, 0);
            }
            currState->frame = currentFrame;

            if(rootFilter) {
                //	qDebug() << "about to notify the root listener of the frame";
                rootFilter->processPoint(currState);
                //	qDebug() << "done notification";
            }
            cvReleaseImage(&currentFrame);
        }
    }
}


