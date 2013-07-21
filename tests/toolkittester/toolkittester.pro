TEMPLATE = app
TARGET = toolkittester
include( ../../config.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri)
DEFINES *= FMF_CORE_BUILD

# DESTDIR = $${PWD}/../bin
DEPENDPATH = $${PACKAGE_LIBS}/toolkit

# LIBS = -L$${PACKAGE_BUILD_PATH}
# mac:*-g++:LIBS *= -Wl,-all_load # import all symbols as the not used ones too
# else:*-g++:LIBS *= -Wl,--whole-archive # import all symbols as the not used ones too
# mac:*-g++:LIBS *= -dynamic
# else:unix:*-g++:LIBS *= -rdynamic
PRE_TARGETDEPS *= ../../libs/toolkit
win32:LIBS *= -ltoolkit_d
else:LIBS *= -ltoolkit_debug
mac:*-g++:LIBS *= -Wl,-noall_load # stop importing all symbols
else:*-g++:LIBS *= -Wl,--no-whole-archive # stop importing all symbols

# define some path in order to access GLOBALRESOURCES
FMF_GLOBAL_RESOURCES = $${PWD}/../../../global_resources
mac:FMF_BIN_PATH = $${PWD}
else:unix|win32:FMF_BIN_PATH = $${PWD}
DEFINES += PACKAGE_VERSION \
    "FMF_BIN_PATH=\"\\\"$${FMF_BIN_PATH}\\\"\"" \
    "FMF_GLOBAL_RESOURCES=\"\\\"$$:FMF_GLOBAL_RESOURCES\\\"\""

# define Qt
QT *= sql \
    network \
    xml

# Compile
SOURCES += main.cpp \
    tkPrinterTester.cpp
RESOURCES += resources.qrc
HEADERS += tkPrinterTester.h
