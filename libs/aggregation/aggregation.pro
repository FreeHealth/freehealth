TEMPLATE = lib
TARGET = Aggregation

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED. Command: echo 0.7.0~beta1 | tr '~' ':' | cut -d : -f 1
# VERSION=1.0.0

#include(../../qtcreatorlibrary.pri)
include( ../libsworkbench.pri )

DEFINES += AGGREGATION_LIBRARY

HEADERS = aggregate.h \
    aggregation_global.h

SOURCES = aggregate.cpp
