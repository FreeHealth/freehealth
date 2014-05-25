TEMPLATE = lib
TARGET = DataPackPlugin

include(../../../plugins/fmf_plugins.pri)
include(datapack_dependencies.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    datapackplugin.cpp \
    datapackcore.cpp \
    imode/datapackmode.cpp \
    imode/datapackmodewidget.cpp

HEADERS  += \
    datapackplugin.h \
    datapackcore.h \
    imode/datapackmode.h \
    imode/datapackmodewidget.h

FORMS += \
    imode/datapackmodewidget.ui

OTHER_FILES += DataPackPlugin.pluginspec

# include translations
TRANSLATION_NAME = fddi_datapack
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

