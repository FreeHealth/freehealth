# toolkit library project file
TEMPLATE        = lib
TARGET          = Utils
PACKAGE_VERSION = 0.1.0

DEFINES *= UTILS_LIBRARY

# include config file
include(../libsworkbench.pri)
include(../translationutils.pri)

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
    database.h \
    log.h \
    updatechecker_p.h \
    updatechecker.h \
    serializer.h \
    widgets/scrollingwidget.h \
    widgets/lineeditechoswitcher.h \
    widgets/QButtonLineEdit.h \
    widgets/QPixLineEdit.h \


SOURCES += \
    database.cpp \
    log.cpp \
    updatechecker.cpp \
    serializer.cpp \
    widgets/scrollingwidget.cpp \
    widgets/lineeditechoswitcher.cpp \
    widgets/QButtonLineEdit.cpp \


# translators
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/utils_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/utils_de.ts
