include(../buildspecs/checkqtversion.pri)

TEMPLATE = subdirs

include(../doc/api-doc.pri)

CONFIG *= ordered

SUBDIRS += libs plugins freemedforms-src
