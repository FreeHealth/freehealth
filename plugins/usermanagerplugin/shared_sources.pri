# Shared sources of UserManager plugin
# Sources are shared between multiple applications

include( $${PWD}/../fmf_plugins.pri)
include( $${PWD}/usermanagerplugin_dependencies.pri )

DEFINES += USERS_LIBRARY

HEADERS += $${PWD}/usermanagerplugin.h \
    $${PWD}/usermanager_exporter.h \
    $${PWD}/iuserlistener.h \
    $${PWD}/currentuserpreferencespage.h \
    $${PWD}/userfirstrunpage.h \
    $${PWD}/usermanagermode.h \
    $${PWD}/widgets/iuserviewerpage.h \
    $${PWD}/widgets/defaultuserviewerpages.h \
    $${PWD}/iuserwizardpage.h \
    $${PWD}/widgets/useridentityandloginpage.h \
    $${PWD}/usercore.h \
    $${PWD}/usermanagermodel.h \
    $${PWD}/coreusermodelwrapper.h

SOURCES += $${PWD}/usermanagerplugin.cpp \
    $${PWD}/currentuserpreferencespage.cpp \
    $${PWD}/userfirstrunpage.cpp \
    $${PWD}/usermanagermode.cpp \
    $${PWD}/widgets/iuserviewerpage.cpp \
    $${PWD}/widgets/defaultuserviewerpages.cpp \
    $${PWD}/widgets/useridentityandloginpage.cpp \
    $${PWD}/usercore.cpp \
    $${PWD}/usermanagermodel.cpp \
    $${PWD}/coreusermodelwrapper.cpp

# Compile
HEADERS += \
    $${PWD}/constants.h \
    $${PWD}/userdata.h \
    $${PWD}/usermodel.h \
    $${PWD}/database/userbase.h \
    $${PWD}/widgets/userviewer.h \
    $${PWD}/widgets/useridentifier.h \
    $${PWD}/widgets/usermanager.h \
    $${PWD}/widgets/usermanager_p.h \
    $${PWD}/widgets/userrightswidget.h \
    $${PWD}/widgets/usercreatorwizard.h \
    $${PWD}/widgets/userlineeditcompletersearch.h

SOURCES += \
    $${PWD}/userdata.cpp \
    $${PWD}/usermodel.cpp \
    $${PWD}/database/userbase.cpp \
    $${PWD}/widgets/userviewer.cpp \
    $${PWD}/widgets/useridentifier.cpp \
    $${PWD}/widgets/usermanager.cpp \
    $${PWD}/widgets/userrightswidget.cpp \
    $${PWD}/widgets/usercreatorwizard.cpp \
    $${PWD}/widgets/userlineeditcompletersearch.cpp

FORMS += $${PWD}/widgets/userviewer.ui \
    $${PWD}/widgets/useridentifier.ui \
    $${PWD}/widgets/usermanager.ui \
    $${PWD}/widgets/currentuserpreferenceswidget.ui \
    $${PWD}/widgets/firstrunusercreationwidget.ui \
    $${PWD}/widgets/usermanagerwidget.ui \
    $${PWD}/widgets/userwizardcontactwidget.ui \
    $${PWD}/widgets/userviewer_contact.ui \
    $${PWD}/widgets/userviewer_papers.ui \
    $${PWD}/widgets/userviewer_medicalrights.ui \
    $${PWD}/widgets/userviewer_professional.ui \
    $${PWD}/widgets/userviewer_treedelegate.ui

equals(TEST, 1){
    SOURCES += tests/tst_userdata.cpp
}

# include translations
TRANSLATION_NAME = usermanager
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
