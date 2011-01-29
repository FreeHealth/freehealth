/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *   pm.desombre@medsyn.fr                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "receiptviewer.h"
#include "receiptsmanager.h"	
#include "receiptsIO.h"
#include "findReceiptsValues.h"
#include "choiceDialog.h"

#include "constants.h"

#include "ui_receiptviewer.h"

#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/workingplacesmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <QAbstractItemModel>

#include <QDebug>
#include <QMessageBox>
#include <QFrame>
#include <QPushButton>
#include <QKeySequence>
#include <QString>
#include <QBrush>
#include <QColor>


using namespace Core;
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
using namespace ReceiptsConstants;
using namespace InternalAmount;
using namespace Constants;


ReceiptViewer::ReceiptViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiptViewer)
{
    m_model = new InternalAmount::AmountModel(this);
    ui->setupUi(this);
    ui->amountsView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->setModel(m_model);
    ui->amountsView->setItemDelegateForColumn(0, new Utils::SpinBoxDelegate(this));
    ui->dateExecution->setDisplayFormat("yyyy-MM-dd");
    ui->dateExecution->setDate(QDate::currentDate());
    ui->datePayment->setDisplayFormat("yyyy-MM-dd");
    ui->datePayment->setDate(QDate::currentDate());
    ui->dateBanked->setDisplayFormat("yyyy-MM-dd");
    ui->dateBanked->setDate(QDate::currentDate());
    ui->dateBook->setDisplayFormat("yyyy-MM-dd");
    ui->dateBook->setDate(QDate::currentDate());
    ui->inputRadioButton->setChecked(true);
    ui->saveAndQuitButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->quitButton->setShortcut(QKeySequence("Ctrl+q"));
    ui->thesaurusButton->setShortcut(QKeySequence("Ctrl+t"));
    ui->returnedListView->setStyleSheet("background-color: rgb(201, 201, 201)");
    fillActionTreeView();
    m_modelReturnedList = new QStringListModel;
    ui->returnedListView->setModel(m_modelReturnedList);
    ui->returnedListView->setEnabled(true);
    ui->returnedListView->show();
    //right click
    m_clear = new QAction(trUtf8("Clear all."),this);
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->saveButton,SIGNAL(pressed()),this,SLOT(save()));
    connect(ui->saveAndQuitButton,SIGNAL(pressed()),this,SLOT(saveAndQuit()));
    connect(ui->thesaurusButton,SIGNAL(pressed()),this,SLOT(saveInThesaurus()));
    connect(ui->actionsTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(treeViewsActions(const QModelIndex&)));
    connect(m_clear,SIGNAL(triggered(bool)),this,SLOT(clearAll(bool)));
}

ReceiptViewer::~ReceiptViewer()
{
    delete ui;
}

void ReceiptViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ReceiptViewer::setPosition(QWidget *parent){
    QRect rect = parent->rect();
    setGeometry(parent->x(),parent->y(),rect.width()-10,rect.height()-10);
}

void ReceiptViewer::deleteLine()
{
    QMessageBox::information(0,"try","delete line",QMessageBox::Ok);
}

void ReceiptViewer::fillActionTreeView(){
    m_actionsTreeModel = new QStandardItemModel;
    QStringList listOfMainActions;
    QMap<QString,QString> parametersMap;
    parametersMap.insert("Debtor","insurance");
    parametersMap.insert("Thesaurus","thesaurus");
    parametersMap.insert("Values","values");
    parametersMap.insert("Sites","sites");
    parametersMap.insert("Prefered Value","Prefered Value");
    listOfMainActions = parametersMap.keys();
    //insert items from tables if available
    QMap<QString,QString> mapSubItems;
    receiptsManager manager;
    QString strKeysParameters;
    foreach(strKeysParameters,listOfMainActions){
        QString table = parametersMap.value(strKeysParameters);
        QStringList listOfItemsOfTable;
        QString null = QString();
        listOfItemsOfTable = manager.getParametersDatas(null,table).keys();//QHash<QString,QVariant> name,uid
        QString strItemsOfTable;
        foreach(strItemsOfTable,listOfItemsOfTable){
            mapSubItems.insertMulti(strKeysParameters,strItemsOfTable);
        }
        //default values if unavailables :
        if (listOfItemsOfTable.size()<1)
        {
        	  if (strKeysParameters == "Debtor")
        	  {
        	       mapSubItems.insertMulti("Debtor","Patient");
                       mapSubItems.insertMulti("Debtor","CPAM28");  
        	      }
        	  else if (strKeysParameters == "Thesaurus")
        	  {
        	       mapSubItems.insertMulti("Thesaurus","CS");
                       mapSubItems.insertMulti("Thesaurus","V");  
        	      }
        	  else if (strKeysParameters == "Sites")
        	  {
        	       mapSubItems.insertMulti("Sites","cabinet");
                       mapSubItems.insertMulti("Sites","clinique");  
        	      }
        	  else
        	  {
        	       qWarning() << __FILE__ << QString::number(__LINE__) 
        	       << " No default value for "<< strKeysParameters ;
        	       }
            }
    }
    QStandardItem *parentItem = m_actionsTreeModel->invisibleRootItem();
    QString strMainActions;
    foreach(strMainActions,listOfMainActions){
        QStandardItem * actionItem = new QStandardItem(strMainActions);
        //treeViewsActions colors
        if (strMainActions == "Debtor")
        {
        	  QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);
            }
        else if (strMainActions == "Prefered Value")
        {
        	  QBrush red(Qt::red);
                  actionItem->setForeground(red);
            }
        else if (strMainActions == "Sites")
        {
        	  QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);       	  
            }
        else if (strMainActions == "Thesaurus")
        {
        	  QBrush red(Qt::red);
                  actionItem->setForeground(red);        	  
            }
        else if (strMainActions == "Values")
        {
        	  QBrush blue(Qt::blue);
                  actionItem->setForeground(blue);        	  
            }
        else{
                  qWarning() << __FILE__ << QString::number(__LINE__) << "Error color treeViewsActions." ;
        }
        
        parentItem->appendRow(actionItem);
        QStringList listSubActions;
        listSubActions = mapSubItems.values(strMainActions);
        QString strSubActions;
        foreach(strSubActions,listSubActions){
            QStandardItem * subActionItem = new QStandardItem(strSubActions);
            actionItem->appendRow(subActionItem);
        }
    }
    ui->actionsTreeView->setHeaderHidden(true);
    ui->actionsTreeView->setStyleSheet("background-color: rgb(201, 201, 201)");
   // ui->actionsTreeView->setStyleSheet("foreground-color: red");
    ui->actionsTreeView->setModel(m_actionsTreeModel);
}

void ReceiptViewer::treeViewsActions(const QModelIndex & index){
    QString data = index.data(Qt::DisplayRole).toString();
    qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data;
    receiptsManager manager;
    QHash<QString,QString> hashOfValues;
    int typeOfPayment = ReceiptsConstants::Cash;
    double percentage = 100.00;
    if(data == "Values"){
        findReceiptsValues * rv = new findReceiptsValues(this);
        if(rv->exec() == QDialog::Accepted){
            hashOfValues = rv -> getChoosenValues();
            choiceDialog choice(rv);
            if(hashOfValues.keys().size() > 0){
                if(choice.exec() == QDialog::Accepted){
                    typeOfPayment = choice.returnChoiceDialog();//int
                    percentage = choice.m_percentValue;//double
                    }
                }
            qDebug() << __FILE__ << QString::number(__LINE__) << " typeOfPayment = "<< QString::number(typeOfPayment);
            }
             /*QStringList*/ m_listOfValues << hashOfValues.keys();
             m_modelReturnedList->setStringList(m_listOfValues);
             fillModel(hashOfValues,typeOfPayment,percentage);
         }
    if(data == "Prefered Value"){// preferential act of payment
        choiceDialog choice(this);
        if(choice.exec() == QDialog::Accepted){
            typeOfPayment = choice.returnChoiceDialog();//int
            percentage = choice.m_percentValue;
            }
            
            hashOfValues.insertMulti("CS","23.00");//preferential act
            m_listOfValues << hashOfValues.keys();
            m_modelReturnedList->setStringList(m_listOfValues);
            fillModel(hashOfValues,typeOfPayment,percentage);
        }
        qDebug() << __FILE__ << QString::number(__LINE__) << "manager.m_hashOfSites.keys()  =" << QString::number(manager.getHashOfSites().keys().size()) ;
        
    if (manager.getHashOfSites().keys().contains(data))
    {
    	  m_siteUid = manager.getHashOfSites().value(data);
    	  qDebug() << __FILE__ << QString::number(__LINE__) << " m_siteUid =" << m_siteUid.toString() ;
        }
    if (manager.getHashOfInsurance().keys().contains(data))
    {
    	  m_insuranceUid = manager.getHashOfInsurance().value(data);
    	  qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUid =" << m_insuranceUid.toString() ;
        }
    if (manager.getHashOfThesaurus().keys().contains(data))
    {
        choiceDialog choice(this);
        if(choice.exec() == QDialog::Accepted){
            typeOfPayment = choice.returnChoiceDialog();//int
            percentage = choice.m_percentValue;
            }        
        }
}

void ReceiptViewer::fillModel(QHash<QString,QString> & hashOfValues, int typeOfPayment, double percentage){
    double value = 0.00;
    QHashIterator<QString,QString> it(hashOfValues);
    while(it.hasNext()){
        it.next();
        value += it.value().toDouble();
    }
    value = value*percentage/100.00;
    qDebug() << __FILE__ << QString::number(__LINE__) << " values =" << QString::number(value);
    const QModelIndex index = m_model->index(typeOfPayment,AmountModel::Col_Value);
    double lastValue = m_model->data(index).toDouble();
    value += lastValue;
    m_model->setData(index, value, Qt::EditRole);
    qDebug() << __FILE__ << QString::number(__LINE__) << " post set data"  ;
}

void ReceiptViewer::save(){
    QString textOfListOfActs = m_listOfValues.join("+");
    double cash = m_model->data(m_model->index(AmountModel::Row_Cash,AmountModel::Col_Value)).toDouble();
    double cheque = m_model->data(m_model->index(AmountModel::Row_Cheque,AmountModel::Col_Value)).toDouble();
    double visa = m_model->data(m_model->index(AmountModel::Row_Visa,AmountModel::Col_Value)).toDouble();
    double banking = m_model->data(m_model->index(AmountModel::Row_Banking,AmountModel::Col_Value)).toDouble();
    double other = m_model->data(m_model->index(AmountModel::Row_Other,AmountModel::Col_Value)).toDouble();
    double due = m_model->data(m_model->index(AmountModel::Row_Du,AmountModel::Col_Value)).toDouble();
    qDebug() << __FILE__ << QString::number(__LINE__) << " values =" << QString::number(cash)+ " "
                                                                     << QString::number(cheque)+ " "
                                                                     << QString::number(visa)+ " "
                                                                     << QString::number(banking)+ " "
                                                                     << QString::number(other)+ " "
                                                                     << QString::number(due) 
                                                                     << "site uid = "+m_siteUid.toString()
                                                                     << "insurance = "+m_insuranceUid.toString();
    QHash<int,QVariant> hash;
    hash.insert(ACCOUNT_UID,"UID");
    hash.insert(ACCOUNT_USER_UID,user()->value(Core::IUser::Uuid).toString());
    hash.insert(ACCOUNT_PATIENT_UID,patient()->data(Core::IPatient::Uid).toString());
    hash.insert(ACCOUNT_PATIENT_NAME,patient()->data(Core::IPatient::FullName).toString());
    hash.insert(ACCOUNT_SITE_ID,m_siteUid);//AccountDB::Constants::SITES_UID
    hash.insert(ACCOUNT_INSURANCE_ID,m_insuranceUid);
    hash.insert(ACCOUNT_DATE,ui->dateExecution->date().toString("yyyy-MM-dd"));
    hash.insert(ACCOUNT_MEDICALPROCEDURE_XML,NULL);
    hash.insert(ACCOUNT_MEDICALPROCEDURE_TEXT,textOfListOfActs);
    hash.insert(ACCOUNT_COMMENT,NULL);
    hash.insert(ACCOUNT_CASHAMOUNT,cash);
    hash.insert(ACCOUNT_CHEQUEAMOUNT,cheque);
    hash.insert(ACCOUNT_VISAAMOUNT,visa);
    hash.insert(ACCOUNT_INSURANCEAMOUNT,banking);
    hash.insert(ACCOUNT_OTHERAMOUNT,other);
    hash.insert(ACCOUNT_DUEAMOUNT,due);
    hash.insert(ACCOUNT_DUEBY,"by");
    hash.insert(ACCOUNT_ISVALID,0);
    hash.insert(ACCOUNT_TRACE,NULL);
    receiptsEngine r;
    if (!r.insertIntoAccount(hash))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Error inserting into AccountModel!"),QMessageBox::Ok);
        }
}

void ReceiptViewer::saveAndQuit(){
    save();
    close();
}

void ReceiptViewer::mousePressEvent(QMouseEvent * event){
  if(event->button() == Qt::RightButton){
    qDebug() << "in right clic" << __FILE__ << QString::number(__LINE__) ;
    m_menu         = new QMenu(this);
    m_menu        -> addAction(m_clear);
    //m_menu        -> exec(QCursor::pos());
    //m_menu->exec(ui->returnedListView->mapToGlobal(QPoint(0, 0)));
    m_menu->exec(event->globalPos());
  }
}
  
void ReceiptViewer::saveInThesaurus(){
    QMessageBox::information(0,"info","save in thesaurus",QMessageBox::Ok);
}

void ReceiptViewer::clearAll(){
    m_listOfValues.clear();
    //m_modelReturnedList->clear();
    
}
