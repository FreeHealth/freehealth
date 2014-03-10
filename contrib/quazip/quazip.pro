# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

    # ZLib notes:
    # We need to compile a comptabile zlib version in some particular cases.
    # The internal zlib is built as a static lib.

    # On Mac OS X the zlib version is too old (1.2.3 on 10.6.8), and we need the newest one.
    # So without distinction of OS version we compile the included zlib to use it as internal.

    # On Windows* there are no available precompiled zlib.

    # For all Linux/FreeBSD version, user will install FreeMedForms' apps using his prefered
    # package manager, and all apps will depend on a compatible zlib version.

macx|win32 {
    SUBDIRS += zlib-1.2.8
    CONFIG *= ordered
}

    # When we are sure that the zlib is correctly built, we can then build the quazip wrapper.
SUBDIRS += quazip


