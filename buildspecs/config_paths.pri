defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

defineReplace(targetPath) {
    win32:1 ~= s|/|\|g
    return($$1)
}

# For use in custom compilers which just copy files
win32:i_flag = i
defineReplace(stripSrcDir) {
    win32 {
        !contains(1, ^.:.*):1 = $$OUT_PWD/$$1
    } else {
        !contains(1, ^/.*):1 = $$OUT_PWD/$$1
    }
    out = $$cleanPath($$1)
    out ~= s|^$$re_escape($$PWD/)||$$i_flag
    return($$out)
}

# define some paths related to application sources
#SOURCES_ROOT_PATH is defined in config.pri
SOURCES_LIBS_PATH          = $${SOURCES_ROOT_PATH}/libs
SOURCES_PLUGINS_PATH       = $${SOURCES_ROOT_PATH}/plugins
SOURCES_CONTRIBS_PATH      = $${SOURCES_ROOT_PATH}/contrib
SOURCES_GLOBAL_RESOURCES   = $${SOURCES_ROOT_PATH}/global_resources
SOURCES_RESOURCES_SQL      = $${SOURCES_GLOBAL_RESOURCES}/sql
SOURCES_RESOURCES_TEXTFILES= $${SOURCES_GLOBAL_RESOURCES}/textfiles
SOURCES_TRANSLATIONS_PATH  = $${SOURCES_GLOBAL_RESOURCES}/translations
SOURCES_FORMS              = $${SOURCES_GLOBAL_RESOURCES}/forms
SOURCES_PROFILES           = $${SOURCES_GLOBAL_RESOURCES}/profiles
SOURCES_DATABASES          = $${SOURCES_GLOBAL_RESOURCES}/databases
SOURCES_PROTECTED_PATH     = $${SOURCES_ROOT_PATH}/_protected
SOURCES_COMMON_CORE_PATH   = $${SOURCES_ROOT_PATH}/plugins/coreplugin
#keep old code working
SOURCES_TRANSLATIONS = $${SOURCES_TRANSLATIONS_PATH}

!isEmpty(BUILD_PATH_POSTFIXE):SOURCES_BUILD_PATH = $${SOURCES_ROOT_PATH}/build/$${BUILD_PATH_POSTFIXE}
else:SOURCES_BUILD_PATH = $${SOURCES_ROOT_PATH}/build

message(Building into path $${SOURCES_BUILD_PATH})

#exists( $${SOURCES_PROTECTED_PATH} ){
#    message( *** BUILDING FULL APPLICATION *** )
    DEFINES *= FULLAPPLICATION_BUILD
#}

# define some paths related to application sources
BUILD_BINARY_PATH         = $${SOURCES_ROOT_PATH}/bin/$${LOWERED_APPNAME}
BUILD_LIB_PATH            = $${BUILD_BINARY_PATH}/plugins
BUILD_PLUGIN_PATH         = $${BUILD_LIB_PATH}

RELEASE_BINARY_PATH       = $${SOURCES_ROOT_PATH}/packages
# redefine binary target in debug mode add _d or _debug
BINARY_TARGET             = $$quote($${TARGET})

CONFIG(debug, debug|release) {
 unix:BINARY_TARGET   = $$quote($$join(BINARY_TARGET,,,_debug))
 else:BINARY_TARGET  = $$quote($$join(BINARY_TARGET,,,_d))
}
DEFINES	*= "BINARY_NAME=\"\\\"$${BINARY_TARGET}\\\"\"" \
           "BUILD_PATH_POSTFIXE=\"\\\"$${BUILD_PATH_POSTFIXE}\\\"\"" \
           "PACKAGE_VERSION=\"\\\"$${PACKAGE_VERSION}\\\"\""

# define building path
unix:OBJECTS_DIR   = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/unix
win32:OBJECTS_DIR  = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/win32
mac:OBJECTS_DIR    = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/mac
CONFIG(crosscompil):OBJECTS_DIR = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/win-x
UI_DIR	           = $${SOURCES_BUILD_PATH}/.ui
MOC_DIR	           = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.moc
RCC_DIR	           = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.rcc

# define some usefull values
QMAKE_TARGET_COMPANY       = "Eric MAEKER and the FreeMedForms Team"
QMAKE_TARGET_PRODUCT       = $${BINARY_TARGET}
QMAKE_TARGET_DESCRIPTION   = "Crossplatform Medical Forms Generator"
QMAKE_TARGET_COPYRIGHT     = "Copyright (C) 2008-2012 Eric MAEKER, MD"
PACKAGE_DOMAIN             = "http://www.freemedforms.com"
