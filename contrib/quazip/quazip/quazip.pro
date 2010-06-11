TEMPLATE  = lib
TARGET    = quazip

DEFINES += QUAZIP_LIBRARY

include( ../../../libs/libsworkbench.pri )

LIBS *= -L../ -lz

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
           exporter.h

SOURCES += ioapi.c \
           quazip.cpp \
           quazipfile.cpp \
           quazipnewinfo.cpp \
           unzip.c \
           zip.c
