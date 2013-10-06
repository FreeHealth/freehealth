TEMPLATE        = lib
TARGET          = eDRC

DEFINES += EDRC_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/edrcplugin_dependencies.pri)

HEADERS += \
    $${PWD}/edrc_exporter.h \
    $${PWD}/edrcplugin.h \
    $${PWD}/edrccore.h \
    $${PWD}/database/constants_db.h \
    $${PWD}/database/edrcbase.h \
    $${PWD}/models/crtreemodel.h \
    $${PWD}/models/classandcrtreemodel.h \
    $${PWD}/models/preventablecriticalriskmodel.h \
    $${PWD}/models/critemmodel.h \
    $${PWD}/models/crclassmodel.h \
    $${PWD}/models/crcriteriasmodel.h \
    $${PWD}/widgets/crargumentsdialog.h \
    $${PWD}/widgets/sfmgaboutdialog.h \
    $${PWD}/widgets/creditorwidget.h \
    $${PWD}/widgets/edrcpreferences.h \
    $${PWD}/widgets/crlistviewer.h \
    $${PWD}/consultresult.h \
    $${PWD}/consultresultvalidator.h \
    $${PWD}/constants.h \
    $${PWD}/widgets/creditordialog.h \
    $${PWD}/widgets/edrcwidgetmanager.h \
    $${PWD}/widgets/edrcwontextualwidget.h

SOURCES += \
    $${PWD}/edrcplugin.cpp \
    $${PWD}/edrccore.cpp \
    $${PWD}/database/edrcbase.cpp \
    $${PWD}/models/crtreemodel.cpp \
    $${PWD}/models/classandcrtreemodel.cpp \
    $${PWD}/models/preventablecriticalriskmodel.cpp \
    $${PWD}/models/critemmodel.cpp \
    $${PWD}/models/crclassmodel.cpp \
    $${PWD}/models/crcriteriasmodel.cpp \
    $${PWD}/widgets/crargumentsdialog.cpp \
    $${PWD}/widgets/sfmgaboutdialog.cpp \
    $${PWD}/widgets/creditorwidget.cpp \
    $${PWD}/widgets/edrcpreferences.cpp \
    $${PWD}/widgets/crlistviewer.cpp \
    $${PWD}/consultresultvalidator.cpp \
    $${PWD}/consultresult.cpp \
    $${PWD}/widgets/creditordialog.cpp \
    $${PWD}/widgets/edrcwidgetmanager.cpp \
    $${PWD}/widgets/edrcwontextualwidget.cpp

FORMS += \
    $${PWD}/widgets/creditorwidget.ui \
    $${PWD}/widgets/crargumentsdialog.ui \
    $${PWD}/widgets/sfmgaboutdialog.ui \
    $${PWD}/widgets/edrcpreferences.ui \
    $${PWD}/widgets/crlistviewer.ui \
    $${PWD}/widgets/creditordialog.ui

# include tests
equals(TEST, 1) {
    SOURCES += \
        $${PWD}/tests/tst_edrcvalidator.cpp \
        $${PWD}/tests/tst_consultresult.cpp
}

OTHER_FILES += $${PWD}/eDRC.pluginspec

# include translations
TRANSLATION_NAME = edrc
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
