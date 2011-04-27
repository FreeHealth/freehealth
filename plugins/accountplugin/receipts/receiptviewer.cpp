/***************************************************************************
 *  Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *  pm.desombre@medsyn.fr                                                 *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU Library General Public License as       *
 *  published by the Free Software Foundation; either version 3 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU Library General Public     *
 *  License along with this program; if not, write to the                 *
 *  Free Software Foundation, Inc.,                                       *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/***************************************************************************
 *  Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *  Contributors :                                                        *
 *      Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *      NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "receiptviewer.h"
#include "receiptsmanager.h"	
#include "receiptsIO.h"
#include "findReceiptsValues.h"
#include "choiceDialog.h"
#include "distance.h"

#include "constants.h"

#include "ui_receiptviewer.h"

#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/workingplacesmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <utils/widgets/spinboxdelegate.h>

#include <QAbstractItemModel>
#include <QDebug>
#include <QMessageBox>
#include <QFrame>
#include <QPushButton>
#include <QKeySequence>
#include <QString>
#include <QBrush>
#include <QColor>

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

using namespace ReceiptsConstants;
using namespace Constants;

namespace InternalAmount {

    class AmountModel : public QAbstractTableModel
    {
    public:
        enum ColumnRepresentation {
            Col_Value = 0,
            Col_Currency,  // Devise monétaire
            ColCount
        };

        enum RowRepresentation {
            Row_Cash = 0,
            Row_Cheque,
            Row_Visa,
            Row_Banking,  // Virement banquaire
            Row_Other,
            Row_Du,
            RowCount
        };

        AmountModel(QObject *parent = 0) : QAbstractTableModel(parent)
        {
            for(int i=0; i < rowCount(); ++i)
                m_Values.append(0.0);
        }

        int rowCount(const QModelIndex &parent = QModelIndex()) const {return RowCount;}
        int columnCount(const QModelIndex &parent = QModelIndex()) const {return ColCount;}

        bool submit(){
            if (!submit()) {
               return false;
            }
          return true;
        }


        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        {
            if (!index.isValid())
                return QVariant();

            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                switch (index.column()) {
                case Col_Value:
                    return m_Values[index.row()];
                    break;
                default: return QVariant();
                }
            }
            return QVariant();
        }

        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
        {
            if (!index.isValid())
                return false;

            if (role==Qt::EditRole) {
                switch (index.column()) {
                case Col_Value:
                    {
                        m_Values[index.row()] = value.toFloat();
                        Q_EMIT dataChanged(index, index);
                        return true;
                    }
                }
            }
            return false;
        }

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
        {
            if (role==Qt::DisplayRole) {
                if (orientation==Qt::Vertical) {
                    switch (section) {
                    case Row_Cash: return tr("Cash");
                    case Row_Visa: return "Visa";
                    case Row_Cheque: return "Cheque";
                    case Row_Banking: return "Banking";
                    case Row_Other: return "Other";
                    case Row_Du: return "Du";
                    }
                } else if (orientation==Qt::Horizontal) {
                    switch (section) {
                    case Col_Value: return "Value";
                    case Col_Currency : return "Currency";
                    }
                }
            }
            return QVariant();
        }

        QSqlError lastError(){
            return lastError();
            }

        Qt::ItemFlags flags(const QModelIndex &index) const
        {
            if (index.column()==Col_Value) {
                return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
            } else {
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
            }
        }


    private:
        QList<float> m_Values;
    };

}  // End namespace Internal





treeViewsActions::treeViewsActions(QWidget *parent){
    m_deleteThesaurusValue = new QAction(trUtf8("Delete this value."),this);
    m_choosePreferedValue = new QAction(trUtf8("Choose this value like the prefered."),this);
    connect(m_choosePreferedValue,SIGNAL(triggered(bool)),this,SLOT(choosePreferedValue(bool)));
    connect(m_deleteThesaurusValue,SIGNAL(triggered(bool)),this,SLOT(deleteBox(bool)));
    }
    
treeViewsActions::~treeViewsActions(){}

void treeViewsActions::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        if(isChildOfThesaurus()){
            blockSignals(true);
            m_menuRightClic = new QMenu(this); 
            m_menuRightClic -> addAction(m_choosePreferedValue);
            m_menuRightClic-> addAction(m_deleteThesaurusValue);
            m_menuRightClic->exec(event->globalPos());
            blockSignals(false);
        }

    }
    if(event->button() == Qt::LeftButton){
        blockSignals(false);
        QTreeView::mousePressEvent(event);
    }
}

void treeViewsActions::deleteBox(bool b){
    QMessageBox msgBox;
            msgBox.setText("Do you want to delete choosen item ?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            QModelIndex index;
            switch(ret){
                case QMessageBox::Yes :
                    index = currentIndex();
                    deleteItemFromThesaurus(index);
                    break;
                case QMessageBox::No :
                    break;
            }
}

void treeViewsActions::choosePreferedValue(bool b){
    QMessageBox msgBox;
            msgBox.setText("Do you want to choose this item as prefered value ?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            QModelIndex index;
            switch(ret){
                case QMessageBox::Yes :
                    index = currentIndex();
                    if (!addPreferedItem(index))
                    {
                    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to choose this item."),
                    	                       QMessageBox::Ok);
                        }
                    break;
                case QMessageBox::No :
                    break;
            }
} 

bool treeViewsActions::addPreferedItem(QModelIndex &index){
    bool ret = true;
    QString data = index.data().toString();
    receiptsEngine r;
    if (!r.addBoolTrue(data))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Cannot change value bool in thesaurus :")+data,
    	                       QMessageBox::Ok);
    	  ret = false;
        }
    fillActionTreeView();
    return ret;
}

bool treeViewsActions::isChildOfThesaurus(){
    bool ret = true;
    QModelIndex current = currentIndex();
    QModelIndex indexParent = treeModel()->parent(current);
    QString dataParent = treeModel()->data(indexParent).toString();
    qDebug() << __FILE__ << QString::number(__LINE__) << " dataParent =" << dataParent ;
    if (dataParent != "Thesaurus")
    {
    	  ret = false;
        }
    return ret;
}

void treeViewsActions::fillActionTreeView()
{
    m_actionsTreeModel = new QStandardItemModel;
    QStringList listOfMainActions;
    QMap<QString,QString> parametersMap;
    parametersMap.insert("Debtor","insurance");
    parametersMap.insert("Thesaurus","thesaurus");
    parametersMap.insert("Values","values");
    parametersMap.insert("Sites","sites");
    parametersMap.insert("Prefered Value","Prefered Value");
    parametersMap.insert("Round trip","Round trip");
    parametersMap.insert("Distance rules","distance_rules");
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
        	       mapSubItems.insertMulti(strKeysParameters,"Patient");
                       mapSubItems.insertMulti(strKeysParameters,"CPAM28");  
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
        	  else if (strKeysParameters == "Distance rules")
        	  {
        	  	  mapSubItems.insertMulti("Distance rules","DistPrice");
        	      }
        	  else
        	  {
        	       qWarning() << __FILE__ << QString::number(__LINE__) 
        	       << " No default value for "<< strKeysParameters ;
        	       }
            }
    }
    QStandardItem *parentItem = treeModel()->invisibleRootItem();
    QString strMainActions;
    foreach(strMainActions,listOfMainActions){
        qDebug() << __FILE__ << QString::number(__LINE__) << " strMainActions =" << strMainActions ;
        QStandardItem *actionItem = new QStandardItem(strMainActions);
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
        else if (strMainActions == "Round trip")
        {
        	  QBrush blue(Qt::blue);
                  actionItem->setForeground(blue);    
            }
        else if (strMainActions == "Distance rules")
        {
        	  QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);
            }
        else{
                  qWarning() << __FILE__ << QString::number(__LINE__) << "Error color treeViewsActions." ;
        }
        
        parentItem->appendRow(actionItem);
        QStringList listSubActions;
        listSubActions = mapSubItems.values(strMainActions);
        QString strSubActions;
        foreach(strSubActions,listSubActions){
            qDebug() << __FILE__ << QString::number(__LINE__) << " strSubActions =" << strSubActions ;
            QStandardItem *subActionItem = new QStandardItem(strSubActions);
            actionItem->appendRow(subActionItem);            
        }
    }
    qDebug() << __FILE__ << QString::number(__LINE__)  ;
    setHeaderHidden(true);
    setStyleSheet("background-color: rgb(201, 201, 201)");
   // actionsTreeView->setStyleSheet("foreground-color: red");
    setModel(treeModel());
    qDebug() << __FILE__ << QString::number(__LINE__)  ;
}

bool treeViewsActions::deleteItemFromThesaurus(QModelIndex &index){
    bool ret = true;
    QString data = index.data().toString();
    receiptsEngine r;
    if (!r.deleteFromThesaurus(data))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Cannot delete in thesaurus :")+data,QMessageBox::Ok);
    	  ret = false;
        }
    fillActionTreeView();
    return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////RECEIPTS VIEWER/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ReceiptViewer::ReceiptViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiptViewer)
{
    m_userUuid = user()->value(Core::IUser::Uuid).toString();
    receiptsManager rManager;
    m_kilometers = 0.00 ;
    m_distanceRuleValue = 0.00;
    m_insuranceUid = 0;
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
    
    m_modelReturnedList = new QStringListModel;
    ui->returnedListView->setModel(m_modelReturnedList);
    ui->returnedListView->setEnabled(true);
    ui->returnedListView->show();
    actionTreeView = new treeViewsActions(this);
    QVBoxLayout *vbox = new QVBoxLayout;
     vbox->addWidget(actionTreeView);
    ui->actionsBox->setLayout(vbox);
    actionTreeView->fillActionTreeView();
    //preferential choices in the tree view.
    QString site = QString("Sites");
    QString distRule = QString("Distance rules");
    QString debtor = QString("Debtor");
    m_siteUid = firstItemChoosenAsPreferential(site);
    m_distanceRuleValue = firstItemChoosenAsPreferential(distRule).toDouble();
    m_distanceRuleType = rManager.m_preferedDistanceRule.toString();
    m_insuranceUid = firstItemChoosenAsPreferential(debtor);
    qDebug() << __FILE__ << QString::number(__LINE__) 
             << " site,dist,ins prefered =" << m_siteUid.toString()
                                            << QString::number(m_distanceRuleValue)
                                            << m_insuranceUid.toString() ;
    
    //right click
    m_clear = new QAction(trUtf8("Clear all."),this);
    //ui_controlreceipts
    m_control = new ControlReceipts(this);
    m_control->hide();
    ui->displayRadioButton->setCheckable(true);
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->saveButton,SIGNAL(pressed()),this,SLOT(save()));
    connect(ui->saveAndQuitButton,SIGNAL(pressed()),this,SLOT(saveAndQuit()));
    connect(ui->thesaurusButton,SIGNAL(pressed()),this,SLOT(saveInThesaurus()));
    connect(ui->displayRadioButton,SIGNAL(clicked(bool)),this,SLOT(showControlReceipts(bool)));
    
    connect(actionTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(actionsOfTreeView(const QModelIndex&)));
    connect(m_clear,SIGNAL(triggered(bool)),this,SLOT(clearAll(bool)));
    connect(m_control,SIGNAL(isClosing()),this,SLOT(controlReceiptsDestroyed()));
    
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



void ReceiptViewer::actionsOfTreeView(const QModelIndex &index){
    QString data = index.data(Qt::DisplayRole).toString();
    qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data;
    receiptsManager manager;
    QHash<QString,QString> hashOfValues;
    int typeOfPayment = ReceiptsConstants::Cash;
    double percentage = 100.00;
    if(data == "Values"){
        findReceiptsValues *rv = new findReceiptsValues(this);
        if(rv->exec() == QDialog::Accepted){
            hashOfValues = rv -> getChoosenValues();
            choiceDialog choice(rv);
            if(hashOfValues.keys().size() > 0){
                if(choice.exec() == QDialog::Accepted){
                    typeOfPayment = choice.returnChoiceDialog();//int
                    percentage = choice.m_percentValue;//double
                    
                
            //qDebug() << __FILE__ << QString::number(__LINE__) << " typeOfPayment = "<< QString::number(typeOfPayment);
            
             /*QStringList*/ m_listOfValues << hashOfValues.keys();
             m_modelReturnedList->setStringList(m_listOfValues);
             fillModel(hashOfValues,typeOfPayment,percentage);
             }
            }
           }
         }
    if(data == "Prefered Value"){// preferential act of payment
        choiceDialog choice(this);
        if(choice.exec() == QDialog::Accepted){
            typeOfPayment = choice.returnChoiceDialog();//int
            percentage = choice.m_percentValue;
            qDebug() << __FILE__ << QString::number(__LINE__) << " prefered value =" << data ;
            hashOfValues = manager.getPreferentialActFromThesaurus();
            qDebug() << __FILE__ << QString::number(__LINE__) << " preferential acts =" << hashOfValues.keys()[0] ;
            if (hashOfValues.size() < 1)
            {
            	  hashOfValues.insertMulti("NULL","23.00");//preferential act
            	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("You have to insert your prefered "
            	                 	  "value\nin thesaurus\nand choose it as prefered."),QMessageBox::Ok);
                }           
            m_listOfValues << hashOfValues.keys();
            m_modelReturnedList->setStringList(m_listOfValues);
            fillModel(hashOfValues,typeOfPayment,percentage);
            }
        }
        
    if (manager.getDistanceRules().keys().contains(data))
    {
    	  m_distanceRuleValue = manager.getDistanceRules().value(data).toDouble();
    	  m_distanceRuleType = data;
    	  qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleValue =" << QString::number(m_distanceRuleValue) ;
    	  qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleType =" << m_distanceRuleType ;
        }
    if (data == "Round trip")
    {
    	  distance dist(this);
    	  if (dist.exec()== QDialog::Accepted)
    	  {
    	          m_kilometers = dist.getDistanceNumber(m_distanceRuleType);
                  double value = m_kilometers *m_distanceRuleValue;
    	          typeOfPayment = dist.returnDistanceDialog();
    	          percentage = dist.m_percentValue;
    	  	  hashOfValues.insertMulti("DistPrice",QString::number(value));
    	          qDebug() << __FILE__ << QString::number(__LINE__) << " distance =" << QString::number(m_kilometers) ;
    	          qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << QString::number(value) ;
    	          m_listOfValues << trUtf8("Kilometers");
                  m_modelReturnedList->setStringList(m_listOfValues);
    	          fillModel(hashOfValues,typeOfPayment,percentage);
    	      }
    	  
        }
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
            
            QStringList list;
            list = data.split("+");
            receiptsEngine r;
            QHash<QString,QVariant> hashFromMp ;
            hashFromMp = r.getNamesAndValuesFromMP();
            QString str;
            foreach(str,list){
                QString value = hashFromMp.value(str).toString();
                hashOfValues.insertMulti(str,value);
                }
            fillModel(hashOfValues,typeOfPayment,percentage);
            }
        }
        //actionTreeView->reset();
}

void ReceiptViewer::fillModel(QHash<QString,QString> &hashOfValues, int typeOfPayment, double percentage){
    double value = 0.00;
    QHashIterator<QString,QString> it(hashOfValues);
    while(it.hasNext()){
        it.next();
        value += it.value().toDouble();
    }
    value = value*percentage/100.00;
    qDebug() << __FILE__ << QString::number(__LINE__) << " values =" << QString::number(value);
    const QModelIndex index = m_model->index(typeOfPayment, ::InternalAmount::AmountModel::Col_Value);
    double lastValue = m_model->data(index).toDouble();
    value += lastValue;
    m_model->setData(index, value, Qt::EditRole);
}

void ReceiptViewer::save()
{
    using namespace ::Internal;
    QString userUuid = user()->value(Core::IUser::Uuid).toString();
    QString textOfListOfActs = m_listOfValues.join("+");
    double cash = m_model->data(m_model->index(InternalAmount::AmountModel::Row_Cash,InternalAmount::AmountModel::Col_Value)).toDouble();
    double cheque = m_model->data(m_model->index(InternalAmount::AmountModel::Row_Cheque,InternalAmount::AmountModel::Col_Value)).toDouble();
    double visa = m_model->data(m_model->index(InternalAmount::AmountModel::Row_Visa,InternalAmount::AmountModel::Col_Value)).toDouble();
    double banking = m_model->data(m_model->index(InternalAmount::AmountModel::Row_Banking,InternalAmount::AmountModel::Col_Value)).toDouble();
    double other = m_model->data(m_model->index(InternalAmount::AmountModel::Row_Other,InternalAmount::AmountModel::Col_Value)).toDouble();
    double due = m_model->data(m_model->index(InternalAmount::AmountModel::Row_Du,InternalAmount::AmountModel::Col_Value)).toDouble();
    qDebug() << __FILE__ << QString::number(__LINE__) << " values =" << QString::number(cash)+ " "
                                                                     << QString::number(cheque)+ " "
                                                                     << QString::number(visa)+ " "
                                                                     << QString::number(banking)+ " "
                                                                     << QString::number(other)+ " "
                                                                     << QString::number(due) 
                                                                     << "site uid = "+m_siteUid.toString()
                                                                     << "insurance = "+m_insuranceUid.toString();
    QString patientUid = patient()->data(Core::IPatient::Uid).toString();
    if (patientUid.isEmpty())
    {
    	  patientUid = "no-patient-uid";
        }
    QString patientName = patient()->data(Core::IPatient::FullName).toString();
    if (patientName.isEmpty())
    {
    	  patientName = "Patient Name";
        }
    QVariant insurance = "by";//todo
    QHash<int,QVariant> hash;
    hash.insert(ACCOUNT_UID,"UID");
    hash.insert(ACCOUNT_USER_UID,userUuid);
    hash.insert(ACCOUNT_PATIENT_UID,patientUid);
    hash.insert(ACCOUNT_PATIENT_NAME,patientName);
    hash.insert(ACCOUNT_SITE_ID,m_siteUid);//AccountDB::Constants::SITES_UID
    hash.insert(ACCOUNT_INSURANCE_ID,m_insuranceUid);
    hash.insert(ACCOUNT_DATE,ui->dateExecution->date()/*.toString("yyyy-MM-dd")*/);
    hash.insert(ACCOUNT_MEDICALPROCEDURE_XML, QVariant());
    hash.insert(ACCOUNT_MEDICALPROCEDURE_TEXT,textOfListOfActs);
    hash.insert(ACCOUNT_COMMENT, QVariant());
    hash.insert(ACCOUNT_CASHAMOUNT,cash);
    hash.insert(ACCOUNT_CHEQUEAMOUNT,cheque);
    hash.insert(ACCOUNT_VISAAMOUNT,visa);
    hash.insert(ACCOUNT_INSURANCEAMOUNT,banking);
    hash.insert(ACCOUNT_OTHERAMOUNT,other);
    hash.insert(ACCOUNT_DUEAMOUNT,due);
    hash.insert(ACCOUNT_DUEBY,insurance);
    hash.insert(ACCOUNT_ISVALID,0);
    hash.insert(ACCOUNT_TRACE, QVariant());
    receiptsEngine r;
    if (!r.insertIntoAccount(hash,userUuid)) {
        QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Error inserting into AccountModel!"),QMessageBox::Ok);
    }
}

void ReceiptViewer::saveAndQuit()
{
    save();
    close();
}

void ReceiptViewer::mousePressEvent(QMouseEvent *event){
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
    QString listOfValuesStr = m_listOfValues.join("+");
    receiptsEngine r;
    if(r.insertInThesaurus(listOfValuesStr,m_userUuid)){
        QMessageBox::information(0,trUtf8("Information"),trUtf8("Saved in thesaurus."),QMessageBox::Ok);
        }
    actionTreeView->fillActionTreeView();
}

void ReceiptViewer::clearAll(bool b)
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " in clearAll ";
    if (b==false) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Clear all is uncheckable." ;
    }
    m_listOfValues.clear();
    m_modelReturnedList->removeRows(0,m_modelReturnedList->rowCount(),QModelIndex());
    //clear accountmodel
    for (int i = 0; i < ::InternalAmount::AmountModel::RowCount; i += 1) {
        double value = 0.00;
        m_model->setData(m_model->index(i, ::InternalAmount::AmountModel::Col_Value), value, Qt::EditRole);
    }
}

QVariant ReceiptViewer::firstItemChoosenAsPreferential(QString & item)
{
    QVariant variantValue = QVariant("No item");
    receiptsManager manager;
    if (item == "Distance rules")
    {
    	  variantValue = manager.m_preferedDistanceValue;
        }
    if (manager.getHashOfSites().keys().contains(item))
    {
    	  variantValue = manager.m_preferedSite;
        }
    if (manager.getHashOfInsurance().keys().contains(item))
    {
    	  variantValue = manager.m_preferedInsurance;
        }
    return variantValue;
}

void ReceiptViewer::showControlReceipts(bool b){
    if (b)
    {
    	  m_control->show();
        }
}

void ReceiptViewer::resizeEvent(QResizeEvent *event){
    m_control->resizeControlReceipts(this);
}

void ReceiptViewer::controlReceiptsDestroyed(){
    qDebug() << __FILE__ << QString::number(__LINE__) << " in controlReceiptsDestroyed " ;
    ui->inputRadioButton->setChecked(true);
}
