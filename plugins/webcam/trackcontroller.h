#ifndef TRACK_CONTROLLER_H
#define TRACK_CONTROLLER_H

#include "capturethread.h"

class Filter;
class ProcessingThread;
class ImageBuffer;

class TrackController : public QObject {
    Q_OBJECT;
public:
    TrackController();
    bool isTracking();
    void setFrameSize(CaptureThread::FrameSize size) { frameSize = size; }
    CaptureThread::FrameSize getFrameSize() { return frameSize; }
    void setFrameRate(int rate) { frameRate = rate; }
    int getFrameRate() { return frameSize; }
    void setRootFilter(Filter* filter);
    ProcessingThread* getProcessingThread() { return processingThread; }
    double getFPS();
public slots:
    void startTracking();
    void stopTracking();
private:
    int frameRate;
    CaptureThread::FrameSize frameSize;
    CaptureThread* captureThread;
    ProcessingThread* processingThread;
    ImageBuffer* imageBuffer;
};

#endif
