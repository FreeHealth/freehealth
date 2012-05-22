TARGET = Webcam
TEMPLATE = lib

BUILD_PATH_POSTFIXE = FreeMedForms
PROVIDER = FreeMedForms

include(../fmf_plugins.pri)
include(webcam_dependencies.pri)
include(shared_sources.pri)

OTHER_FILES = \
    Webcam.pluginspec

HEADERS += \
    webcamdialog.h \
    trackcontroller.h \
    processingthread.h \
    imagebuffer.h \
    headstate.h \
    filter.h \
    capturethread.h \
    renderwidget.h \

SOURCES += \
    webcamdialog.cpp \
    trackcontroller.cpp \
    processingthread.cpp \
    capturethread.cpp \
    filter.cpp \
    capturethread.cpp \
    renderwidget.cpp

FORMS += \
    webcamdialog.ui
