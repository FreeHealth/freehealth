include ( checkqtversion.pri )

TEMPLATE = subdirs

include( config.pri )

CONFIG *= ordered

SUBDIRS += contrib/quazip \
           libs \
           freemedforms \
           plugins


ProjectSettings {
    EDITOR = QMakeProjectEditor
    EXECUTE_RELEASE = bin/freemedforms
    QT_VERSION = 4.5
    EXECUTE_DEBUG = bin/freemedforms_debug
    BUILDER = GNUMake
    COMPILER = G++
}

OTHER_FILES += update-fmf.txt
