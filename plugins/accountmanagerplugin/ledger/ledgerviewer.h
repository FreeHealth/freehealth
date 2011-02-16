#ifndef LEDGERVIEWER_H
#define LEDGERVIEWER_H

#include <QWidget>

namespace Ui{
    class LedgerViewerWidget;
}

class LedgerViewer : public QWidget {
    Q_OBJECT
    public :
        LedgerViewer(QWidget * parent = 0);
        ~LedgerViewer();
    
    private :
        Ui::LedgerViewerWidget * ui;
        
};

#endif

