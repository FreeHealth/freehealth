DEFINES += CATEGORY_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/categoryplugin_dependencies.pri)


HEADERS += \
    $${PWD}/category_exporter.h \
    $${PWD}/categoryplugin.h \
    $${PWD}/categorybase.h \
    $${PWD}/categoryitem.h \
    $${PWD}/categorycore.h \
    $${PWD}/icategorymodelhelper.h \
    $${PWD}/constants.h \
    $${PWD}/categoryonlyproxymodel.h \
    $${PWD}/categorylabelsmodel.h \
    $${PWD}/categorydialog.h \

SOURCES += \
    $${PWD}/categoryplugin.cpp \
    $${PWD}/categorybase.cpp \
    $${PWD}/categoryitem.cpp \
    $${PWD}/categorycore.cpp \
    $${PWD}/icategorymodelhelper.cpp \
    $${PWD}/categoryonlyproxymodel.cpp \
    $${PWD}/categorylabelsmodel.cpp \
    $${PWD}/categorydialog.cpp \

FORMS += \
    $${PWD}/categorydialog.ui \

# include translations
TRANSLATION_NAME = category
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
