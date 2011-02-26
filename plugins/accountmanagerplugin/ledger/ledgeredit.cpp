#include "ledgeredit.h"
#include "ui_ledgeredit.h"
#include "ledgerIO.h"

LedgerEdit::LedgerEdit(QWidget * parent):QWidget(parent),ui(new Ui::LedgerEditWidget){
    ui->setupUi(this);
    setAutoFillBackground(true);
    LedgerIO lio(this);
    QStringList listOfYears;
    QString currentDate = QDate::currentDate().toString("yyyy");
    listOfYears << currentDate;
    listOfYears << lio.getListOfYears();
    listOfYears.removeDuplicates();
    ui->yearComboBox->addItems(listOfYears);
    ui->infoLabel->setText("");
    emit choosenDate(currentDate);
    m_doc = new QTextDocument(ui->textEdit);
    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::Base, QColor ("#DDDDDD"));
    ui->textEdit->setPalette(p);
    ui->textEdit->setDocument(m_doc);
    m_myThread = new ProduceDoc(parent,m_date);
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->showButton,SIGNAL(pressed()),this,SLOT(showLedger()));
    connect(ui->printButton,SIGNAL(pressed()),this,SLOT(printLedger()));
    connect(ui->yearComboBox,SIGNAL(activated(const QString &)),this,SLOT(choosenDate(const QString &)));
    
    connect(m_myThread ,SIGNAL(outThread(const QString &)),this,       SLOT(fillInfoLabel(const QString &)));
    connect(m_myThread ,SIGNAL(started()),                 this,       SLOT(inThread()));
    connect(m_myThread ,SIGNAL(finished()),                this,       SLOT(getDocument()));
    connect(this       ,SIGNAL(deleteThread()),            this,       SLOT(slotDeleteThread()));
}

LedgerEdit::~LedgerEdit(){
    //delete m_doc;
}

void LedgerEdit::showLedger(){
    m_myThread->start();
}

void LedgerEdit::printLedger(){}

void LedgerEdit::choosenDate(const QString & dateText){
    m_date = QDate::fromString(dateText,"yyyy");
}

void LedgerEdit::fillInfoLabel(const QString & textFromThread){
    ui->infoLabel->setText(textFromThread);
}

void LedgerEdit::inThread(){
  m_myThread->dateChosen(m_date);
}

void LedgerEdit::getDocument(){
    m_doc = m_myThread->getTextDocument()->clone();
    emit  deleteThread();
    ui->textEdit->setDocument(m_doc);
}

void LedgerEdit::slotDeleteThread(){
  delete m_myThread;
  m_myThread = new ProduceDoc(this,m_date); 
}
