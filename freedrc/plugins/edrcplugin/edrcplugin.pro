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
    models/preventablecriticalriskmodel.h \
    models/rcitemmodel.h \
    models/rcclassmodel.h \
    models/rctreemodel.h \
    models/rcusermodel.h \
    models/rccriteriasmodel.h \
    widgets/rcargumentsdialog.h \
    widgets/sfmgaboutdialog.h \
    widgets/rceditorwidget.h \
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
    models/preventablecriticalriskmodel.cpp \
    models/rcitemmodel.cpp \
    models/rcclassmodel.cpp \
    models/rctreemodel.cpp \
    models/rccriteriasmodel.cpp \
    models/rcusermodel.cpp \
    widgets/rcargumentsdialog.cpp \
    widgets/sfmgaboutdialog.cpp \
    widgets/rceditorwidget.cpp \
    widgets/edrcpreferences.cpp \
    widgets/crlistviewer.cpp \
    consultresultvalidator.cpp \
    consultresult.cpp \
    widgets/creditordialog.cpp \
    widgets/edrcwidgetmanager.cpp \
    widgets/edrcwontextualwidget.cpp

FORMS += \
    widgets/rceditorwidget.ui \
    widgets/rcargumentsdialog.ui \
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
