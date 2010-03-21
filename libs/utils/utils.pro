# toolkit library project file
TEMPLATE = lib
TARGET = Utils
PACKAGE_VERSION = 0.4.0
DEFINES *= UTILS_LIBRARY

# include config file
include(../libsworkbench.pri)
include(../utils_dependencies.pri)

CONFIG *= dll \
    sql \
    network

QT *= sql \
    network

HEADERS += global_exporter.h \
    global.h \
    database.h \
    log.h \
    updatechecker_p.h \
    updatechecker.h \
    messagesender.h \
    serializer.h \
    licenseterms.h \
    widgets/scrollingwidget.h \
    widgets/lineeditechoswitcher.h \
    widgets/QButtonLineEdit.h \
    widgets/QPixLineEdit.h \
    widgets/colorbuttonchooser.h \
    widgets/spinboxdelegate.h
SOURCES += global.cpp \
    database.cpp \
    log.cpp \
    updatechecker.cpp \
    messagesender.cpp \
    licenseterms.cpp \
    serializer.cpp \
    widgets/scrollingwidget.cpp \
    widgets/lineeditechoswitcher.cpp \
    widgets/QButtonLineEdit.cpp \
    widgets/colorbuttonchooser.cpp \
    widgets/spinboxdelegate.cpp

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/utils_fr.ts \
    $${SOURCES_TRANSLATIONS}/utils_de.ts \
    $${SOURCES_TRANSLATIONS}/utils_es.ts
