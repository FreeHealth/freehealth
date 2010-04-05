# first definitions
TEMPLATE        = lib
TARGET          = Core
PACKAGE_VERSION = 0.0.9

# include(interfaces/interfaces.pri)
include(common_core.pri)

# include(private/private.pri)
HEADERS += \
    coreplugin.h \
    coreimpl.h \
#    maininterface/mainwindow.h \
    mfObject.h \
    mfObjectFundamental.h \
    iformio.h

SOURCES += \
    coreplugin.cpp \
    coreimpl.cpp \
#    maininterface/mainwindow.cpp \

