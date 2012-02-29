TEMPLATE = lib
TARGET = ExtensionSystem

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

QT += xml
DEFINES += EXTENSIONSYSTEM_LIBRARY
#include(../../qtcreatorlibrary.pri)
include( ../libsworkbench.pri )
include( ../extensionsystem_dependencies.pri )
#include(extensionsystem_dependencies.pri)

unix:!macx {
  LIBS += -ldl
}

DEFINES += IDE_TEST_DIR=\\\"$$PACKAGE_PWD\\\"

HEADERS += pluginerrorview.h \
    plugindetailsview.h \
    iplugin.h \
    iplugin_p.h \
    extensionsystem_global.h \
    pluginmanager.h \
    pluginmanager_p.h \
    pluginspec.h \
    pluginspec_p.h \
    pluginview.h \
    pluginview_p.h \
    optionsparser.h
SOURCES += pluginerrorview.cpp \
    plugindetailsview.cpp \
    iplugin.cpp \
    pluginmanager.cpp \
    pluginspec.cpp \
    pluginview.cpp \
    optionsparser.cpp
FORMS += pluginview.ui \
    pluginerrorview.ui \
    plugindetailsview.ui
RESOURCES += pluginview.qrc
