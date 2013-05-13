QT = core network xml

include(network.pri)

HEADERS += qxtglobal.h qxthmac.h \
    lib/mail_exporter.h \
    lib/widgets/mailaccountwidget.h \
    lib/mailaccount.h \
    lib/models/mailaccountmodel.h

SOURCES += qxtglobal.cpp qxthmac.cpp main.cpp \
    lib/widgets/mailaccountwidget.cpp \
    lib/mailaccount.cpp \
    lib/models/mailaccountmodel.cpp

#contains(DEFINES,HAVE_OPENSSL){
# include(../3rdparty/libssh2/libssh2.pri)
#}

