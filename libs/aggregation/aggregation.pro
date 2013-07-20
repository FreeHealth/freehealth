TEMPLATE = lib
TARGET = Aggregation

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

include( ../libsworkbench.pri )
#include(../../qtcreatorlibrary.pri)

DEFINES += AGGREGATION_LIBRARY

HEADERS = aggregate.h \
    aggregation_global.h

SOURCES = aggregate.cpp

