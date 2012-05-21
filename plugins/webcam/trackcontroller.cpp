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
