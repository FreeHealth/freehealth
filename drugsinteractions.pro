include ( checkqtversion.pri )
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS += \
           libs/toolkit \
           libs/medintuxtoolkit \
           libs/medicaltoolkit \
           drugsinteractions

ProjectSettings { 
    EDITOR = QMakeProjectEditor
    QT_VERSION = 4.5
    BUILDER = GNUMake
    COMPILER = G++
}
