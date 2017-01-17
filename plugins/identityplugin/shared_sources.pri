include($${PWD}/../fmf_plugins.pri)
include($${PWD}/identity_dependencies.pri)

DEFINES += IDENTITY_LIBRARY

HEADERS += \
    $${PWD}/identityplugin.h \
    $${PWD}/identity_exporter.h \
    $${PWD}/identityconstants.h \
#    $${PWD}/identitypreferences.h \
    $${PWD}/identityeditorwidget.h \
    $${PWD}/themedgenderbutton.h \
    $${PWD}/passwordwidget.h \
    $${PWD}/passworddialog.h
        
SOURCES += \
    $${PWD}/identityplugin.cpp \
#    $${PWD}/identitypreferences.cpp \
    $${PWD}/identityeditorwidget.cpp \
    $${PWD}/themedgenderbutton.cpp \
    $${PWD}/passwordwidget.cpp \
    $${PWD}/passworddialog.cpp

FORMS += \
    $${PWD}/identitypreferences.ui \
    $${PWD}/identityeditorwidget.ui \
    $${PWD}/passwordwidget.ui \
    $${PWD}/passworddialog.ui

# include translations
TRANSLATION_NAME = identity
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

