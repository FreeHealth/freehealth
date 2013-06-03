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
    edrcbase.h \
    rcmodel.h \
    rcemodel.h \
    rcitemmodel.h \
    rcclassmodel.h \
    edrc_editor_widget.h \
    rctreemodel.h \
    rccriteriasmodel.h \
    rcargumentsdialog.h \
    sfmgaboutdialog.h \
    rcusermodel.h

SOURCES += \
    edrcplugin.cpp \
    edrccore.cpp \
    edrcbase.cpp \
    rcmodel.cpp \
    rcemodel.cpp \
    rcitemmodel.cpp \
    rcclassmodel.cpp \
    edrc_editor_widget.cpp \
    rctreemodel.cpp \
    rccriteriasmodel.cpp \
    rcargumentsdialog.cpp \
    sfmgaboutdialog.cpp \
    rcusermodel.cpp

FORMS += \
    edrc_editor_widget.ui \
    rcargumentsdialog.ui \
    sfmgaboutdialog.ui


OTHER_FILES += eDRC.pluginspec
