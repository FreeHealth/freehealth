include ( checkqtversion.pri )
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS += \
           freediams/libs \
           freediams

ProjectSettings { 
    EDITOR = QMakeProjectEditor
    QT_VERSION = 4.5
    BUILDER = GNUMake
    COMPILER = G++
}

OTHER_FILES += update-di.txt
