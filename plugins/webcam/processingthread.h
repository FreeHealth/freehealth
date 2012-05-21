#ifndef PROCESSING_THREAD_H
#define PROCESSING_THREAD_H

#include <QThread>

class ImageBuffer;
class Filter;
class HeadState;

class ProcessingThread : public QThread {
public: 
    ProcessingThread(ImageBuffer* buffer);
    void setRootFilter(Filter* filter) { rootFilter = filter; }
    void setFlipVertical(bool fv) { flipVertical = fv; }
    void run();
private:
    ImageBuffer* imageBuffer;
    Filter* rootFilter;
    HeadState* currState;
    bool flipVertical;
};

#endif
