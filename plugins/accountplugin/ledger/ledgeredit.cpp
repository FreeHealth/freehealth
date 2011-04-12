#include "ledgeredit.h"
#include "ui_ledgeredit.h"
#include "ledgerIO.h"
#include <QDebug>

LedgerEdit::LedgerEdit(QWidget * parent):QWidget(parent),ui(new Ui::LedgerEditWidget){
    ui->setupUi(this);
    int h = parent->height();
    int w = parent->width();
    resize(w,h);
    setAutoFillBackground(true);
    LedgerIO lio(this);
    QStringList listOfYears;
    QString currentDate = QDate::currentDate().toString("yyyy");
    listOfYears << currentDate;
    listOfYears << lio.getListOfYears();
    listOfYears.removeDuplicates();
    for (int i = 0; i < listOfYears.size(); i += 1)
    {
    	//qDebug() << __FILE__ << QString::number(__LINE__) << " listOfYears[i] =" << listOfYears[i] ;
        }
    ui->yearComboBox->addItems(listOfYears);
    ui->infoLabel->setText("");
    emit choosenDate(currentDate);
    m_doc = new QTextDocument(ui->textEdit);
    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::Base, QColor ("#DDDDDD"));
    ui->textEdit->setPalette(p);
    ui->textEdit->setDocument(m_doc);
    m_myThread = new ProduceDoc();
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->showButton,SIGNAL(pressed()),this,SLOT(showLedger()));
    connect(ui->printButton,SIGNAL(pressed()),this,SLOT(printLedger()));
    connect(ui->yearComboBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(choosenDate(const QString &)));
}

LedgerEdit::~LedgerEdit(){
    delete m_myThread;
    delete m_doc;
}

void LedgerEdit::showLedger(){
    //qDebug() << __FILE__ << QString::number(__LINE__) << " SHOW !!! "  ;
    //m_doc->clear();
    m_myThread->dateChosen(m_date);
    if (m_myThread->isRunning())
    {
    	  m_myThread->terminate();
    	  //qDebug() << __FILE__ << QString::number(__LINE__) << " in  m_myThread->terminate"   ;
        }
    m_myThread->start();
    connect(m_myThread ,SIGNAL(finished()),this,SLOT(getDocument()));
    connect(m_myThread ,SIGNAL(outThread(const QString &)),this,       SLOT(fillInfoLabel(const QString &)));
    connect(m_myThread ,SIGNAL(started()),                 this,       SLOT(inThread()));
    connect(this       ,SIGNAL(deleteThread()),            this,       SLOT(slotDeleteThread()));
}

void LedgerEdit::printLedger(){}

void LedgerEdit::choosenDate(const QString & dateText){
    ////qDebug() << __FILE__ << QString::number(__LINE__) << " dateText =" << dateText ;
    m_date = QDate::fromString(dateText,"yyyy");
}

void LedgerEdit::fillInfoLabel(const QString & textFromThread){
    ui->infoLabel->setText(textFromThread);
}

void LedgerEdit::inThread(){
  m_myThread->dateChosen(m_date);
}

void LedgerEdit::getDocument(){
    ////qDebug() << __FILE__ << QString::number(__LINE__) << " getDocument " ;
    m_doc = m_myThread->getTextDocument()->clone();
    emit  deleteThread();
    ui->textEdit->setDocument(m_doc);
}

void LedgerEdit::slotDeleteThread(){
    if (m_myThread)
    {
        delete m_myThread;    	  
        }

    m_myThread = new ProduceDoc(); 
}

void LedgerEdit::resizeLedgerEdit(QWidget * parent){
    int h = parent->height();
    int w = parent->width();
    resize(w,h);
}
