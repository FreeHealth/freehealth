include ( checkqtversion.pri )
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS += \
           freediams/libs \
           freediams/plugins \
           freediams \

OTHER_FILES += update-di.txt \
               README \
               COPYING \
               INSTALL \
               global_resources/textfiles/freediams.desktop \
               #global_resources/textfiles/freediams_win32_icon.rc \
               global_resources/textfiles/dataintest.xml
