#ifndef LEDGERIO_H
#define LEDGERIO_H

#include <QObject>

class LedgerIO : public QObject {
    Q_OBJECT
    public :
        LedgerIO(QObject * parent);
        ~LedgerIO();
};

#endif

