DEFINES *= GUARD_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include(guardplugin_dependencies.pri)

HEADERS += \
    $${PWD}/guard_exporter.h \
    $${PWD}/guardplugin.h \
#    $${PWD}/guardcore.h \
    $${PWD}/database/constants_db.h \
    $${PWD}/database/guardbase.h \
#    $${PWD}/database/guardbasequery.h \
#    $${PWD}/database/guardbaseresult.h \
#    $${PWD}/models/feemodel.h \
#    $${PWD}/models/paymentmodel.h \
#    $${PWD}/models/bankaccountmodel.h \
#    $${PWD}/models/medicalproceduremodel.h \
#    $${PWD}/modes/guardmode.h \


SOURCES += $${PWD}/guardplugin.cpp \
#    $${PWD}/guardcore.cpp \
    $${PWD}/database/guardbase.cpp \
#    $${PWD}/database/guardbasequery.cpp \
#    $${PWD}/database/guardbaseresult.cpp \
#    $${PWD}/models/feemodel.cpp \
#    $${PWD}/models/paymentmodel.cpp \
#    $${PWD}/models/bankaccountmodel.cpp \
#    $${PWD}/models/medicalproceduremodel.cpp \
#    $${PWD}/modes/guardmode.cpp \

# include translations
TRANSLATION_NAME = guard
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
