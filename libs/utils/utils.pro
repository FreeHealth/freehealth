# toolkit library project file
TEMPLATE        = lib
TARGET          = Utils
PACKAGE_VERSION = 0.1.0

DEFINES *= UTILS_LIBRARY

# include config file
include(../libsworkbench.pri)
include(../utils_dependencies.pri)


CONFIG *= dll sql network
QT *= sql network

#include subprojects
#include($${PWD}/richtexteditor/richtexteditor.pri)  ---> plugin
#include($${PWD}/actionmanager/actionmanager.pri)    obsolete
#include($${PWD}/contextmanager/contextmanager.pri)  obsolete
#include($${PWD}/listview/listview.pri)              ---> plugin


#include($${PWD}/printer/printer.pri)                ---> plugin

HEADERS += \
    global_exporter.h \
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


SOURCES += \
    global.cpp \
    database.cpp \
    log.cpp \
    updatechecker.cpp \
    messagesender.cpp \
    licenseterms.cpp \
    serializer.cpp \
    widgets/scrollingwidget.cpp \
    widgets/lineeditechoswitcher.cpp \
    widgets/QButtonLineEdit.cpp \


# translators
TRANSLATIONS += $${SOURCES_GLOBAL_RESOURCES}/translations/utils_fr.ts \
    $${SOURCES_GLOBAL_RESOURCES}/translations/utils_de.ts
