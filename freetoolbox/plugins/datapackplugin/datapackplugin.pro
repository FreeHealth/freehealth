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
    datapackresult.cpp \
    datapackserverquery.cpp \
    datapackpage.cpp \
    datapackwidget.cpp


HEADERS  += \
    datapackplugin.h \
    datapackcore.h \
    datapackquery.h \
    datapackresult.h \
    datapackserverquery.h \
    datapackpage.h \
    datapackwidget.h

OTHER_FILES += DataPackPlugin.pluginspec

FORMS += \
    datapackwidget.ui \
    serverwidget.ui
