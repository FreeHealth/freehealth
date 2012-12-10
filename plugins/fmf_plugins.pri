#This is the include file to add to your plugins project

# set plugin name define
isEmpty(TARGET):error(You must specify a target when including fmf_plugins.pri)

# manage pre tag translations
PRE_TRANSLATION  = plugin

PLUGINSPECS = $${_PRO_FILE_PWD_}/$${TARGET}.pluginspec

# include config file
include($${PWD}/../buildspecs/config.pri)
DESTDIR =  $$BUILD_PLUGIN_PATH
DEPENDPATH += $$SOURCES_ROOT_PATH/src/plugins

CONFIG *= plugin plugin_with_soname

# copy pluginspec to the right location
copy2build.input = PLUGINSPECS
copy2build.output = $$DESTDIR/${QMAKE_FUNC_FILE_IN_stripSrcDir}
isEmpty(vcproj):copy2build.variable_out = PRE_TARGETDEPS
copy2build.commands = $$QMAKE_COPY \"${QMAKE_FILE_IN}\" \"${QMAKE_FILE_OUT}\"
copy2build.name = COPY ${QMAKE_FILE_IN}
copy2build.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += copy2build

include(../libs/rpath.pri)

#macx {
#    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/$${RPATH_LIBS_BIN}/
#} else:linux-* {
#    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
#    QMAKE_RPATHDIR += \$\$ORIGIN
#    QMAKE_RPATHDIR += \$\$ORIGIN/..
##    QMAKE_RPATHDIR += \$\$ORIGIN/../..
##    QMAKE_RPATHDIR += \$\$ORIGIN/$${RPATH_LIBS_BIN}/
#    IDE_PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")
#    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${IDE_PLUGIN_RPATH}\'
#    QMAKE_RPATHDIR =
#}

