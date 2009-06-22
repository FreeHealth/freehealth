include ( checkqtversion.pri )

TEMPLATE = subdirs

include( config.pri )

CONFIG *= ordered

SUBDIRS += contrib/quazip \
           libs/toolkit \
           libs/usertoolkit \
           freemedforms \
           plugins/formwidgets \
           plugins/io


ProjectSettings {
    EDITOR = QMakeProjectEditor
    EXECUTE_RELEASE = bin/freemedforms
    QT_VERSION = 4.4.3
    EXECUTE_DEBUG = bin/freemedforms_debug
    BUILDER = GNUMake
    COMPILER = G++
}
