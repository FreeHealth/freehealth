# toolkit library project file
TEMPLATE        = lib
TARGET          = DataPack

# include config file
include(../libsworkbench.pri)
include(../datapackutils_dependencies.pri)

DEFINES *= DATAPACKUTILS_LIBRARY

HEADERS += datapack_exporter.h \
    servermanager.h \
    servermanager_p.h \
    server.h \
    server_p.h \
    serverdescription.h \
    packdescription.h \
    pack.h \
    serveridentification.h \
    constants.h \
    servercontents.h

SOURCES += \
    servermanager.cpp \
    servermanager_p.cpp \
    server.cpp \
    server_p.cpp \
    serverdescription.cpp \
    packdescription.cpp \
    pack.cpp \
    serveridentification.cpp \
    servercontents.cpp





















