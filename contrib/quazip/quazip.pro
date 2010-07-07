TEMPLATE = subdirs

win32 {
    SUBDIRS += zlib-1.2.3
    CONFIG *= ordered
}

SUBDIRS += quazip


