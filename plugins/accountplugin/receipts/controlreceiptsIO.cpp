/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "controlreceiptsIO.h"
#include "ui_controlreceipts.h"

#include <QMessageBox>
#include <QDebug>
using namespace AccountDB;
using namespace Constants;
ControlReceipts::ControlReceipts(QWidget * parent):QWidget(parent),ui(new Ui::ControlReceiptsWidget){
    ui->setupUi(this);
    int h = parent->height();
    int w = parent->width();
    resize(w,h);
    setAutoFillBackground(true);
    ui->resultLabel->setText("");
    m_accountModel = new AccountModel(this);
    if (!m_accountModel->setHeaderData(ACCOUNT_PATIENT_NAME,Qt::Horizontal ,trUtf8("Patient") , Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to set header data" ;
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << " headerData =" << m_accountModel->headerData(ACCOUNT_PATIENT_NAME,Qt::Horizontal, Qt::DisplayRole).toString() ;
    m_accountModel->setHeaderData(ACCOUNT_DATE,Qt::Horizontal ,trUtf8("Date") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_MEDICALPROCEDURE_TEXT,Qt::Horizontal ,trUtf8("Acts") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_COMMENT,Qt::Horizontal ,trUtf8("Comment") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_CASHAMOUNT,Qt::Horizontal ,trUtf8("Cash") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_CHEQUEAMOUNT,Qt::Horizontal ,trUtf8("Checks") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_VISAAMOUNT,Qt::Horizontal ,trUtf8("Credit card") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_INSURANCEAMOUNT,Qt::Horizontal ,trUtf8("Insurance") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_OTHERAMOUNT,Qt::Horizontal ,trUtf8("Other") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_DUEAMOUNT,Qt::Horizontal ,trUtf8("Due") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_DUEBY,Qt::Horizontal ,trUtf8("by") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_ISVALID,Qt::Horizontal ,trUtf8("Valid") , Qt::EditRole);
    m_userUuid = m_accountModel->getUserUuid();
    m_typeOfMoney = trUtf8("Euros");
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
    search();
    connect(ui->searchButton,SIGNAL(pressed()),this,SLOT(search()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteLine()));
    connect(ui->duesButton,SIGNAL(pressed()),this,SLOT(printDues()));
    connect(ui->printButton,SIGNAL(pressed()),this,SLOT(print()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(closeAction()));
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
    filter += QString("%1 NOT LIKE '%2' AND ").arg(field,"0.0");
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_accountModel->filter() ;
    ui->tableView->setModel(m_accountModel);
    ui->tableView->setColumnHidden(ACCOUNT_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_USER_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_PATIENT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_SITE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_INSURANCE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_MEDICALPROCEDURE_XML,true);
    ui->tableView->setColumnHidden(ACCOUNT_TRACE,true);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    //ui->tableView->resizeColumnsToContents();
    QString textResult = textOfSums(m_accountModel);
    ui->resultLabel->setText(textResult);
    //refreshFilter(filter);
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
  QString textResult = textOfSums(m_accountModel);
  ui->resultLabel->setText(textResult);
  const QString filter = m_accountModel->filter();
  qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
  refreshFilter(filter);
}

QString ControlReceipts::textOfSums(AccountModel * model){
    QString labelText;
    QString labelTextStr;
    double cash = 0.00;
    double chq = 0.00;
    double visa = 0.00;
    double dues = 0.00;
    double totalReceived = 0.00;
    double totals = 0.00;
   int modelRowCount = model->rowCount(QModelIndex());
   qDebug() << __FILE__ << QString::number(__LINE__) << " modelRowCount = " << QString::number(modelRowCount);
   for(int i = 0; i < modelRowCount ; i ++){
       QSqlRecord rowRecord = model->record(i);//ligne d'enregistrement
       cash  += rowRecord.value(ACCOUNT_CASHAMOUNT).toDouble();
       chq  += rowRecord.value(ACCOUNT_CHEQUEAMOUNT).toDouble();
       visa += rowRecord.value(ACCOUNT_VISAAMOUNT).toDouble();
       dues  += rowRecord.value(ACCOUNT_DUEAMOUNT).toDouble();
       }
    totals = cash + chq + visa + dues;
    totalReceived = cash + chq + visa;
    QString totStr = "<font size = 3 color = ""blue"">Totaux = </font><font size = 3 color = ""red"">"+QString::number(totals)+" "+m_typeOfMoney+" </font><br/>";
    QString totReceived = "<font size = 3 color = ""blue"">Totaux reçus = </font><font size = 3 color = ""red"">"+QString::number(totalReceived)+" "+m_typeOfMoney+" </font><br/>";
    QString sumsStr = "<font size = 3 color = ""blue"">Esp = </font><font size = 3 color = ""red"">"+QString::number(cash)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Chq = </font><font size = 3 color = ""red"">"+QString::number(chq)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">CB = </font><font size = 3 color = ""red"">"+QString::number(visa)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">dues = </font><font size = 3 color = ""red"">"+QString::number(dues)+" "+m_typeOfMoney+"</font>";
    labelTextStr = totStr+totReceived+sumsStr;
    labelText = "<html><body>"+labelTextStr+"</body></html>";
    return labelText;
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

void ControlReceipts::refreshFilter(const QString & filter){
    delete m_accountModel;
    m_accountModel = new AccountModel(this);
    m_accountModel->setFilter(filter);
    ui->tableView->setModel(m_accountModel);
    ui->tableView->setColumnHidden(ACCOUNT_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_USER_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_PATIENT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_SITE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_INSURANCE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_MEDICALPROCEDURE_XML,true);
    ui->tableView->setColumnHidden(ACCOUNT_TRACE,true);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void ControlReceipts::closeAction(){
    emit isClosing();
    emit close();
}
