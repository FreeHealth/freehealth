TARGET = Webcam
TEMPLATE = lib

BUILD_PATH_POSTFIXE = FreeMedForms
PROVIDER = FreeMedForms

include(../fmf_plugins.pri)
include(webcam_dependencies.pri)
include(shared_sources.pri)

OTHER_FILES = Webcam.pluginspec
