include ( checkqtversion.pri )

TEMPLATE = subdirs

include(config.pri)
include(doc/api-doc.pri)

CONFIG *= ordered

SUBDIRS += \
           libs \
#           contrib/quazip \
           freemedforms \
           plugins

OTHER_FILES += update-fmf.txt

