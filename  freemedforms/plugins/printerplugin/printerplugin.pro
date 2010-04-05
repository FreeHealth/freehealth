TEMPLATE = lib
TARGET = Printer
PACKAGE_VERSION = 0.0.2
DEFINES += PRINTER_LIBRARY
include(../fmf_plugins.pri)
include( printerplugin_dependencies.pri )
HEADERS = printerplugin.h \
    printer_exporter.h \
    printer.h \
    printerpreviewer_p.h \
    textdocumentextra.h \ # textdocumentheader.h
    printerpreferences.h
SOURCES = printerplugin.cpp \
    printer.cpp \
    printerpreviewer_p.cpp \
    textdocumentextra.cpp \
    printerpreferences.cpp
FORMS = printerpreviewer_p.ui \
    printerpreferences.ui

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/printerplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/printerplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/printerplugin_es.ts
OTHER_FILES = Printer.pluginspec
