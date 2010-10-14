include ( checkqtversion.pri )
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS += \
           freeicd/libs \
           freeicd/plugins \
           freeicd

OTHER_FILES += update-di.txt \
               README \
               COPYING \
               INSTALL \
               global_resources/textfiles/freeicd.desktop \
               #global_resources/textfiles/freediams_win32_icon.rc \
               freeicd/Info.plist
