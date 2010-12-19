macx{
   # Qt 4.7 does not support ppc by default
   CONFIG *= x86 x86-64
   CONFIG  -= ppc

   # define rpath
   CONFIG(debug, debug|release) {
     RPATH_LIBS_BIN   = ../../../plugins/
   } else {
     RPATH_LIBS_BIN   = ../plugins/
   }
}
