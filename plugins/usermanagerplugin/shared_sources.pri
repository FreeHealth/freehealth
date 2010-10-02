# Shared sources of UserManager plugin
# Sources are shared between multiple applications (FreeAccount, FreeMedForms)

include( $${PWD}/../fmf_plugins.pri)
include( $${PWD}/usermanagerplugin_dependencies.pri )

HEADERS += $${PWD}/usermanagerplugin.h \
    $${PWD}/usermanager_exporter.h \
    $${PWD}/iuserlistener.h

SOURCES += $${PWD}/usermanagerplugin.cpp

# Compile
HEADERS += $${PWD}/global.h \
    $${PWD}/constants.h \
    $${PWD}/userdata.h \
    $${PWD}/usermodel.h \
    $${PWD}/database/userbase.h \
    $${PWD}/widgets/userviewer.h \
    $${PWD}/widgets/userviewer_p.h \
    $${PWD}/widgets/useridentifier.h \
    $${PWD}/widgets/usermanager.h \
    $${PWD}/widgets/usermanager_p.h \
    $${PWD}/widgets/userpassworddialog.h \
    $${PWD}/widgets/userrightswidget.h \
    $${PWD}/widgets/userwizard.h

SOURCES += $${PWD}/global.cpp \
    $${PWD}/userdata.cpp \
    $${PWD}/usermodel.cpp \
    $${PWD}/database/userbase.cpp \
    $${PWD}/widgets/userviewer.cpp \
    $${PWD}/widgets/useridentifier.cpp \
    $${PWD}/widgets/usermanager.cpp \
    $${PWD}/widgets/userpassworddialog.cpp \
    $${PWD}/widgets/userrightswidget.cpp \
    $${PWD}/widgets/userwizard.cpp

FORMS += $${PWD}/widgets/userviewer.ui \
    $${PWD}/widgets/useridentifier.ui \
    $${PWD}/widgets/usermanager.ui \
    $${PWD}/widgets/headersfootersviewerprivate.ui \
    $${PWD}/widgets/userpassworddialog.ui

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/usermanagerplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/usermanagerplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/usermanagerplugin_es.ts
