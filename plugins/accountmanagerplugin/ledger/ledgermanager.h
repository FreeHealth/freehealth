#ifndef LEDGERMANAGER_H
#define LEDGERMANAGER_H

#include <QObject>

class LedgerManager : public QObject {
    Q_OBJECT
    public :
        LedgerManager(QObject * parent);
        ~LedgerManager();
};

#endif

