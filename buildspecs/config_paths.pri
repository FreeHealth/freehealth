defineReplace(cleanPath) {
    return($$clean_path($$1))
}

defineReplace(targetPath) {
    return($$shell_path($$1))
}

# For use in custom compilers which just copy files
defineReplace(stripSrcDir) {
    return($$relative_path($$absolute_path($$1, $$OUT_PWD), $$_PRO_FILE_PWD_))
}

# define some paths related to application sources
#SOURCES_ROOT_PATH is defined in config.pri
SOURCES_ROOT_PATH          = $$cleanPath($$SOURCES_ROOT_PATH)
SOURCES_BUILDSPECS_PATH    = $${SOURCES_ROOT_PATH}/buildspecs
SOURCES_LIBS_PATH          = $${SOURCES_ROOT_PATH}/libs
SOURCES_PLUGINS_PATH       = $${SOURCES_ROOT_PATH}/plugins
SOURCES_CONTRIBS_PATH      = $${SOURCES_ROOT_PATH}/contrib
SOURCES_GLOBAL_RESOURCES   = $${SOURCES_ROOT_PATH}/global_resources
SOURCES_RESOURCES_SQL      = $${SOURCES_GLOBAL_RESOURCES}/sql
SOURCES_RESOURCES_TEXTFILES= $${SOURCES_GLOBAL_RESOURCES}/textfiles
SOURCES_TRANSLATIONS_PATH  = $${SOURCES_GLOBAL_RESOURCES}/translations
SOURCES_FORMS              = $${SOURCES_GLOBAL_RESOURCES}/forms
SOURCES_PROFILES           = $${SOURCES_GLOBAL_RESOURCES}/profiles
SOURCES_FREEDATAPACK_PATH  = $${SOURCES_GLOBAL_RESOURCES}/datapacks/appinstalled
SOURCES_COMMON_CORE_PATH   = $${SOURCES_ROOT_PATH}/plugins/coreplugin
SOURCES_DATABASES          = $${SOURCES_GLOBAL_RESOURCES}/databases
#keep old code working
SOURCES_TRANSLATIONS = $${SOURCES_TRANSLATIONS_PATH}

include($${PWD}/catchspec.pri)
!isEmpty(BUILD_PATH_POSTFIXE):SOURCES_BUILD_PATH = $${SOURCES_ROOT_PATH}/build/Qt$${QT_MAJOR_VERSION}_$${CURRENT_SPEC}/$${BUILD_PATH_POSTFIXE}
else:SOURCES_BUILD_PATH = $${SOURCES_ROOT_PATH}/build/Qt$${QT_MAJOR_VERSION}_$${CURRENT_SPEC}
#message(Building into path $${SOURCES_BUILD_PATH})

#exists( $${SOURCES_PROTECTED_PATH} ){
#    message( *** BUILDING FULL APPLICATION *** )
    DEFINES *= FULLAPPLICATION_BUILD
#}

# define some paths related to application sources
BUILD_BINARY_PATH         = $${SOURCES_ROOT_PATH}/bin/$${LOWERED_APPNAME}_Qt$${QT_MAJOR_VERSION}_$${CURRENT_SPEC}
BUILD_PLUGIN_PATH         = $${BUILD_BINARY_PATH}/plugins
CONFIG(LINUX_INTEGRATED) {
  BUILD_LIB_PATH            = $${BUILD_BINARY_PATH}/../libs
}else{
  BUILD_LIB_PATH            = $${BUILD_PLUGIN_PATH}
}

RELEASE_BINARY_PATH       = $${SOURCES_ROOT_PATH}/packages

# redefine binary target in debug mode add _d or _debug
BINARY_TARGET             = $$quote($${TARGET})
BINARY_POSTFIXE=
!CONFIG(dont_postfixe_binaries):CONFIG(debug, debug|release) {
    unix:BINARY_POSTFIXE= _debug
    else:BINARY_POSTFIXE= _d
    BINARY_TARGET   = $$quote($$join(BINARY_TARGET,,,$${BINARY_POSTFIXE}))
}
include(../buildspecs/projectversion.pri)
DEFINES	*= "BINARY_NAME=\"\\\"$${BINARY_TARGET}\\\"\"" \
           "BUILD_PATH_POSTFIXE=\"\\\"$${BUILD_PATH_POSTFIXE}\\\"\"" \
           "PACKAGE_VERSION=\"\\\"$${PACKAGE_VERSION}\\\"\"" \
           "SOURCES_ROOT_PATH=\"\\\"$${SOURCES_ROOT_PATH}\\\"\""

# define building path
unix:OBJECTS_DIR   = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/unix
win32:OBJECTS_DIR  = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/win32
mac:OBJECTS_DIR    = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/mac
UI_DIR	           = $${SOURCES_BUILD_PATH}/.ui
MOC_DIR	           = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.moc
RCC_DIR	           = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.rcc
