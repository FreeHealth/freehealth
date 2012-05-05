#include "ialert.h"
#include "alertcore.h"

using namespace Alert;

IAlert * IAlert::instance()
{
    IAlert * Ptr = AlertCore::singleton();
    return Ptr;
}

