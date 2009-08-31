TEMPLATE        = lib
TARGET          = Core
PACKAGE_VERSION = 0.0.2

# include shared files for the coreplugin.
# FreeMedForms and derivatives share the same Core basic structure that is
# found in an unique pri file. The files may be included from <coreplugin/...> instead of
# <fmfcoreplugin/...>
include(../coreplugin/common_core.pri)

# specific derivative files
HEADERS += \
    coreplugin.h \
    coreimpl.h \
    mainwindow.h \
    appaboutpage.h
#    maininterface/mainwindow.h \

SOURCES += \
    coreplugin.cpp \
    coreimpl.cpp \
    mainwindow.cpp \
    appaboutpage.cpp
