#ifndef DOSAGESENDERTESTER_H
#define DOSAGESENDERTESTER_H

#include <tkSendMessage.h>

#include <QObject>
#include <QHash>

class DosageSenderTester : public QObject
{
    Q_OBJECT
public:
    DosageSenderTester(QObject *parent);
    void start();

public Q_SLOTS:
    void done();

Q_SIGNALS:
    void end();

public:
    tkSendMessage sender;
    QHash<QString,QString> m_Datas;
};

#endif // DOSAGESENDERTESTER_H
