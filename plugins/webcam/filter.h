#ifndef FILTER_H
#define FILTER_H

#include "headstate.h"

/**
 * Base class for filters
 */
class Filter {
public:
    Filter();
    void setListener(Filter* list);
    virtual void processPoint(HeadState* state) = 0;
protected:
    Filter* listener;
    void notifyListener(HeadState* state);
};

#endif
