TEMPLATE        = lib
TARGET          = DDIManager

DEFINES += DDIMANAGER_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include($${PWD}/ddiplugin_dependencies.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += \
    constants.h \
    ddiplugin.h \
    ddicore.h \
    database/ddidatabase.h \
    atc/atctablemodel.h \
    atc/atctreeproxymodel.h \
    atc/atccollectioneditorwidget.h \
    atc/atceditorwidget.h \
    atc/searchatcindatabasedialog.h \


SOURCES += \
    ddiplugin.cpp \
    ddicore.cpp \
    database/ddidatabase.cpp \
    atc/atctablemodel.cpp \
    atc/atctreeproxymodel.cpp \
    atc/atccollectioneditorwidget.cpp \
    atc/atceditorwidget.cpp \
    atc/searchatcindatabasedialog.cpp \


FORMS += \
    atc/atccollectioneditorwidget.ui \
    atc/atceditorwidget.ui \
    atc/searchatcindatabasedialog.ui \


OTHER_FILES += DDIManager.pluginspec

# include translations
TRANSLATION_NAME = ddimanager
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
