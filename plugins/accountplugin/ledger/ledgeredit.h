#ifndef LEDGEREDIT_H
#define LEDGEREDIT_H
#include "mythread.h"

#include <QWidget>

namespace Ui{
    class LedgerEditWidget;
}

class LedgerEdit : public QWidget {
    Q_OBJECT
    public :
        LedgerEdit(QWidget * parent);
        ~LedgerEdit();
        void resizeLedgerEdit(QWidget * parent);
        
    private slots :
        void showLedger();
        void printLedger();
        void choosenDate(const QString & dateText);
        void fillInfoLabel(const QString & textFromThread);
        void inThread();
        void getDocument();
        void slotDeleteThread();
    signals :
        void deleteThread();
    private :
        Ui::LedgerEditWidget * ui;
        ProduceDoc    *m_myThread;
        QTextDocument *m_doc;
        QDate m_date;
};

#endif

