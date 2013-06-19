TEMPLATE        = lib
TARGET          = eDRC

INCLUDEPATH += ../
DEPENDPATH += ../

DEFINES += EDRC_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include(edrc_dependencies.pri)

HEADERS += \
    constants.h \
    edrc_exporter.h \
    edrcplugin.h \
    edrccore.h \
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
    consultresult.h

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
    consultresult.cpp

FORMS += \
    widgets/rceditorwidget.ui \
    widgets/rcargumentsdialog.ui \
    widgets/sfmgaboutdialog.ui


OTHER_FILES += eDRC.pluginspec
