TEMPLATE        = lib
TARGET          = eDRC

INCLUDEPATH += ../
DEPENDPATH += ../

DEFINES += EDRC_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include(edrcplugin_dependencies.pri)

HEADERS += \
    edrc_exporter.h \
    edrcplugin.h \
    edrccore.h \
    database/constants_db.h \
    database/edrcbase.h \
    models/crtreemodel.h \
    models/classandcrtreemodel.h \
    models/preventablecriticalriskmodel.h \
    models/critemmodel.h \
    models/crclassmodel.h \
    models/crcriteriasmodel.h \
    widgets/crargumentsdialog.h \
    widgets/sfmgaboutdialog.h \
    widgets/creditorwidget.h \
    widgets/edrcpreferences.h \
    widgets/crlistviewer.h \
    consultresult.h \
    consultresultvalidator.h \
    constants.h \
    widgets/creditordialog.h \
    widgets/edrcwidgetmanager.h \
    widgets/edrcwontextualwidget.h

SOURCES += \
    edrcplugin.cpp \
    edrccore.cpp \
    database/edrcbase.cpp \
    models/crtreemodel.cpp \
    models/classandcrtreemodel.cpp \
    models/preventablecriticalriskmodel.cpp \
    models/critemmodel.cpp \
    models/crclassmodel.cpp \
    models/crcriteriasmodel.cpp \
    widgets/crargumentsdialog.cpp \
    widgets/sfmgaboutdialog.cpp \
    widgets/creditorwidget.cpp \
    widgets/edrcpreferences.cpp \
    widgets/crlistviewer.cpp \
    consultresultvalidator.cpp \
    consultresult.cpp \
    widgets/creditordialog.cpp \
    widgets/edrcwidgetmanager.cpp \
    widgets/edrcwontextualwidget.cpp

FORMS += \
    widgets/creditorwidget.ui \
    widgets/crargumentsdialog.ui \
    widgets/sfmgaboutdialog.ui \
    widgets/edrcpreferences.ui \
    widgets/crlistviewer.ui \
    widgets/creditordialog.ui

# include tests
equals(TEST, 1) {
    SOURCES += \
        tests/tst_edrcvalidator.cpp \
        tests/tst_consultresult.cpp
}

OTHER_FILES += eDRC.pluginspec

# include translations
TRANSLATION_NAME = edrc
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
