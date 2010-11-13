/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.                  *
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

#include "receipts.h"
#include "xmlcategoriesparser.h"

#include "ui_ReceiptsWidget.h"

#include <QMessageBox>
#include <QFile>
#include <QMouseEvent>
#include <QMenu>

//static inline  AccountDB::AccountBase * DBInstance(){return AccountDB::AccountBase::instance();}

ReceiptsGUI * ReceiptsGUI::d = NULL ;

//ReceiptsGUI *ReceiptsGUI::getInstance()
//{
//    if (!d) {
//        d = new ReceiptsGUI;
//        qDebug() << __FILE__ << QString::number(__LINE__) << " creating singleton ReceiptsGUI.";
//    } else {
//        qWarning() << __FILE__ << QString::number(__LINE__) << " singleton ReceiptsGUI already created";
//    }
//    return d;
//}

ReceiptsGUI::ReceiptsGUI(QWidget *parent) :
        QWidget(parent),
        m_rbm(0), m_recEng(0),
        m_receiptsIsOn(false)
{
    ui = new Ui::ReceiptsDialog;
    ui->setupUi(this);
    initialize();
}

ReceiptsGUI::~ReceiptsGUI()
{
    delete ui;
    if (m_rbm)
        delete m_rbm;
    m_rbm = 0;
    if (m_recEng)
        delete m_recEng;
    m_recEng = 0;
    
    m_hashPercents.clear();
    m_countMoreOrLess = m_hashPercents.count();
    m_name = "";
    m_firstname = "";
    m_uid = "";
    m_birthday = "";
}

void ReceiptsGUI::initialize()
{
    if(m_receiptsIsOn == true) return;
    //initialize pointers
    m_rbm = new receiptsManager;
    m_recEng = new receiptsEngine;
    m_receiptsValues = new findReceiptsValues(this);

    m_receiptsIsOn = true;
    ui->registerLabel->setWordWrap(true);
    ui->registerLabel->setText("");
    ui->cashRadioButton->setChecked(true);
    m_rightClic = new QAction(trUtf8("Clear all"),this);

    // name,firstname,uid,birthday
    ui->lineEditName->setText(m_name);
    ui->lineEditFirstname->setText(m_firstname);

    // hide widgets
    showFreeWidgetsEntry(ui->checkBoxFreeEntry->isChecked());
    showFreeWidgetsValue(ui->checkBoxFreeValue->isChecked());

    // shortcuts
    ui->saveButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->closeButton->setShortcut(QKeySequence("Ctrl+Q"));
    ui->plusButton->setShortcut(QKeySequence("Ctrl++"));
    ui->lessButton->setShortcut(QKeySequence("Ctrl+-"));

    // fill widgets with datas of database
    percentages();
    QStringList listDebtor;
    listDebtor << "NAME";
    QString strDebtor = "insurance";
    fillComboBoxes(ui->comboBoxDebtor,listDebtor,strDebtor);
    QStringList listWhere;
    listWhere << "NAME";
    QString tableWhere = "sites";
    fillComboBoxes(ui->comboBoxWhere,listWhere,tableWhere);
    QStringList listWho;
    listWho << "EMR_USER_UID";
    QString tableWho = "users";
    fillComboBoxes(ui->comboBoxWho,listWho,tableWho);
    QStringList listBankAccount;
    listBankAccount << "BD_LABEL";
    QString tableBankAccount = "bank_details";
    fillComboBoxes(ui->comboBoxBankAccount,listBankAccount,tableBankAccount);
    QStringList listRules;
    listRules << "NAME_OF_RULE";
    QString strRules = "rules";
    fillComboBoxes(ui->comboBoxRules,listRules,strRules);
    QStringList listDistanceRule;
    listDistanceRule << "NAME_DIST_RULE";
    QString strDistanceRule = "distance_rules";
    fillComboBoxes(ui->comboBoxDistance, listDistanceRule,strDistanceRule);

  //comboBoxes of thesaurus
    ui->comboBoxThesaurus->setEditable(true);
    ui->comboBoxThesaurus->setInsertPolicy(QComboBox::NoInsert);
    QStringList thesaurusList;
              thesaurusList << fillWithThesaurusList();
    QString strThesaurus;
    foreach(strThesaurus,thesaurusList){
        ui->comboBoxThesaurus->addItem(strThesaurus);
        }

    // progressBar for percentages
    int countPercentBar = m_hashPercents.count()-1;
    ui->percentBar->setRange(0,countPercentBar);
    ui->percentBar->setValue(countPercentBar);
    ui->percentLabel->setText(m_hashPercentType.values().last()+" : "+m_hashPercents.values().last());
    m_countMoreOrLess = countPercentBar;
      //default values--------------------------------
    m_site_id = ui->comboBoxWhere->currentText();
    m_insurance_id = ui->comboBoxDebtor->currentText();

    show();
    connect(ui->closeButton,SIGNAL(pressed()),      this,SLOT(close()));
    connect(ui->receiptsFindButton,SIGNAL(pressed()),this,SLOT(getReceiptsLists()));
    connect(ui->saveButton, SIGNAL(pressed()),      this,SLOT(save()));
    connect(ui->comboBoxThesaurus,SIGNAL(activated(const QString &)),this,SLOT(comboBoxThesaurus_changed(const QString &)));
    connect(ui->plusButton, SIGNAL(pressed()),      this,SLOT(plusFunction()));
    connect(ui->lessButton, SIGNAL(pressed()),      this,SLOT(lessFunction()));
    connect(ui->checkBoxFreeEntry,   SIGNAL(toggled(bool)),  this,SLOT(showFreeWidgetsEntry(bool)));
    connect(ui->checkBoxFreeValue,   SIGNAL(toggled(bool)),  this,SLOT(showFreeWidgetsValue(bool)));
    connect(m_rightClic,SIGNAL(triggered()),    this,SLOT(clearAll()));// right clic

}

void ReceiptsGUI::clearAll()
{
    ui->percentBar->setValue(m_hashPercents.count());
    ui->registerLabel->setText("");
    ui->cashRadioButton->setChecked(true);
}

void ReceiptsGUI::save()
{
  //QHash<QString,QString> m_hashValuesChoosenFromFindValues
  QHash<int,QString> hashParams = paramsSelected();
  m_recEng->insertIntoAccount(m_hashValuesChoosenFromFindValues,hashParams);
}

void ReceiptsGUI::plusFunction()
{
    QList<int> listOfKeys = m_hashPercents.keys();
    qDebug() << " in plus " << __FILE__ << QString::number(__LINE__) +" plus = "+QString::number(m_countMoreOrLess);
    QHash <int,QString> ::iterator iMoreOrLess = m_hashPercents.find(listOfKeys[m_countMoreOrLess]);
    QHash <int,QString> ::iterator iTypeMoreOrLess = m_hashPercentType.find(listOfKeys[m_countMoreOrLess]);
    if(m_countMoreOrLess == m_hashPercents.count() -1)
        return;
    else{qDebug() << " in plus " << __FILE__ << QString::number(__LINE__) ;
        iMoreOrLess++;
        iTypeMoreOrLess++;
        qDebug() << " in plus " << __FILE__ << QString::number(__LINE__) ;
        QString choice	= iTypeMoreOrLess.value();
        qDebug() << " in plus " << __FILE__ << QString::number(__LINE__) << choice;
        ui->percentLabel	->setText(choice);
        m_percentFactor	= iMoreOrLess.value().toDouble();
        ui->percentLabel	->setText(choice+" : "+QString::number(m_percentFactor));
        m_countMoreOrLess++;
        ui->percentBar	->setValue(m_countMoreOrLess);
    }
}

void ReceiptsGUI::lessFunction()
{
    QList<int> listOfKeys = m_hashPercents.keys();
    qDebug() << " in less " << __FILE__ << QString::number(__LINE__) +" less = "+QString::number(m_countMoreOrLess);
    QHash <int,QString> ::iterator iMoreOrLess = m_hashPercents.find(listOfKeys[m_countMoreOrLess]);
    QHash <int,QString> ::iterator iTypeMoreOrLess = m_hashPercentType.find(listOfKeys[m_countMoreOrLess]);
    //qDebug() << " in less " << __FILE__ << __LINE__ +" count = "+QString::number(m_hashPercents.count());
    if(m_countMoreOrLess == 0) {
        return;
    } else {
        qDebug() << " in less " << __FILE__ << QString::number(__LINE__) ;
        iMoreOrLess--;
        iTypeMoreOrLess--;
        qDebug() << " in less " << __FILE__ << QString::number(__LINE__) ;
        QString choice	= iTypeMoreOrLess.value();
        qDebug() << " in less " << __FILE__ << QString::number(__LINE__) << choice;
        ui->percentLabel	->setText(choice);
        m_percentFactor	= iMoreOrLess.value().toDouble();
        ui->percentLabel	->setText(choice+" : "+QString::number(m_percentFactor));
        m_countMoreOrLess--;
        ui->percentBar	->setValue(m_countMoreOrLess);
    }
}

void ReceiptsGUI::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::RightButton) {
        qDebug() << " clic droit " << __FILE__ << QString::number(__LINE__);
        m_menu = new QMenu;
        m_menu->addAction(m_rightClic);
        m_menu->exec(QCursor::pos());
        /** \todo MEMORY LEAK ? */
    }
}

void ReceiptsGUI::percentages()
{
    receiptsBaseManager rb;
    QList<QMultiHash<int,QString> > listMultiHash = rb.getPercentages();
    m_hashPercentType = listMultiHash[0];
    m_hashPercents    = listMultiHash[1];
}

void ReceiptsGUI::fillComboBoxes(QComboBox *comboBox, const QStringList &list, const QString &table)
{
    // Combo will change so it can't be const...
    QString values = list.join(",");
    comboBox ->setEditable(true);
    comboBox ->setInsertPolicy(QComboBox::NoInsert);
    receiptsManager rbm;
    QStringList listReceiptsManager;
    listReceiptsManager = rbm.getComboBoxesDatas(values,table);
    QString str;
    foreach(str,listReceiptsManager){
        comboBox->addItem(str);
    }
}

void ReceiptsGUI::showFreeWidgetsEntry(bool checkBoxchecked)
{
    if (checkBoxchecked) {
        ui->labelName->hide();
        ui->labelFirstname->hide();
        ui->lineEditName->hide();
        ui->lineEditFirstname->hide();
        ui->labelFreeName->show();
        
        ui->labelFreeName->setWordWrap(true);
        
        ui->labelFreeName->setText(trUtf8("Free entry"));
       
        ui->labelFreeName->show();
      
        ui->lineEditFreeName->show();
        
    } else {
        ui->labelFreeName->hide();
        
        ui->lineEditFreeName->hide();
        
        ui->labelName->show();
        ui->labelFirstname->show();
        ui->lineEditName->show();
        ui->lineEditFirstname->show();
    }
}

void ReceiptsGUI::showFreeWidgetsValue(bool checkBoxchecked){
            if(checkBoxchecked && !ui->checkBoxFreeEntry->isChecked()){
            ui->labelFreeValue->setText(trUtf8("Free value"));
            ui->lineEditFreeValue->show();
            }
        else if(checkBoxchecked && ui->checkBoxFreeEntry->isChecked()){
             ui->labelName->hide();
             ui->labelFirstname->hide();
             ui->lineEditName->hide();
             ui->lineEditFirstname->hide();
             ui->labelFreeName->show();
             ui->labelFreeValue->show();
             ui->labelFreeName->setWordWrap(true);
             ui->labelFreeValue->setWordWrap(true);
             ui->labelFreeName->setText(trUtf8("Free entry"));
             ui->labelFreeValue->setText(trUtf8("Free value"));
             ui->labelFreeName->show();
             ui->labelFreeValue->show();
             ui->lineEditFreeName->show();
             ui->lineEditFreeValue->show();
            }
       else if(!checkBoxchecked){
            ui->labelFreeValue->setText(trUtf8(""));
            ui->lineEditFreeValue->hide();
            }
       else{}
}

QStringList ReceiptsGUI::fillWithThesaurusList(){
    QStringList list;
    list << "thesaurusItem1" << "thesaurusItem2" ;// todo : fill with datas of table Thesaurus
    return list;
}

void ReceiptsGUI::comboBoxThesaurus_changed(const QString & comboItem){
    /*comboBoxChoice->clear();
    QString item = comboItem;
    QStringList choiceList = m_rbm->getChoiceFromCategories(item);
    comboBoxChoice->addItems(choiceList);*/
    if(comboItem == "thesaurus"){QMessageBox::information(0,"Info","item = "+comboItem,QMessageBox::Ok);}
    else{
        
        }
}

void ReceiptsGUI::getReceiptsLists(){
    ui->registerLabel->setText("");
    if(m_hashValuesChoosenFromFindValues.size()>0){
        m_hashValuesChoosenFromFindValues.clear();
        }
    if(m_receiptsValues->exec()== QDialog::Accepted){
        m_hashValuesChoosenFromFindValues = m_receiptsValues->getChoosenValues();
        m_receiptsValues->clear();
        
        writeOnRegisterLabel();
    }

}

void ReceiptsGUI::getPatientDatas(const QString & name,const QString & firstname,const QString & uid,const QString & birthday)
{
    m_name = name;
    m_firstname = firstname;
    m_uid = uid;
    m_birthday = birthday;
}

void ReceiptsGUI::writeOnRegisterLabel(){
    QString textOnLabel = trUtf8("The selected values are : \n");
    QHashIterator<QString,QString> it(m_hashValuesChoosenFromFindValues);
    while(it.hasNext()){
        it.next();
        QString dataChoosen = it.key()+" "+it.value()+ " euros ";
        textOnLabel += dataChoosen+"\n";
    }
    ui->registerLabel->setText(textOnLabel);
    textOnLabel = "";
}

QHash<int,QString> ReceiptsGUI::paramsSelected(){
  QString cash = "0",cheque = "0",visa = "0",insurance= "0",other= "0",due = "0";
  if(ui->cashRadioButton->isChecked()) cash = "1";
  if(ui->checkRadioButton->isChecked()) cheque = "1";
  if(ui->cardRadioButton->isChecked()) visa ="1";
  if(ui->bankRadioButton->isChecked()) insurance ="1";
  if(ui->dueRadioButton->isChecked()) due ="1";
 
  
  QHash<int,QString> hash;
              hash.insert(receiptsEngine::ACCOUNT_UID,m_account_uid); // ??????
              hash.insert(receiptsEngine::USER_UID,m_user_uid);
              hash.insert(receiptsEngine::PATIENT_UID,m_patient_uid);
              hash.insert(receiptsEngine::PATIENT_NAME,m_name+","+m_firstname);
              hash.insert(receiptsEngine::SITE_ID,m_site_id);
              hash.insert(receiptsEngine::INSURANCE_ID,m_insurance_id);
              hash.insert(receiptsEngine::DATE,dateTimeEdit->dateTime().toString("yyyy-MM-dd"));
              hash.insert(receiptsEngine::MP_XML,"");
              hash.insert(receiptsEngine::MP_TXT,"");
              hash.insert(receiptsEngine::COMMENT,"");
              hash.insert(receiptsEngine::CASH,cash);
              hash.insert(receiptsEngine::CHEQUE,cheque);
              hash.insert(receiptsEngine::VISA,visa);
              hash.insert(receiptsEngine::INSURANCE,insurance);
              hash.insert(receiptsEngine::OTHER,other);
              hash.insert(receiptsEngine::DUE,due);
              hash.insert(receiptsEngine::DUE_BY,"");
              hash.insert(receiptsEngine::ISVALID,"0");
              hash.insert(receiptsEngine::TRACE,"0");
  return hash;
}
