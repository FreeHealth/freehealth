QT = core network

include(network.pri)

HEADERS += qxtglobal.h qxthmac.h \
    lib/widgets/mailaccountwidget.h \
    lib/widgets/mailaccount.h \
    lib/mail_exporter.h
SOURCES += qxtglobal.cpp qxthmac.cpp main.cpp \
    lib/widgets/mailaccountwidget.cpp \
    lib/widgets/mailaccount.cpp

#contains(DEFINES,HAVE_OPENSSL){
# include(../3rdparty/libssh2/libssh2.pri)
#}

