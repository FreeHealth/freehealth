INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD

HEADERS +=\
    src/qxtmail_p.h \
    src/qxtsmtp.h \
    src/qxtsmtp_p.h \
    src/qxtmailattachment.h \
    src/qxtmailmessage.h \
    src/qxtpop3.h \
    src/qxtpop3_p.h \
    src/qxtpop3reply.h \
    src/qxtpop3reply_p.h \
    src/qxtpop3statreply.h \
    src/qxtpop3listreply.h \
    src/qxtpop3retrreply.h \


SOURCES +=\
    src/qxtmailattachment.cpp \
    src/qxtmailmessage.cpp \
    src/qxtsmtp.cpp \
    src/qxtpop3.cpp \
    src/qxtpop3reply.cpp \
