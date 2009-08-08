include ( checkqtversion.pri )

TEMPLATE = subdirs

include(config.pri)
include(doc/api-doc.pri)

CONFIG *= ordered

SUBDIRS += libs \
#           contrib/quazip \
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

