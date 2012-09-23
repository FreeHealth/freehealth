TEMPLATE = lib
TARGET = DataPackPlugin

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

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
