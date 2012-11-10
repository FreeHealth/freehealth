TEMPLATE = lib
TARGET = DataPackPlugin

include(../../../plugins/fmf_plugins.pri)
include(datapack_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    datapackplugin.cpp \
    datapackcore.cpp \
    datapackquery.cpp \
    datapackresult.cpp


HEADERS  += \
    datapackplugin.h \
    datapackcore.h \
    datapackquery.h \
    datapackresult.h

OTHER_FILES += DataPackPlugin.pluginspec
