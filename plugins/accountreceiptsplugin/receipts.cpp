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
#include "connexion.h"

//static inline  AccountDB::AccountBase * DBInstance(){return AccountDB::AccountBase::instance();}

ReceiptsGUI * ReceiptsGUI::d = NULL ;

ReceiptsGUI *ReceiptsGUI::getInstance()
{
    if (!d) {
        d = new ReceiptsGUI;
        qDebug() << __FILE__ << QString::number(__LINE__) << " creating singleton ReceiptsGUI.";
    } else {
        qWarning() << __FILE__ << QString::number(__LINE__) << " singleton ReceiptsGUI already created";
    }
    return d;
}

ReceiptsGUI::ReceiptsGUI() :
        m_receiptsIsOn(false)
{
}

ReceiptsGUI::~ReceiptsGUI(){
    m_hashPercents.clear();
    m_countMoreOrLess = m_hashPercents.count();
    m_name = "";
    m_firstname = "";
    m_uid = "";
    m_birthday = "";
    delete m_rbm;
    delete m_recEng;
}

void ReceiptsGUI::initialize(){
    if(m_receiptsIsOn == true) return;
    //initialize pointers
    m_rbm = new receiptsBaseManager;
    m_recEng = new receiptsEngine;
    //----connexion and complete database
    if(!connexion()) {
        qWarning() <<  __FILE__ << QString::number(__LINE__) << " no connexion !" ;
    }
    if(QFile(qApp->applicationDirPath ()+"/account.db").size()<1){
        QString createTablesAndFields = m_rbm->createTablesAndFields();
        if(m_rbm->writeAllDefaultsValues()){
            qWarning() <<  __FILE__ << QString::number(__LINE__) << "write default values not failed";
        }

        qDebug() <<  __FILE__ << QString::number(__LINE__)
                << " createTablesAndFields= "<< createTablesAndFields;
        qDebug() << __FILE__ << QString::number(__LINE__) << "requetes = "+m_rbm->m_rbmReq;
    }
    //------------------------------------------------------------------
    m_receiptsIsOn = true;
    setupUi(this);
    qDebug() <<  __FILE__ << QString::number(__LINE__) ;
    registerLabel->setText("");
    cashRadioButton->setChecked(true);
    m_rightClic = new QAction(trUtf8("Clear all"),this);
    //name,firstname,uid,birthday
    lineEditName     ->setText(m_name);
    lineEditFirstname->setText(m_firstname);
    //hide widgets-------------------------------------------
    showFreeWidgets(checkBox->isChecked());
    //shortcuts----------------------------------------
    saveButton-> setShortcut(QKeySequence::InsertParagraphSeparator);
    closeButton-> setShortcut(QKeySequence("Ctrl+Q"));
    plusButton-> setShortcut(QKeySequence("Ctrl++"));
    lessButton-> setShortcut(QKeySequence("Ctrl+-"));
    //-------------------------------------------------------
    //fill widgets with datas of database
    percentages();
    QStringList listDebtor;
    listDebtor << "NAME";
    QString strDebtor = "insurance";
    fillComboBoxes(comboBoxDebtor,listDebtor,strDebtor);
    QStringList listWhere;
    listWhere << "NAME";
    QString tableWhere = "sites";
    fillComboBoxes(comboBoxWhere,listWhere,tableWhere);
    QStringList listWho;
    listWho << "EMR_USER_UID";
    QString tableWho = "users";
    fillComboBoxes(comboBoxWho,listWho,tableWho);
    QStringList listBankAccount;
    listBankAccount << "BD_LABEL";
    QString tableBankAccount = "bank_details";
    fillComboBoxes(comboBoxBankAccount,listBankAccount,tableBankAccount);
    QStringList listRules;
    listRules << "NAME_OF_RULE";
    QString strRules = "rules";
    fillComboBoxes(comboBoxRules,listRules,strRules);
    QStringList listDistanceRule;
    listDistanceRule << "NAME_DIST_RULE";
    QString strDistanceRule = "distance_rules";
    fillComboBoxes(comboBoxDistance, listDistanceRule,strDistanceRule);
    //-------------------------------------------------
    //comboBoxes of categories
    comboBoxCategories->setEditable(true);
    comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    QStringList categoriesList;
    categoriesList << "thesaurus"
            << fillWithCategoriesList();
    QString strCategories;
    foreach(strCategories,categoriesList){
        comboBoxCategories->addItem(strCategories);
    }
    //progressBar for percentages
    int countPercentBar = m_hashPercents.count()-1;
    percentBar->setRange(0,countPercentBar);
    percentBar->setValue(countPercentBar);
    percentLabel->setText(m_hashPercentType.values().last()+" : "+m_hashPercents.values().last());
    m_countMoreOrLess = countPercentBar;
    //----------------------------------------------
    show();
    connect(closeButton,SIGNAL(pressed()),      this,SLOT(close()));
    connect(saveButton, SIGNAL(pressed()),      this,SLOT(save()));
    connect(comboBoxCategories,SIGNAL(activated(const QString &)),this,SLOT(comboBoxCategories_changed(const QString &)));
    connect(plusButton, SIGNAL(pressed()),      this,SLOT(plusFunction()));
    connect(lessButton, SIGNAL(pressed()),      this,SLOT(lessFunction()));
    connect(checkBox,   SIGNAL(toggled(bool)),  this,SLOT(showFreeWidgets(bool)));
    connect(m_rightClic,SIGNAL(triggered()),    this,SLOT(clearAll()));// right clic

}

void ReceiptsGUI::clearAll(){
    percentBar->setValue(m_hashPercents.count());
    registerLabel->setText("");
    cashRadioButton->setChecked(true);
}

void ReceiptsGUI::save(){
    QMessageBox::information(0,"try","save",QMessageBox::Ok);
    /*QMultiHash<QString,QString> hash,
  m_recEng->insertInAccountDatabase(hash,);*/
}

void ReceiptsGUI::plusFunction(){
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
        percentLabel	->setText(choice);
        m_percentFactor	= iMoreOrLess.value().toDouble();
        percentLabel	->setText(choice+" : "+QString::number(m_percentFactor));
        m_countMoreOrLess++;
        percentBar	->setValue(m_countMoreOrLess);
    }
}

void ReceiptsGUI::lessFunction(){

    QList<int> listOfKeys = m_hashPercents.keys();
    qDebug() << " in less " << __FILE__ << QString::number(__LINE__) +" less = "+QString::number(m_countMoreOrLess);
    QHash <int,QString> ::iterator iMoreOrLess = m_hashPercents.find(listOfKeys[m_countMoreOrLess]);
    QHash <int,QString> ::iterator iTypeMoreOrLess = m_hashPercentType.find(listOfKeys[m_countMoreOrLess]);
    //qDebug() << " in less " << __FILE__ << __LINE__ +" count = "+QString::number(m_hashPercents.count());
    if(m_countMoreOrLess == 0)
        return;
    else{qDebug() << " in less " << __FILE__ << QString::number(__LINE__) ;
        iMoreOrLess--;
        iTypeMoreOrLess--;
        qDebug() << " in less " << __FILE__ << QString::number(__LINE__) ;
        QString choice	= iTypeMoreOrLess.value();
        qDebug() << " in less " << __FILE__ << QString::number(__LINE__) << choice;
        percentLabel	->setText(choice);
        m_percentFactor	= iMoreOrLess.value().toDouble();
        percentLabel	->setText(choice+" : "+QString::number(m_percentFactor));
        m_countMoreOrLess--;
        percentBar	->setValue(m_countMoreOrLess);
    }
}

void ReceiptsGUI::mousePressEvent(QMouseEvent * event){
    if(event->button() == Qt::RightButton){
        qDebug() << " clic droit " << __FILE__ << QString::number(__LINE__);
        m_menu         = new QMenu;
        m_menu        -> addAction(m_rightClic);
        m_menu        -> exec(QCursor::pos());
    }
}

void ReceiptsGUI::percentages(){
    receiptsBaseManager rb;
    QList<QMultiHash<int,QString> > listMultiHash = rb.getPercentages();
    m_hashPercentType = listMultiHash[0];
    m_hashPercents    = listMultiHash[1];
}

void ReceiptsGUI::fillComboBoxes(QComboBox *comboBox, const QStringList &list, const QString &table){
    // Combo will change so it can't be const...
    QString values = list.join(",");
    comboBox ->setEditable(true);
    comboBox ->setInsertPolicy(QComboBox::NoInsert);
    receiptsBaseManager rbm;
    QStringList listReceiptsBaseManager;
    listReceiptsBaseManager = rbm.getComboBoxesDatas(values,table);
    QString str;
    foreach(str,listReceiptsBaseManager){
        comboBox->addItem(str);
    }
}

void ReceiptsGUI::showFreeWidgets(bool checkBoxchecked){
    if(checkBoxchecked){
        labelName->hide();
        labelFirstname->hide();
        lineEditName->hide();
        lineEditFirstname->hide();
        labelFreeName->show();
        labelFreeValue->show();
        labelFreeName->setWordWrap(true);
        labelFreeValue->setWordWrap(true);
        labelFreeName->setText(trUtf8("Free entry"));
        labelFreeValue->setText(trUtf8("Free value"));
        labelFreeName->show();
        labelFreeValue->show();
        lineEditFreeName->show();
        lineEditFreeValue->show();
    }
    else{
        labelFreeName->hide();
        labelFreeValue->hide();
        lineEditFreeName->hide();
        lineEditFreeValue->hide();
        labelName->show();
        labelFirstname->show();
        lineEditName->show();
        lineEditFirstname->show();
    }
}

QStringList ReceiptsGUI::fillWithCategoriesList(){
    QList<QHash<QString,QString> > hashList;
    xmlCategoriesParser xml;
    hashList = xml.readXmlFile();  
    QStringList list;
    list = hashList[0].value("typesOfReceipts").trimmed().split(",");
    return list;
}

void ReceiptsGUI::comboBoxCategories_changed(const QString & comboItem){
    comboBoxChoice->clear();
    QString item = comboItem;
    QStringList choiceList = m_rbm->getChoiceFromCategories(item);
    comboBoxChoice->addItems(choiceList);
}

void ReceiptsGUI::getPatientDatas(const QString & name,const QString & firstname,const QString & uid,const QString & birthday){
    m_name = name;
    m_firstname = firstname;
    m_uid = uid;
    m_birthday = birthday;

}


