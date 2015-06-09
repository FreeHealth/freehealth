TEMPLATE        = lib
TARGET          = PadTools

DEFINES += PADTOOLS_LIBRARY
DEFINES += FREEPAD
FREEPAD = 1

BUILD_PATH_POSTFIXE = FreePad

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/padtoolsplugin/padtools_sources.pri)

OTHER_FILES += Padtools.pluginspec
