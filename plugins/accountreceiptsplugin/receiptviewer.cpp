#include "receiptviewer.h"
#include "receiptsmanager.h"
#include "findReceiptsValues.h"
#include "choiceDialog.h"

#include "constants.h"

#include "ui_receiptviewer.h"

#include <utils/widgets/spinboxdelegate.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <QAbstractItemModel>

#include <QDebug>
#include <QMessageBox>
#include <QFrame>
using namespace Core;
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
using namespace ReceiptsConstants;
using namespace InternalAmount;


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
    ui->deleteLineButton->hide();
    ui->inputRadioButton->setChecked(true);
    fillActionTreeView();
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->deleteLineButton,SIGNAL(pressed()),this,SLOT(deleteLine()));
    connect(ui->actionsTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(treeViewsActions(const QModelIndex&)));
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
    parametersMap.insert("Preferential rate","preferential rate");
    listOfMainActions = parametersMap.keys();
    //insert items from tables if available
    QMap<QString,QString> mapSubItems;
    receiptsManager manager;
    QString strKeysParameters;
    foreach(strKeysParameters,listOfMainActions){
        QString table = parametersMap.value(strKeysParameters);
        QStringList listOfItemsOfTable;
        QString null = QString();
        listOfItemsOfTable = manager.getParametersDatas(null,table);
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
    ui->actionsTreeView->setStyleSheet("background-color: silver");
    ui->actionsTreeView->setModel(m_actionsTreeModel);
}

void ReceiptViewer::treeViewsActions(const QModelIndex & index){
    QString data = index.data(Qt::DisplayRole).toString();
    qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data;
    QHash<QString,QString> hashOfValues;
    int typeOfPayment = ReceiptsConstants::Cash;
    if(data == "Values"){
        findReceiptsValues * rv = new findReceiptsValues(this);
        if(rv->exec() == QDialog::Accepted){
            hashOfValues = rv -> getChoosenValues();
            choiceDialog choice(rv);
            if(hashOfValues.keys().size() > 0){
                if(choice.exec() == QDialog::Accepted){
                    typeOfPayment = choice.returnChoiceDialog();//int
                    }
                }
            qDebug() << __FILE__ << QString::number(__LINE__) << " typeOfPayment = "<< QString::number(typeOfPayment);
            }
         }
    else if(data == "Preferential rate"){// preferential act of payment
        choiceDialog choice(this);
        if(choice.exec() == QDialog::Accepted){
            typeOfPayment = choice.returnChoiceDialog();//int
            }
        hashOfValues.insert("CS","23.00");//preferential act
        }        
    else{}
    fillModel(hashOfValues,typeOfPayment);
    
}

void ReceiptViewer::fillModel(QHash<QString,QString> & hashOfValues, int typeOfPayment){
    double value = 0.00;
    QHashIterator<QString,QString> it(hashOfValues);
    while(it.hasNext()){
        it.next();
        value += it.value().toDouble();
    }
    qDebug() << __FILE__ << QString::number(__LINE__) << " values =" << QString::number(value);
    const QModelIndex index = m_model->index(typeOfPayment,AmountModel::Col_Value);
    m_model->setData(index, value, Qt::EditRole);
}
