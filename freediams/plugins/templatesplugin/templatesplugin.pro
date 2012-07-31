TEMPLATE = lib
TARGET = Templates

DEFINES += FREEDIAMS
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/templatesplugin/shared_sources.pri)

OTHER_FILES += Templates.pluginspec
