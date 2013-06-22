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
    models/rcmodel.h \
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
    consultresult.h \
    consultresultvalidator.h \
    constants.h

SOURCES += \
    edrcplugin.cpp \
    edrccore.cpp \
    database/edrcbase.cpp \
    models/rcmodel.cpp \
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
    consultresultvalidator.cpp \
    consultresult.cpp

FORMS += \
    widgets/rceditorwidget.ui \
    widgets/rcargumentsdialog.ui \
    widgets/sfmgaboutdialog.ui \
    widgets/edrcpreferences.ui


OTHER_FILES += eDRC.pluginspec
