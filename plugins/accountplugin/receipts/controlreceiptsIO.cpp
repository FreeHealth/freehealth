/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
 
#include "controlreceiptsIO.h"
#include "ui_controlreceipts.h"

#include <QMessageBox>
#include <QDebug>

ControlReceipts::ControlReceipts(QWidget * parent):QWidget(parent),ui(new Ui::ControlReceiptsWidget){
    ui->setupUi(this);
    int h = parent->height();
    int w = parent->width();
    resize(w,h);
    setAutoFillBackground(true);
    m_accountModel = new AccountModel(this);
    m_userUuid = m_accountModel->getUserUuid();
    ui->beginDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->searchButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->deleteButton->setShortcut(QKeySequence::Delete);
    ui->quitButton->setShortcut(QKeySequence("Ctrl+q"));
    m_mapCombo.insert(trUtf8("Patient"),"PATIENT_NAME");
    m_mapCombo.insert(trUtf8("Cash"),"CASH");
    m_mapCombo.insert(trUtf8("Check"),"CHEQUE");
    m_mapCombo.insert(trUtf8("Credit card"),"VISA");
    m_mapCombo.insert(trUtf8("Banking"),"BANKING");
    m_mapCombo.insert(trUtf8("Other"),"OTHER");
    m_mapCombo.insert(trUtf8("Due"),"DUE");
    m_mapCombo.insert(trUtf8("Due by"),"DUE_BY");
    QStringList listForCombo;
    listForCombo = m_mapCombo.keys();
    listForCombo.prepend(trUtf8("Patient"));
    listForCombo.removeDuplicates();
    ui->fieldComboBox->addItems(listForCombo);
    ui->fieldComboBox->setEditText(trUtf8("Patient"));
    connect(ui->searchButton,SIGNAL(pressed()),this,SLOT(search()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteLine()));
    connect(ui->duesButton,SIGNAL(pressed()),this,SLOT(printDues()));
    connect(ui->printButton,SIGNAL(pressed()),this,SLOT(print()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
}

ControlReceipts::~ControlReceipts(){}

void ControlReceipts::resizeControlReceipts(QWidget * parent){
    int h = parent->height();
    int w = parent->width();
    resize(w,h);    
}

void ControlReceipts::search(){
    refresh();
    QString dateBeginStr = ui->beginDateEdit->date().toString("yyyy-MM-dd");
    QString dateEndStr = ui->endDateEdit->date().toString("yyyy-MM-dd");
    QString filterEdit = "%"+ui->filterEdit->text()+"%";
    QString fieldKey = ui->fieldComboBox->currentText();
    QString field = m_mapCombo.value(fieldKey);
    QString filter = QString("%1 = '%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("%1 LIKE '%2'").arg(field,filterEdit);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_accountModel->filter() ;
    ui->tableView->setModel(m_accountModel);
}

void ControlReceipts::deleteLine(){
  QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
  if(!index.isValid()){
      QMessageBox::warning(0,trUtf8("Error"),trUtf8("Please select a line to delete."),QMessageBox::Ok);
      return;
      }
  int i = index.row();
  if(m_accountModel->removeRows(i,1,QModelIndex())){
          QMessageBox::information(0,trUtf8("Information"),trUtf8("Line is deleted."),QMessageBox::Ok);
      }
}

void ControlReceipts::printDues(){
    QMessageBox::information(0,trUtf8("Information"),trUtf8("Not yet"),QMessageBox::Ok);
}

void ControlReceipts::print(){
    QMessageBox::information(0,trUtf8("Information"),trUtf8("Not yet"),QMessageBox::Ok);	
}

void ControlReceipts::coloringDoubles(){}

void ControlReceipts::refresh(){
    delete m_accountModel;
    m_accountModel = new AccountModel(this);
}

void ControlReceipts::refreshFilter(const QString & filter){}
