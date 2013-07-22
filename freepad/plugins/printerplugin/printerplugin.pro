DEFINES += FREEPAD
FREEPAD = 1

BUILD_PATH_POSTFIXE = FreePad
DEFINES += WITH_PAD

include(../../../plugins/printerplugin/shared_sources.pri)

OTHER_FILES += Printer.pluginspec
