include(../buildspecs/checkqtversion.pri)

TEMPLATE = subdirs

include(../doc/api-doc.pri)

CONFIG *= ordered

SUBDIRS += \
    appnamedefine \
    libs \
    plugins \
    freemedforms-src
    #../qtcreator_wizards
