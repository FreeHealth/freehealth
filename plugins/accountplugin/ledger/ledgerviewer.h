#ifndef LEDGERVIEWER_H
#define LEDGERVIEWER_H

#include "ledgeredit.h"
#include "ledgermanager.h"
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>


namespace Ui{
    class LedgerViewerWidget;
}

class LedgerViewer : public QWidget {
    Q_OBJECT
    public :
        LedgerViewer(QWidget * parent = 0);
        ~LedgerViewer();
    private slots :
        void monthlyReceiptsAnalysis();
        void monthlyAndTypeReceiptsAnalysis();
        void yearlyAndTypeReceiptsAnalysis();
        void monthlyMovementsAnalysis();
        void monthlyAndTypeMovementsAnalysis();
        void yearlyAndTypeMovementsAnalysis();
        void ledgerActionShow();

    private :
        void fillMenuBar();
        void createMenus();
        void createActions();
        void resizeEvent(QResizeEvent *event);
        LedgerEdit * m_ledgerEdit;
        LedgerManager *m_lm;
        QMenuBar * m_menuBar;
        QMenu * m_menuWidgetAction;
        QMenu * m_menuAnalyze;
        QMenu * m_ledger;
        QAction * m_closeAction;
        QAction * m_monthlyReceiptsAnalysis;
        QAction * m_monthlyAndTypeReceiptsAnalysis;
        QAction * m_yearlyAndTypeReceiptsAnalysis;
        QAction * m_monthlyMovementsAnalysis;
        QAction * m_monthlyAndTypeMovementsAnalysis;
        QAction * m_yearlyAndTypeMovementsAnalysis;
        QAction * m_ledgerActionShow;
        
        QString m_currency;
        Ui::LedgerViewerWidget * ui;

};

#endif

