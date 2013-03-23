DEFINES *= ACCOUNT2_LIBRARY

include(../fmf_plugins.pri)
include(account2plugin_dependencies.pri)

HEADERS += $${PWD}/database/constants_db.h \
    $${PWD}/account2_exporter.h \
    $${PWD}/account2plugin.h \
    $${PWD}/accountcore.h \
    $${PWD}/accountmode.h \
    $${PWD}/accountitems.h \
    $${PWD}/basicfilter.h \
    $${PWD}/database/accountbase.h \
    $${PWD}/database/accountbasequery.h \
    $${PWD}/database/accountbaseresult.h \
    $${PWD}/formitems/accountwidgets.h \
    $${PWD}/models/feemodel.h \
    $${PWD}/models/paymentmodel.h \
    $${PWD}/models/bankaccountmodel.h \
    $${PWD}/models/medicalproceduremodel.h

    

SOURCES += $${PWD}/account2plugin.cpp \
    $${PWD}/accountcore.cpp \
    $${PWD}/accountmode.cpp \
    $${PWD}/accountitems.cpp \
    $${PWD}/basicfilter.cpp \
    $${PWD}/database/accountbase.cpp \
    $${PWD}/database/accountbasequery.cpp \
    $${PWD}/database/accountbaseresult.cpp \
    $${PWD}/formitems/accountwidgets.cpp \
    $${PWD}/models/feemodel.cpp \
    $${PWD}/models/paymentmodel.cpp \
    $${PWD}/models/bankaccountmodel.cpp \
    $${PWD}/models/medicalproceduremodel.cpp



# include translations
TRANSLATION_NAME = account2
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
