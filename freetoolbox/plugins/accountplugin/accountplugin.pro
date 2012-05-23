TEMPLATE = lib
TARGET = Account

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include(account_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    accountplugin.cpp


HEADERS  += \
    accountplugin.h


OTHER_FILES += Account.pluginspec
