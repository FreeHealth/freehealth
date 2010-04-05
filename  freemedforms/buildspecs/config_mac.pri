macx{
   CONFIG *= x86 ppc

   # define rpath
   CONFIG( debug, debug|release ) {
     CONFIG  -= ppc
     RPATH_LIBS_BIN   = ../../../plugins/
   } else {
     RPATH_LIBS_BIN   = ../plugins/
   }
}
