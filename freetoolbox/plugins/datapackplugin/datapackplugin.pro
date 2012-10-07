TEMPLATE = lib
TARGET = DataPackPlugin

include(../../../plugins/fmf_plugins.pri)
include(datapack_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    datapackplugin.cpp


HEADERS  += \
    datapackplugin.h

OTHER_FILES += DataPackPlugin.pluginspec
