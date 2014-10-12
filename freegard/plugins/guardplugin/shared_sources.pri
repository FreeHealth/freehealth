DEFINES *= GUARD_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include(guardplugin_dependencies.pri)

HEADERS += \
    $${PWD}/guard_exporter.h \
    $${PWD}/constants.h \
    $${PWD}/guardplugin.h \
    $${PWD}/guardcore.h \
    $${PWD}/database/constants_db.h \
    $${PWD}/database/guardbase.h \
    $${PWD}/models/guardiantablemodel.h \
#    $${PWD}/database/guardbasequery.h \
#    $${PWD}/database/guardbaseresult.h \
#    $${PWD}/models/paymentmodel.h \
#    $${PWD}/models/bankaccountmodel.h \
#    $${PWD}/models/medicalproceduremodel.h \
    $${PWD}/modes/guardianmode.h \
    $${PWD}/modes/guardlinemode.h \
    $${PWD}/modes/guardtablemode.h \
    $${PWD}/views/guardlinetimeslotview.h \
    $${PWD}/views/guardlineview.h

SOURCES += $${PWD}/guardplugin.cpp \
    $${PWD}/guardcore.cpp \
    $${PWD}/database/guardbase.cpp \
    $${PWD}/models/guardiantablemodel.cpp \
#    $${PWD}/database/guardbasequery.cpp \
#    $${PWD}/database/guardbaseresult.cpp \
#    $${PWD}/models/paymentmodel.cpp \
#    $${PWD}/models/bankaccountmodel.cpp \
#    $${PWD}/models/medicalproceduremodel.cpp \
    $${PWD}/modes/guardianmode.cpp \
    $${PWD}/modes/guardlinemode.cpp \
    $${PWD}/modes/guardtablemode.cpp \
    $${PWD}/views/guardlinetimeslotview.cpp \
    $${PWD}/views/guardlineview.cpp

FORMS += \
    $${PWD}/views/guardlinetimeslotview.ui \
    $${PWD}/views/guardlineview.ui

# include translations
TRANSLATION_NAME = guard
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
