/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <QCoreApplication>

#include <src/qxtpop3.h>

#include <QDebug>


class Pop3 : public QObject
{
    Q_OBJECT
public:
    Pop3(QObject *parent) : QObject(parent) {}
    ~Pop3() {}

    bool initialize()
    {
        pop3 = new QxtPop3(this);
        qWarning() << "oici";
        connect(pop3, SIGNAL(authenticated()), this, SLOT(authenticated()));
        connect(pop3, SIGNAL(connected()), this, SLOT(connected()));
        connect(pop3, SIGNAL(encrypted()), this, SLOT(encrypted()));
        connect(pop3, SIGNAL(disconnected()), this, SLOT(disconnected()));

        connect(pop3, SIGNAL(authenticationFailed(QByteArray)), this, SLOT(authenticationFailed(QByteArray)));
        connect(pop3, SIGNAL(connectionFailed(QByteArray)), this, SLOT(connectionFailed(QByteArray)));
        connect(pop3, SIGNAL(encryptionFailed(QByteArray)), this, SLOT(encryptionFailed(QByteArray)));

        pop3->setUsername("docteur@ericmaeker.fr");
        qWarning() << "oici";
        if (qApp->arguments().count() >= 2)
            pop3->setPassword(qApp->arguments().at(1).toUtf8());
        qWarning() << "oici";
        //pop3->connectToSecureHost("ns0.ovh.net", 110);
        pop3->setStartTlsDisabled(true);
        pop3->connectToHost("ns0.ovh.net", 110);
        qWarning() << "oici";
        return true;
    }

public Q_SLOTS:
    void authenticated()
    {
        qWarning() <<"Auth+";
        _stat = pop3->stat();
        qWarning() << _stat->status() << _stat->error() << _stat->type();
        connect(_stat, SIGNAL(finished(int)), this, SLOT(statFinished(int)));
        connect(_stat, SIGNAL(progress(int)), this, SLOT(progress(int)));
        qWarning() << "isConnected" << pop3->isConnected();
    }

    void connected()
    {
        qWarning() << "Connect+";
    }

    void encrypted()
    {
        qWarning() << "Encrypted+;";
    }

    void disconnected()
    {
        qWarning() << "Disconnect+";
    }

    void authenticationFailed(const QByteArray &b) {qWarning() << "authenticationFailed(QByteArray)" << b;}
    void connectionFailed(const QByteArray &b) {qWarning() << "connectionFailed(QByteArray)" << b;}
    void encryptionFailed(const QByteArray &b) {qWarning() << "encryptionFailed(QByteArray)" << b;}

    void progress(int i)
    {
        qWarning() << "progress" << i;
    }

    void statFinished(int i)
    {
        qWarning() << "statFinished: code" << i << "Count:" << _stat->count() << "Size:" << _stat->size();
        qWarning() << "isConnected" << pop3->isConnected();
        _list = pop3->messageList();
        connect(_list, SIGNAL(finished(int)), this, SLOT(listFinished(int)));
        connect(_list, SIGNAL(progress(int)), this, SLOT(progress(int)));
    }

    void listFinished(int i)
    {
        qWarning() << "listFinished; code:" << i << "listCount:" << _list->list().count();
        foreach(const QxtPop3Reply::MessageInfo &info, _list->list()) {
            qWarning() << info.id << info.size;
            QxtPop3RetrReply *m = pop3->retrieveMessage(info.id);
            connect(m, SIGNAL(finished(int)), this, SLOT(messageFinished(int)));
            _msg.append(m);
        }
    }

    void messageFinished(int)
    {
        QxtPop3RetrReply *m = static_cast<QxtPop3RetrReply*>(sender());
        if (!m)
            return;
        qWarning() << m->message()->rfc2822();
    }

private:
    QxtPop3 *pop3;
    QxtPop3StatReply *_stat;
    QxtPop3ListReply *_list;
    QList<QxtPop3RetrReply*> _msg;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Test POP3 retrier
    Pop3 *pop3 = new Pop3(&app);
    pop3->initialize();

    int r = app.exec();
    return r;
}

#include "main.moc"
