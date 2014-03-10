# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

# On some OS we need to build our own zlib.
# It is build statically and it is mainly used by quazip.

TEMPLATE = lib
TARGET = z

include(../../../libs/libsworkbench.pri)

# redefine the target because the included libsworkbench.pri modifies the target
TARGET = z

CONFIG *= staticlib

DEPENDPATH *= .
INCLUDEPATH *= .

RESOURCES += zlib.qrc

# -D_LARGEFILE64_SOURCE=1

# Input
HEADERS += \
    crc32.h \
    deflate.h \
    gzguts.h \
    inffast.h \
    inffixed.h \
    inflate.h \
    inftrees.h \
    trees.h \
    zconf.h \
    zlib.h \
    zutil.h


SOURCES += \
    adler32.c  \
    compress.c \
    crc32.c \
    deflate.c \
    gzclose.c \
    gzlib.c \
    gzread.c \
    gzwrite.c \
    inflate.c \
    infback.c \
    inftrees.c \
    inffast.c \
    trees.c \
    uncompr.c \
    zutil.c
