#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include "opencv/highgui.h"

class ImageBuffer;

class CaptureThread : public QThread {
public: 
    enum FrameSize { Size640, Size320 };
    CaptureThread(ImageBuffer* buffer);
    void run();
    bool startCapture(int framerate, FrameSize size);
    void stopCapture();
    double getFPS() { return fps; }
    bool isCapturing() { return captureActive; }
private:
    void updateFPS(int time);
    QMutex captureLock;
    QWaitCondition captureWait;
    ImageBuffer* imageBuffer;
    bool captureActive;
    CvCapture* capture;
    double fps;
    QQueue<int> frameTimes;
};

#endif
