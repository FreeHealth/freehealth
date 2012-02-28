include(buildspecs/checkqtversion.pri)
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS += \
           freeaccount/libs \
           freeaccount/plugins \
           freeaccount \

OTHER_FILES += update-freeaccount.txt \
               README \
               COPYING \
               INSTALL \
               global_resources/textfiles/freeaccount.desktop \
               global_resources/textfiles/freeaccount_win32_icon.rc \
#               global_resources/textfiles/dataintest.xml
