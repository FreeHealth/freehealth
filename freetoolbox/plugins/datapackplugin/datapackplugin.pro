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
    datapackwidget.cpp \
    genericdatapackcreator.cpp
#    servertoolwidget.cpp


HEADERS  += \
    datapackplugin.h \
    datapackcore.h \
    datapackquery.h \
    datapackresult.h \
    datapackserverquery.h \
    datapackpage.h \
    datapackwidget.h \
    genericdatapackcreator.h
#    servertoolwidget.h

FORMS += \
    datapackwidget.ui \
    serverwidget.ui

OTHER_FILES += DataPackPlugin.pluginspec

# include translations
TRANSLATION_NAME = ftbdatapack
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

