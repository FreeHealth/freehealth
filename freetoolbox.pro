include (checkqtversion.pri)

TEMPLATE = subdirs

CONFIG *= ordered

SUBDIRS += \
           libs \
           freetoolbox/plugins \
           freetoolbox

OTHER_FILES += update-toolbox.txt \
               README \
               COPYING \
               COPYING.FREEMEDFORMS \
               INSTALL
