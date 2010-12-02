TEMPLATE = lib
TARGET = SaveRestore

DEFINES += FREEDIAMS
FREEDIAMS = 1

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/saverestoreplugin/shared_sources.pri)

OTHER_FILES += SaveRestore.pluginspec
