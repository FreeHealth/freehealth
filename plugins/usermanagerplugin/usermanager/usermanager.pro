TEMPLATE = app
TARGET = FreeMedFormsUserManager

# Prepare building process
include( ../../../config.pri )

# Prepare package installation path
include( $${PACKAGE_PWD}/install.pri )

# Add SDKs
include ( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri)
include ( $${PACKAGE_LIBS_SOURCES}/sdk_usertoolkit.pri)
INCLUDEPATH += $${PWD}

# DEPENDPATH = ../
# LIBS = -L$${PACKAGE_BUILD_PATH}
# mac:*-g++:LIBS *= -Wl,-all_load # import all symbols as the not used ones too
# else:*-g++:LIBS *= -Wl,--whole-archive # import all symbols as the not used ones too
# mac:*-g++:LIBS *= -dynamic
# else:unix:*-g++:LIBS *= -rdynamic
PRE_TARGETDEPS *= $${PACKAGE_LIBS_SOURCES}/toolkit \
    $${PACKAGE_LIBS_SOURCES}/usertoolkit

# define Qt
QT *= sql \
    network

# add compiled libs
# CONFIG( debug, debug|release ) {
# # Debug
# win32:LIBS *= -ltoolkit_d \
# -lusertoolkit_d
# else:LIBS *= -ltoolkit_debug \
# -lusertoolkit_debug
# mac:*-g++:LIBS *= -Wl,-noall_load # stop importing all symbols
# else:*-g++:LIBS *= -Wl,--no-whole-archive # stop importing all symbols
# }
# else {
# # Release
# LIBS *= -lusertoolkit \
# -ltoolkit
# win32-*g++:LIBS *= -Wl,--out-implib,$${PACKAGE_BUILD_PATH}/lib$${TARGET}.a,--enable-extra-pe-debug
# win32-msvc*:LIBS *= /IMPLIB:$${PACKAGE_BUILD_PATH}/$${TARGET}.lib \
# -lshell32
# }
# define some path in order to access GLOBALRESOURCES
# FMF_GLOBAL_RESOURCES = $${PWD}/../../../global_resources
# mac:FMF_BIN_PATH = $${PWD}
# else:unix|win32:FMF_BIN_PATH = $${PWD}
# DEFINES += PACKAGE_VERSION \
# "FMF_BIN_PATH=\"\\\"$${FMF_BIN_PATH}\\\"\"" \
# "FMF_GLOBAL_RESOURCES=\"\\\"$$:FMF_GLOBAL_RESOURCES\\\"\""
# Compile
SOURCES += main.cpp \
    UserManagerMainWindow.cpp
HEADERS += UserManagerMainWindow.h
