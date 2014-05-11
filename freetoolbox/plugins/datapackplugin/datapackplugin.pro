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
    servercreationpage.cpp \
    servercreationwidget.cpp \
    genericdatapackcreator.cpp
#    servertoolwidget.cpp


HEADERS  += \
    datapackplugin.h \
    datapackcore.h \
    datapackquery.h \
    datapackresult.h \
    datapackserverquery.h \
    servercreationpage.h \
    servercreationwidget.h \
    genericdatapackcreator.h
#    servertoolwidget.h

FORMS += \
    servercreationwidget.ui \
    serverwidget.ui

OTHER_FILES += DataPackPlugin.pluginspec

# include translations
TRANSLATION_NAME = ftbdatapack
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

