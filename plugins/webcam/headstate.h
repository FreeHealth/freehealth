#ifndef HEAD_STATE_H
#define HEAD_STATE_H

#include <QPoint>
#include "opencv/cxcore.h"

class HeadState {
public:
    QPoint headCenter;
    IplImage* frame;
};
#endif
