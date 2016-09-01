# This is the include file to add to your plugins project

# set plugin name define
isEmpty(TARGET):error(You must specify a target when including fmf_plugins.pri)

# include optional plugin configuration
include(../buildspecs/optionalplugins.pri)

# manage pre tag translations
PRE_TRANSLATION  = plugin

defineReplace(stripOutDir) {
    return($$relative_path($$1, $$OUT_PWD))
}

PLUGINSPEC = $${_PRO_FILE_PWD_}/$${TARGET}.pluginspec
JSON = $${TARGET}.json

# include config file
include($${PWD}/../buildspecs/config.pri)
DESTDIR =  $$BUILD_PLUGIN_PATH
DEPENDPATH += $$SOURCES_ROOT_PATH/src/plugins

# copy pluginspec to the right location
copy2build.input = PLUGINSPEC
copy2build.output = $$DESTDIR/${QMAKE_FUNC_FILE_IN_stripOutDir}
isEmpty(vcproj):copy2build.variable_out = PRE_TARGETDEPS
copy2build.commands = $$QMAKE_COPY \"${QMAKE_FILE_IN}\" \"${QMAKE_FILE_OUT}\"
copy2build.name = COPY ${QMAKE_FILE_IN}
copy2build.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += copy2build

include(../libs/rpath.pri)

#   Create a Json file containing the plugin information required by
#   Qt 5's plugin system by running a XSLT sheet on the
#   pluginspec file before moc runs.
XMLPATTERNS = $$targetPath($$[QT_INSTALL_BINS]/xmlpatterns)

pluginspec2json.name = Create Qt 5 plugin json file
pluginspec2json.input = PLUGINSPEC
pluginspec2json.variable_out = GENERATED_FILES
pluginspec2json.output = $${JSON}
pluginspec2json.commands = $$XMLPATTERNS -no-format -output $$pluginspec2json.output $$SOURCES_PLUGINS_PATH/pluginjsonmetadata.xsl $$PLUGINSPEC
pluginspec2json.CONFIG += no_link
moc_header.depends += $$pluginspec2json.output
QMAKE_EXTRA_COMPILERS += pluginspec2json

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

# put .pro file directory in INCLUDEPATH
#CONFIG += include_source_dir

CONFIG += plugin plugin_with_soname
linux*:QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF
