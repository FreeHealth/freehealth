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
    interactors/druginteractor.h \
    interactors/druginteractortablemodel.h \
    interactors/druginteractortreeproxymodel.h \
    interactors/interactoreditorwidget.h \
    ddi/drugdruginteraction.h \
    ddi/drugdruginteractiontablemodel.h \
    ddi/drugdruginteractioneditorwidget.h \


SOURCES += \
    ddiplugin.cpp \
    ddicore.cpp \
    database/ddidatabase.cpp \
    atc/atctablemodel.cpp \
    atc/atctreeproxymodel.cpp \
    atc/atccollectioneditorwidget.cpp \
    atc/atceditorwidget.cpp \
    atc/searchatcindatabasedialog.cpp \
    interactors/druginteractor.cpp \
    interactors/druginteractortablemodel.cpp \
    interactors/druginteractortreeproxymodel.cpp \
    interactors/interactoreditorwidget.cpp \
    ddi/drugdruginteraction.cpp \
    ddi/drugdruginteractiontablemodel.cpp \
    ddi/drugdruginteractioneditorwidget.cpp \


FORMS += \
    atc/atccollectioneditorwidget.ui \
    atc/atceditorwidget.ui \
    atc/searchatcindatabasedialog.ui \
    interactors/interactoreditorwidget.ui \
    ddi/drugdruginteractioneditorwidget.ui \


OTHER_FILES += DDIManager.pluginspec

equals(TEST, 1) {
    SOURCES += \
        tests/test_druginteractor.cpp
}

# include translations
TRANSLATION_NAME = ddimanager
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
