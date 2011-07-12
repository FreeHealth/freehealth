# Shared sources of UserManager plugin
# Sources are shared between multiple applications (FreeAccount, FreeMedForms)

include( $${PWD}/../fmf_plugins.pri)
include( $${PWD}/usermanagerplugin_dependencies.pri )

DEFINES += USERS_LIBRARY

HEADERS += \

SOURCES += \

HEADERS += $${PWD}/usermanagerplugin.h \
    $${PWD}/usermanager_exporter.h \
    $${PWD}/iuserlistener.h \
    $${PWD}/currentuserpreferencespage.h \
    $${PWD}/userfistrunpage.h \
    $${PWD}/usermanagermode.h \
    $${PWD}/widgets/iuserviewerpage.h \
    $${PWD}/defautuserviewerpages.h

SOURCES += $${PWD}/usermanagerplugin.cpp \
    $${PWD}/currentuserpreferencespage.cpp \
    $${PWD}/userfistrunpage.cpp \
    $${PWD}/usermanagermode.cpp \
    $${PWD}/widgets/iuserviewerpage.cpp \
    $${PWD}/defautuserviewerpages.cpp

# Compile
HEADERS += \
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

SOURCES += \
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
#    $${PWD}/widgets/headersfootersviewerprivate.ui \
    $${PWD}/widgets/userpassworddialog.ui \
    $${PWD}/widgets/currentuserpreferenceswidget.ui \
    $${PWD}/widgets/firstrunusercreationwidget.ui \
    $${PWD}/widgets/usermanagerwidget.ui \
    $${PWD}/widgets/userwizardcontactwidget.ui \
    $${PWD}/widgets/userviewer_identity.ui \
    $${PWD}/widgets/userviewer_papers.ui \
    $${PWD}/widgets/userviewer_medicalrights.ui \
    $${PWD}/widgets/userviewer_professional.ui

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/usermanagerplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/usermanagerplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/usermanagerplugin_es.ts
