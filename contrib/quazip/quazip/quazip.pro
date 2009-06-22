TEMPLATE = lib
TARGET = quazip

isEmpty(BUILDING_PROTECTED):include( ../../../config.pri )

DESTDIR = ../

CONFIG *= staticlib
# CONFIG *= qt warn_on staticlib
# QT -= gui

unix { LIBS += -z  }
else {
    message( Win32 compilation of Quazip )
    PRE_TARGETDEPS += ../zlib-1.2.3
    LIBS += -L../zlib-1.2.3 -lz
}

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
           zip.h

SOURCES += ioapi.c \
           quazip.cpp \
           quazipfile.cpp \
           quazipnewinfo.cpp \
           unzip.c \
           zip.c

