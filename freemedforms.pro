include (checkqtversion.pri)

TEMPLATE = subdirs

include(doc/api-doc.pri)

CONFIG *= ordered

SUBDIRS += \
           libs \
           freemedforms \
           plugins

OTHER_FILES += update-fmf.txt \
               README \
               COPYING \
               INSTALL \
               global_resources/textfiles/freemedforms-todo.txt \
               global_resources/textfiles/FreeMedForms.desktop \
               freemedforms/Info.plist

