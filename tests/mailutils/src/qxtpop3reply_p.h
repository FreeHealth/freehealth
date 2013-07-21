#ifndef QXTPOP3COMMAND_P_H
/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/

#define QXTPOP3COMMAND_P_H

#include "qxtpop3reply.h"

#include <QTimer>

class QxtPop3ReplyPrivate;
class QxtPop3Private;
class QxtPop3ReplyImpl
{
public:
    QxtPop3ReplyImpl(QxtPop3ReplyPrivate& reply): m_reply(reply) {}
    virtual ~QxtPop3ReplyImpl() {}

    virtual QByteArray dialog(QByteArray received) = 0;

    static QByteArray buildCmd(QByteArray cmd, QByteArray arg)
    {
        return cmd + " " + arg + "\r\n";
    }

    static bool isAnswerOK(QByteArray line)
    {
        return line.indexOf("+OK") == 0;
    }

    static bool isAnswerERR(QByteArray line)
    {
        return line.indexOf("-ERR") == 0;
    }

protected:
    QxtPop3ReplyPrivate& m_reply;

};

class QxtPop3ReplyPrivate : public QObject, public QxtPrivate<QxtPop3Reply>
{
    Q_OBJECT
public:
    QxtPop3ReplyPrivate();
    virtual ~QxtPop3ReplyPrivate() {if (impl) delete impl;}

    QXT_DECLARE_PUBLIC(QxtPop3Reply)

    void finish(int code) {emit qxt_p().finished(code);}
    void progress(int pc) {emit qxt_p().progress(pc);}
    QTimer timer;

    virtual void run();
    QxtPop3ReplyImpl* impl;
    QxtPop3Reply::Status status;
    QxtPop3Reply::Type type;
    int timeout;
    QString errString;

public slots:
    void timedOut();
private:
    Q_DISABLE_COPY(QxtPop3ReplyPrivate)

};

class QxtPop3AuthReply: public QxtPop3Reply
{
    friend class QxtPop3Private;
private:
    QxtPop3AuthReply(QxtPop3Private* pop, int timeout, QObject* parent = 0);
    Q_DISABLE_COPY(QxtPop3AuthReply)
};

class QxtPop3DeleReply: public QxtPop3Reply
{
    friend class QxtPop3;
private:
    QxtPop3DeleReply(int which, int timeout, QObject* parent = 0);
};


class QxtPop3ResetReply: public QxtPop3Reply
{
    friend class QxtPop3;
private:
    QxtPop3ResetReply(int timeout, QObject* parent = 0);
};


class QxtPop3QuitReply: public QxtPop3Reply
{
    friend class QxtPop3;
private:
    QxtPop3QuitReply(int timeout, QObject* parent = 0);
};

#endif // QXTPOP3COMMAND_P_H
