TEMPLATE = lib
TARGET = Printer

DEFINES += FREEDIAMS
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/printerplugin/printer_sources.pri)

OTHER_FILES += Printer.pluginspec
