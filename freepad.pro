include ( checkqtversion.pri )
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS += \
           freepad/libs \
           freepad/plugins \
           freepad

OTHER_FILES += README \
               COPYING \
               INSTALL \
               global_resources/textfiles/freepad.desktop \
               freepad/Info.plist
               #global_resources/textfiles/freepad_win32_icon.rc \
