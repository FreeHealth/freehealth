TEMPLATE        = lib
TARGET          = UserManager
PACKAGE_VERSION = 0.0.2

DEFINES += USERS_LIBRARY

include(../fmf_plugins.pri)
include( usermanagerplugin_dependencies.pri )

HEADERS = usermanagerplugin.h \
    usermanager_exporter.h \
    iuserlistener.h
SOURCES = usermanagerplugin.cpp

# Compile
HEADERS += global.h \
    constants.h \
    userdata.h \
    usermodel.h \
    database/userbase.h \
    widgets/userviewer.h \
    widgets/userviewer_p.h \
    widgets/useridentifier.h \
    widgets/usermanager.h \
    widgets/usermanager_p.h \
    widgets/userpassworddialog.h \
    widgets/userrightswidget.h \
    widgets/userwizard.h

SOURCES += global.cpp \
    userdata.cpp \
    usermodel.cpp \
    database/userbase.cpp \
    widgets/userviewer.cpp \
    widgets/useridentifier.cpp \
    widgets/usermanager.cpp \
    widgets/userpassworddialog.cpp \
    widgets/userrightswidget.cpp \
    widgets/userwizard.cpp

FORMS += widgets/userviewer.ui \
    widgets/useridentifier.ui \
    widgets/usermanager.ui \
    widgets/headersfootersviewerprivate.ui \
    widgets/userpassworddialog.ui

# translators
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/usermanagerplugin_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/usermanagerplugin_de.ts

OTHER_FILES = UserManager.pluginspec
