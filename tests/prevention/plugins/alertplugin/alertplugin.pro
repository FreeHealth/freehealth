TEMPLATE        = lib
TARGET          = Alert

DEFINES += PREVENTION

BUILD_PATH_POSTFIXE = Prevention

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../../plugins/alertplugin/shared_sources.pri)

OTHER_FILES += Alert.pluginspec
