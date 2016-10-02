include(../buildspecs/checkqtversion.pri)

TEMPLATE = subdirs

#include(../doc/api-doc.pri)

CONFIG *= ordered C++11

SUBDIRS += \
    appnamedefine \
    libs \
    plugins \
    ehr-src
