#include "ledgerviewer.h"
#include "ui_ledgerviewer.h"
#include "ledgerIO.h"
#include "ledgermanager.h"
#include "ledgeredit.h"

#include <QDebug>
#include <QRect>
#include <QMessageBox>

LedgerViewer::LedgerViewer(QWidget * parent): QWidget(parent),ui(new Ui::LedgerViewerWidget){
    ui->setupUi(this);
    m_currency = "euros";
    LedgerManager *lm = new LedgerManager(this);
        QStringList listOfMonths;
    listOfMonths = lm->getListOfMonths();
    ui->monthsComboBox->addItems(listOfMonths);
    QStringList listOfYears;
    QString currentDate = QDate::currentDate().toString("yyyy");
    qDebug() << __FILE__ << QString::number(__LINE__) << " 1 ="  ;
    listOfYears << currentDate;
    qDebug() << __FILE__ << QString::number(__LINE__) << " 2 ="  ;
    listOfYears << lm->getListOfYears();
    qDebug() << __FILE__ << QString::number(__LINE__) << " 3 ="  ;
    listOfYears.removeDuplicates();
    ui->yearsComboBox->addItems(listOfYears);
    

    createActions();
    createMenus();
    fillMenuBar();
}

LedgerViewer::~LedgerViewer(){}

void LedgerViewer::fillMenuBar(){
    m_menuBar = new QMenuBar(this);
    m_menuBar->setAttribute(Qt::WA_TranslucentBackground);
    m_menuBar->setWindowOpacity(0.0);
    m_menuBar->addMenu(m_menuWidgetAction);
    m_menuBar->addMenu(m_menuAnalyze);
    m_menuBar->addMenu(m_ledger);
    ui->menuHorizontalLayout->addWidget(m_menuBar);
}

void LedgerViewer::createMenus(){
    m_menuWidgetAction = new QMenu("&File",this);
    m_menuWidgetAction->addAction(m_closeAction);
    m_menuAnalyze = new QMenu("&Analyse",this);
    m_menuAnalyze->addAction(m_monthlyReceiptsAnalysis);
    m_menuAnalyze->addAction(m_monthlyAndTypeReceiptsAnalysis);
    m_menuAnalyze->addAction(m_yearlyAndTypeReceiptsAnalysis);
    m_menuAnalyze->addAction(m_monthlyMovementsAnalysis);
    m_menuAnalyze->addAction(m_monthlyAndTypeMovementsAnalysis);
    m_menuAnalyze->addAction(m_yearlyAndTypeMovementsAnalysis);
    m_ledger = new QMenu("&Ledger",this);
    m_ledger->addAction(m_ledgerActionShow);
}

void LedgerViewer::createActions(){
    m_closeAction = new QAction(trUtf8("E&xit"),this);
    m_closeAction->setShortcuts(QKeySequence::Close);
    m_closeAction->setStatusTip(trUtf8("Close Ledger"));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(close()));
    
    m_monthlyReceiptsAnalysis = new QAction(trUtf8("Receipts by month"),this);
    m_monthlyReceiptsAnalysis->setStatusTip(trUtf8("See receipts by month."));
    connect(m_monthlyReceiptsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyReceiptsAnalysis()));
    
    m_monthlyAndTypeReceiptsAnalysis = new QAction(trUtf8("Receipts by month and type"),this);
    m_monthlyAndTypeReceiptsAnalysis->setStatusTip(trUtf8("See receipts by month and type."));
    connect(m_monthlyAndTypeReceiptsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyAndTypeReceiptsAnalysis()));
    
    m_yearlyAndTypeReceiptsAnalysis = new QAction(trUtf8("Receipts by year and type"),this);
    m_yearlyAndTypeReceiptsAnalysis->setStatusTip(trUtf8("See receipts by year and type."));
    connect(m_yearlyAndTypeReceiptsAnalysis, SIGNAL(triggered()), this, SLOT(yearlyAndTypeReceiptsAnalysis()));
    
    m_monthlyMovementsAnalysis = new QAction(trUtf8("Movements by month"),this);
    m_monthlyMovementsAnalysis->setStatusTip(trUtf8("See receipts by month."));
    connect(m_monthlyMovementsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyMovementsAnalysis()));
    
    m_monthlyAndTypeMovementsAnalysis = new QAction(trUtf8("Movements by month and type"),this);
    m_monthlyAndTypeMovementsAnalysis->setStatusTip(trUtf8("See receipts by month and type."));
    connect(m_monthlyAndTypeMovementsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyAndTypeMovementsAnalysis()));
    
    m_yearlyAndTypeMovementsAnalysis = new QAction(trUtf8("Movements by year and type"),this);
    m_yearlyAndTypeMovementsAnalysis->setStatusTip(trUtf8("See receipts by year and type."));
    connect(m_yearlyAndTypeMovementsAnalysis, SIGNAL(triggered()), this, SLOT(yearlyAndTypeMovementsAnalysis()));
    
    m_ledgerActionShow = new QAction(trUtf8("&Ledger"),this);
    m_ledgerActionShow->setStatusTip(trUtf8("See ledger."));
    connect(m_ledgerActionShow, SIGNAL(triggered()), this, SLOT(ledgerActionShow()));
    
}

void LedgerViewer::monthlyReceiptsAnalysis(){
    LedgerManager lm(this);
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    AccountModel * model = lm.getModelMonthlyReceiptsAnalysis(this,
                                                              month,
                                                              year);
    ui->tableView->setModel(model);
    QString labelText = "Total = "+QString::number(lm.m_sums)+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::monthlyAndTypeReceiptsAnalysis(){
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    LedgerManager lm(this);
    QSqlTableModel * model = lm.getModelMonthlyAndTypeReceiptsAnalysis(this,
                                                                       month,
                                                                       year);
    ui->tableView->setModel(model);
    QString labelText = "Total = "+QString::number(lm.m_sums)+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::yearlyAndTypeReceiptsAnalysis(){
    LedgerManager lm(this);
    QString year = ui->yearsComboBox->currentText();
    QSqlTableModel * model = lm.getModelYearlyAndTypeReceiptsAnalysis(this,year);
    ui->tableView->setModel(model);
    QString labelText = "Total = "+QString::number(lm.m_sums)+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::monthlyMovementsAnalysis(){
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    LedgerManager lm(this);
    MovementModel * model = lm.getModelMonthlyMovementsAnalysis(this,month,year);
    ui->tableView->setModel(model);
    QString labelText = "Total = "+QString::number(lm.m_sums)+" "+m_currency;
    ui->sumLabel->setText(labelText);
    
}
void LedgerViewer::monthlyAndTypeMovementsAnalysis(){
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    LedgerManager lm(this);
    QStandardItemModel * model = lm.getModelMonthlyAndTypeMovementAnalysis(this,
                                                                           month,
                                                                           year);
    ui->tableView->setModel(model);
    QString labelText = "Total = "+QString::number(lm.m_sums)+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::yearlyAndTypeMovementsAnalysis(){
    QString year = ui->yearsComboBox->currentText();
    LedgerManager lm(this);
    QStandardItemModel * model = lm.getModelYearlyAndTypeMovementAnalysis(this,year);
    ui->tableView->setModel(model);
    QString labelText = "Total = "+QString::number(lm.m_sums)+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::ledgerActionShow(){
    ui->sumLabel->setText("");
    LedgerEdit * ledger = new LedgerEdit(this);
    ledger->show();
}
