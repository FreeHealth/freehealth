# toolkit library project file
TEMPLATE        = lib
TARGET          = DataPack

# include config file
include(../libsworkbench.pri)
include(../datapackutils_dependencies.pri)

DEFINES *= DATAPACKUTILS_LIBRARY

HEADERS += datapack_exporter.h \
    servermanager.h \
    server.h \
    serverdescription.h \
    packdescription.h \
    pack.h \
    serveridentification.h \
    constants.h \
    servercontent.h \
    core.h \
    iservermanager.h \
    widgets/servereditor.h

SOURCES += \
    servermanager.cpp \
    server.cpp \
    serverdescription.cpp \
    packdescription.cpp \
    pack.cpp \
    serveridentification.cpp \
    servercontent.cpp \
    core.cpp \
    iservermanager.cpp \
    widgets/servereditor.cpp

FORMS += \
    widgets/servereditor.ui




























