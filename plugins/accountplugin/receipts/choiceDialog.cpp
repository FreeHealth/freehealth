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
#include "choiceDialog.h"
#include "receiptsmanager.h"
#include "receiptsIO.h"
#include "ui_ChoiceDialog.h"
#include "constants.h"
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <QRadioButton>
#include <QMessageBox>
#include <QBrush>
#include <QColor>
#include <QMouseEvent>

enum { WarnDebugMessage = false };

using namespace ChoiceActions;

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }

treeViewsActions::treeViewsActions(QWidget *parent):QTreeView(parent){
    m_deleteThesaurusValue = new QAction(trUtf8("Delete this value."),this);
    m_choosePreferedValue = new QAction(trUtf8("Choose this value like the preferred."),this);
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
    Q_UNUSED(b);
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
    Q_UNUSED(b);
    QMessageBox msgBox;
            msgBox.setText("Do you want to choose this item as preferred value ?");
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
    if (WarnDebugMessage)
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
    parametersMap.insert(trUtf8("Debtor"),"insurance");
    //parametersMap.insert("Thesaurus","thesaurus");
    //parametersMap.insert("Values","values");
    parametersMap.insert(trUtf8("Sites"),"sites");
    //parametersMap.insert("Preferred Value","Preferred Value");
    //parametersMap.insert("Round trip","Round trip");
    parametersMap.insert(trUtf8("Distance rules"),"distance_rules");
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
        	  if (strKeysParameters.contains(trUtf8("Debtor")))
        	  {
        	       mapSubItems.insertMulti(strKeysParameters,"Patient");
                       mapSubItems.insertMulti(strKeysParameters,"CPAM28");  
        	      }
        	  /*else if (strKeysParameters == "Thesaurus")
        	  {
        	       mapSubItems.insertMulti("Thesaurus","CS");
                       mapSubItems.insertMulti("Thesaurus","V");  
        	      }*/
        	  else if (strKeysParameters.contains(trUtf8("Sites")))
        	  {
        	       mapSubItems.insertMulti("Sites","cabinet");
                       mapSubItems.insertMulti("Sites","clinique");  
        	      }
        	  else if (strKeysParameters.contains(trUtf8("Distance rules")))
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
        if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " strMainActions =" << strMainActions ;
        QStandardItem *actionItem = new QStandardItem(strMainActions);
        //treeViewsActions colors
        if (strMainActions.contains(trUtf8("Debtor")))
        {
        	  QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);
            }
        /*//preferential choices in the tree view.
    QString site = QString("Sites");
    QString distRule = QString("Distance rules");
    QString debtor = QString("Debtor");
    m_siteUidUid = firstItemChoosenAsPreferential(site);
    m_distanceRuleValue = firstItemChoosenAsPreferential(distRule).toDouble();
    m_distanceRuleType = rManager.m_preferedDistanceRule.toString();
    m_insuranceUid = firstItemChoosenAsPreferential(debtor);else if (strMainActions == "Preferred Value")
        {
        	  QBrush red(Qt::red);
                  actionItem->setForeground(red);
            }*/
        else if (strMainActions.contains(trUtf8("Sites")))
        {
        	  QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);       	  
            }
        /*else if (strMainActions == "Thesaurus")
        {
        	  QBrush red(Qt::red);
                  actionItem->setForeground(red);        	  
            }*/
        /*else if (strMainActions == "Values")
        {
        	  QBrush blue(Qt::blue);
                  actionItem->setForeground(blue);        	  
            }*/
        /*else if (strMainActions == "Round trip")
        {
        	  QBrush blue(Qt::blue);
                  actionItem->setForeground(blue);    
            }*/
        else if (strMainActions.contains(trUtf8("Distance rules")))
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
            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " strSubActions =" << strSubActions ;
            QStandardItem *subActionItem = new QStandardItem(strSubActions);
            actionItem->appendRow(subActionItem);            
        }
    }
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__)  ;
    setHeaderHidden(true);
    setStyleSheet("background-color: rgb(201, 201, 201)");
   // actionsTreeView->setStyleSheet("foreground-color: red");
    setModel(treeModel());
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__)  ;
}

bool treeViewsActions::deleteItemFromThesaurus(QModelIndex &index){
    bool ret = true;
    QString data = index.data().toString();
    receiptsEngine r;
    const QString userUid = user()->uuid();
    if (!r.deleteFromThesaurus(data,userUid))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Cannot delete in thesaurus :")+data,QMessageBox::Ok);
    	  ret = false;
        }
    fillActionTreeView();
    return ret;
}


using namespace ReceiptsConstants;
choiceDialog::choiceDialog(QWidget * parent,bool roundtrip):QDialog(parent),ui(new Ui::ChoiceDialog){
    ui->setupUi(this);
    ui->distanceDoubleSpinBox->hide();
    ui->distanceGroupBox->hide();
    m_percent = 100.00;
    m_percentValue = 100.00;
    receiptsManager manager;
    manager.getPreferedValues();
    m_hashPercentages = manager.getPercentages();
    m_quickInt = m_hashPercentages.keys().last();
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->percentDoubleSpinBox->setSingleStep(0.10);
    ui->percentDoubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    if (roundtrip)
    {
        ui->distanceDoubleSpinBox->show();
        ui->distanceGroupBox->show();
        ui->distanceDoubleSpinBox->setRange(0.00,100000.00);
        ui->distanceDoubleSpinBox->setSingleStep(0.10);
        }

    //treeViewsActions
    m_actionTreeView = new treeViewsActions(this);
    QVBoxLayout *vbox = new QVBoxLayout;
                 vbox-> addWidget(m_actionTreeView);
    ui->paramsGroupBox->setLayout(vbox);
    m_actionTreeView->fillActionTreeView();
    //preferential choices in the tree view.
    QString site = trUtf8("Sites");
    QString distRule = trUtf8("Distance rules");
    QString debtor = trUtf8("Debtor");
    m_siteUid = firstItemChoosenAsPreferential(site);
    m_distanceRuleValue = firstItemChoosenAsPreferential(distRule).toDouble();
    m_distanceRuleType = manager.getPreferedDistanceRule().toString();
    m_insurance = firstItemChoosenAsPreferential(debtor);
    m_insuranceUid = manager.m_preferedInsuranceUid;
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUid =" << m_insuranceUid.toString() ;
    m_modelChoicePercentDebtorSiteDistruleValues = new QStandardItemModel(0,returningModel_MaxParam);
    m_row = 0;
    m_timerUp = new QTimer(this);
    m_timerDown = new QTimer(this);
    connect(ui->okButton,SIGNAL(pressed()),this,SLOT(beforeAccepted()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(reject()));
    connect(ui->percentDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(value(double)));
    connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(valueUp()));
    connect(ui->plusButton,SIGNAL(released()),this,SLOT(valueUpStop()));
    connect(ui->lessButton,SIGNAL(pressed()),this,SLOT(valueDown()));
    connect(ui->lessButton,SIGNAL(released()),this,SLOT(valueDownStop()));
    connect(ui->plusConstButton,SIGNAL(pressed()),this,SLOT(quickPlus()));
    connect(ui->lessConstButton,SIGNAL(pressed()),this,SLOT(quickLess()));
    connect(m_actionTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(actionsOfTreeView(const QModelIndex&)));
}

choiceDialog::~choiceDialog(){
    delete m_timerUp;
    delete m_timerDown;
}

double choiceDialog::getDistanceNumber(const QString & data){
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data  ;
    receiptsEngine recIO;
    double dist = 0.00;
    double minDistance = recIO.getMinDistanceValue(data);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " minDistance =" << QString::number(minDistance) ;
    dist = ui->distanceDoubleSpinBox->value() - minDistance;
    if (dist < 0.00)
    {
    	  dist = 0.00;
        }
    return dist;
}

int choiceDialog::returnChoiceDialog(){
    int ret = 0;
        if (ui->cashButton->isChecked())
    {
    	ret = Cash;
        }
        if (ui->checkButton->isChecked())
    {
    	ret = Check;
        }
        if (ui->visaButton->isChecked())
    {
    	ret = Visa;
        }
        if (ui->bankingButton->isChecked())
    {
    	ret = Banking;
        }
        if (ui->otherButton->isChecked())
    {
    	ret = Other;
        }
        if (ui->dueButton->isChecked())
    {
    	ret = Due;
        }
    return ret;
}

void choiceDialog::value(double val){
    m_percentValue = val;
}

void choiceDialog::valueUp(){
    connect(m_timerUp,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepUp()));
    m_timerUp->start(10);
}

void choiceDialog::valueDown(){
    connect(m_timerDown,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepDown()));
    m_timerDown->start(10);
}


void choiceDialog::valueUpStop(){
    m_timerUp->stop();
}

void choiceDialog::valueDownStop(){
    m_timerDown->stop();
}


void choiceDialog::quickPlus(){
    if(m_quickInt == m_hashPercentages.keys().last())
        return;
    else{
        m_quickInt++;
        m_percent = m_hashPercentages.value(m_quickInt).toDouble();
 	}
    ui->percentDoubleSpinBox->setValue(m_percent);
}

void choiceDialog::quickLess(){
    if(m_quickInt == 1)
	    return;
  	/*else if(m_percent == 100){
            m_percent = list[0].toDouble();
 		}*/
    else{
        m_quickInt--;
        m_percent = m_hashPercentages.value(m_quickInt).toDouble();
 		}
    ui->percentDoubleSpinBox->setValue(m_percent);
}

double choiceDialog::returnPercentValue(){
    return m_percent;
}

QList<double> choiceDialog::listOfPercentValues(){
    return m_listOfPercentValues;
}

void choiceDialog::beforeAccepted(){
     receiptsEngine rIO;
     if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUid =" << QString::number(m_insuranceUid.toInt()) ;
     QString debtor = rIO.getStringFromInsuranceUid(m_insuranceUid);
     if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " debtor =" << debtor ;
     m_modelChoicePercentDebtorSiteDistruleValues->insertRows(m_row,1,QModelIndex());
     
     if (m_percent!=100.00)
     {
     	   QMessageBox msgBox;
           msgBox.setText(trUtf8("Choose another percentage value."));
           msgBox.setInformativeText(trUtf8("Do you want to choose another percentage ?"));
           msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
           msgBox.setDefaultButton(QMessageBox::Ok);
           int ret = msgBox.exec();
           
           switch(ret){
               case QMessageBox::Ok :
                   if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_row =" << QString::number(m_row) ;
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,TYPE_OF_CHOICE),returnChoiceDialog(),Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,PERCENTAGE),m_percent,Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DEBTOR),debtor,Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,SITE),m_siteUid,Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DISTRULES),m_distanceRuleType,Qt::EditRole);
                   ++m_row;
                   return;
                   break;
               case QMessageBox::No :
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,TYPE_OF_CHOICE),returnChoiceDialog(),Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,PERCENTAGE),m_percent,Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DEBTOR),debtor,Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,SITE),m_siteUid,Qt::EditRole);
                   m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DISTRULES),m_distanceRuleType,Qt::EditRole);
                   accept();
                   break;
               default :
                   break;    
               }
         }
     else
     {
        m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,TYPE_OF_CHOICE),returnChoiceDialog(),Qt::EditRole);
        m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,PERCENTAGE),m_percent,Qt::EditRole);
        m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DEBTOR),debtor,Qt::EditRole);
        m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,SITE),m_siteUid,Qt::EditRole);
        m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DISTRULES),m_distanceRuleType,Qt::EditRole);
     	accept();
         }
}

QStandardItemModel * choiceDialog::getChoicePercentageDebtorSiteDistruleModel(){
     return m_modelChoicePercentDebtorSiteDistruleValues;
}

QVariant choiceDialog::firstItemChoosenAsPreferential(QString & item)
{if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " item =" << item ;
    QVariant variantValue = QVariant("No item");
    receiptsManager manager;
    if (item == trUtf8("Distance rules"))
    {
    	  variantValue = manager.m_preferedDistanceValue;
        }
    if (item == trUtf8("Sites"))
    {
    	  variantValue = manager.m_preferedSite;
        }
    if (item== trUtf8("Debtor"))
    {
    	  variantValue = manager.m_preferedInsurance;
        }
    return variantValue;
}

void choiceDialog::actionsOfTreeView(const QModelIndex &index){
    QString data = index.data(Qt::DisplayRole).toString();
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data;
    receiptsManager manager;
    QHash<QString,QString> hashOfValues;
    //int typeOfPayment = ReceiptsConstants::Cash;
    //double percentage = 100.00;
    QVariant  debtor;
    QVariant site;
    QVariant distrules;

    if (manager.getDistanceRules().keys().contains(data))
    {
    	  m_distanceRuleValue = manager.getDistanceRules().value(data).toDouble();
    	  m_distanceRuleType = data;
    	  if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleValue =" << QString::number(m_distanceRuleValue) ;
    	  if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleType =" << m_distanceRuleType ;
        }
    if (manager.getHashOfSites().keys().contains(data))
    {
    	  m_siteUid = manager.getHashOfSites().value(data);
    	  if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_siteUid =" << m_siteUid.toString() ;
        }
    if (manager.getHashOfInsurance().keys().contains(data))
    {
    	  m_insuranceUid = manager.getHashOfInsurance().value(data);
    	  if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUid =" << m_insuranceUid.toString() ;
        }
        //actionTreeView->reset();
}


