#include "filter.h"

Filter::Filter() : listener(0) {}

/**
 * Set the filter that will listen to updated head state events.
 * @param filter Listener that should be notified of updated events.
 */
void Filter::setListener(Filter *filter) {
    listener = filter;
}

/**
 * Notify the registered listener of an updated head state.
 * @param state The current state
 */
void Filter::notifyListener(HeadState* state) {
    if(listener) {
        listener->processPoint(state);
    }
}
