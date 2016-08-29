!CONFIG(norpath) {
    macx {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/$${RPATH_LIBS_BIN}/
    } else:linux*|hurd*|glibc* {
        #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
        # this expands to $ORIGIN (after qmake and make), it does NOT read a qmake var

        !CONFIG(debug_without_install):CONFIG(LINUX_INTEGRATED){
            # rpath for interdependant libs and plugs (everyone in the same dir)
            #QMAKE_RPATHDIR += \$\$ORIGIN
            # rpath application to the libs path. Here the path must be an absolute path (eg:/usr/lib/appname/).
            #QMAKE_RPATHDIR += $${RPATH_LIBS_BIN}

            # libs and plugs are installed in the same place
            QMAKE_RPATHDIR += \$\$ORIGIN
            # only the application binary needs to know where are the libs and plugs
            QMAKE_RPATHDIR += /usr/$${LIBRARY_BASENAME}/$${LOWERED_APPNAME}
            QMAKE_RPATHDIR += /usr/$${LIBRARY_BASENAME}/freehealth-common
        }else{
            # rpath for interdependant libs and plugs (everyone in the same dir)
            QMAKE_RPATHDIR += \$\$ORIGIN
            # rpath application to the libs path. Here the path must be a relative path to the application binary.
            QMAKE_RPATHDIR += \$\$ORIGIN/$${RPATH_LIBS_BIN}
        }

        PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")
        QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${PLUGIN_RPATH}\'
        QMAKE_RPATHDIR =
    }
}


