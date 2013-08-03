TEMPLATE = lib
TARGET = Alert

DEFINES += ALERT_LIBRARY

QT *= sql

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/alertplugin_dependencies.pri)

!with-alerts{
    error(Alert plugin not requested)
} else {
    message(Building Alert plugin)
}

HEADERS += \
    $${PWD}/constants.h \
    $${PWD}/alertplugin.h \
    $${PWD}/alertbase.h \
    $${PWD}/alertcore.h \
    $${PWD}/alertitem.h \
    $${PWD}/ialertplaceholder.h \
    $${PWD}/alertitemeditorwidget.h \
    $${PWD}/alertitemtimingeditorwidget.h \
    $${PWD}/alertitemeditordialog.h \
    $${PWD}/blockingalertdialog.h \
    $${PWD}/alertplaceholderwidget.h \
    $${PWD}/nonblockingalertwidgets.h \
    $${PWD}/alertscriptmanager.h \
    $${PWD}/alertitemscripteditor.h \
    $${PWD}/alertpackdescription.h \
    $${PWD}/alertpreferences.h \
    $${PWD}/patientbaralertplaceholder.h

SOURCES += \
    $${PWD}/alertplugin.cpp \
    $${PWD}/alertbase.cpp \
    $${PWD}/alertcore.cpp \
    $${PWD}/alertitem.cpp \
    $${PWD}/ialertplaceholder.cpp \
    $${PWD}/alertitemeditorwidget.cpp \
    $${PWD}/alertitemtimingeditorwidget.cpp \
    $${PWD}/alertitemeditordialog.cpp \
    $${PWD}/blockingalertdialog.cpp \
    $${PWD}/alertplaceholderwidget.cpp \
    $${PWD}/nonblockingalertwidgets.cpp \
    $${PWD}/alertscriptmanager.cpp \
    $${PWD}/alertitemscripteditor.cpp \
    $${PWD}/alertpackdescription.cpp \
    $${PWD}/alertpreferences.cpp \
    $${PWD}/patientbaralertplaceholder.cpp

FORMS += \
    $${PWD}/alertitemeditorwidget.ui \
    $${PWD}/alertitemtimingeditorwidget.ui \
    $${PWD}/alertitemeditordialog.ui \
    $${PWD}/blockingalertdialog.ui \
    $${PWD}/blockingalertdialogoverridingcomment.ui \
    $${PWD}/alertitemscripteditor.ui \
    $${PWD}/alertpreferences.ui

OTHER_FILES += $${PWD}/Alert.pluginspec

# include tests
equals(TEST, 1) {
    SOURCES += tests/tst_alertitem.cpp
}

# include translations
TRANSLATION_NAME = alert
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
