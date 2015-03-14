# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

QT += testlib

SOURCE_PATH = $${PWD}/../../../

include($${SOURCE_PATH}/buildspecs/config.pri)
include($${SOURCE_PATH}/libs/rpath.pri)

# Includes libs to test
include($${SOURCE_PATH}/libs/utils.pri)
include($${SOURCE_PATH}/libs/translationutils.pri)
include($${SOURCE_PATH}/libs/datapackutils.pri)
include($${SOURCE_PATH}/contrib/quazip/quazip.pri)

SOURCES = \
    ../main.cpp

HEADERS += \
    ../autotest.h

# Only librairies (not plugins) should be tested here
# For plugins unit-tests please include them in the plugin code
# See documentation

# Include unit-tests for librairies
include(lib_utils/lib_utils.pri)
#include(lib_translations/lib_translations.pri)
#include(lib_datapacks/lib_datapacks.pri)

