TEMPLATE  = lib
TARGET    = quazip

DEFINES += QUAZIP_LIBRARY

include(../../../libs/libsworkbench.pri)
include($${SOURCES_LIBS_PATH}/utils.pri)

win32:LIBS *= -L$${SOURCES_CONTRIBS_PATH}/quazip
#else:linux*:LIBS *= -L/lib

LIBS *= -lz

DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += crypt.h \
           ioapi.h \
           quazip.h \
           quazipfile.h \
           quazipfileinfo.h \
           quazipnewinfo.h \
           unzip.h \
           zip.h \
           exporter.h \
           global.h

SOURCES += ioapi.c \
           quazip.cpp \
           quazipfile.cpp \
           quazipnewinfo.cpp \
           unzip.c \
           zip.c \
           global.cpp
