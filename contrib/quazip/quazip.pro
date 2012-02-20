TEMPLATE = subdirs

win32 {
    SUBDIRS += zlib-1.2.6
    CONFIG *= ordered
}

SUBDIRS += quazip


