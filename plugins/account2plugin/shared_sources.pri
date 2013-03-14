DEFINES *= ACCOUNT2_LIBRARY

include(../fmf_plugins.pri)
include(account2plugin_dependencies.pri)

HEADERS += $${PWD}/database/constants_db.h \
    $${PWD}/account2_exporter.h \
    $${PWD}/account2plugin.h \
    $${PWD}/accountcore.h \
    $${PWD}/accountitems.h \
    $${PWD}/database/accountbase.h \
    $${PWD}/database/accountbasequery.h \
    $${PWD}/database/accountbaseresult.h
    

SOURCES += $${PWD}/account2plugin.cpp \
    $${PWD}/accountcore.cpp \
    $${PWD}/accountitems.cpp \
    $${PWD}/database/accountbase.cpp \
    $${PWD}/database/accountbasequery.cpp \
    $${PWD}/database/accountbaseresult.cpp

# include translations
TRANSLATION_NAME = account2
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
