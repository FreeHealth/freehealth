TEMPLATE = lib
TARGET = Aggregation

# The version number is defined by the scripts/source.sh script, keep the following line unchanged
# VERSION=1.0.0

#include(../../qtcreatorlibrary.pri)
include( ../libsworkbench.pri )

DEFINES += AGGREGATION_LIBRARY

HEADERS = aggregate.h \
    aggregation_global.h

SOURCES = aggregate.cpp
