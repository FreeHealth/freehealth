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
}

void ReceiptsGUI::initialize()
{
    if(m_receiptsIsOn == true) return;
    //initialize pointers
    m_rbm = new receiptsBaseManager;
    m_recEng = new receiptsEngine;
    m_receiptsValues = new findReceiptsValues(this);
    // connexion and complete database
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

    m_receiptsIsOn = true;
    qDebug() <<  __FILE__ << QString::number(__LINE__) ;
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

    // comboBoxes of categories
    ui->comboBoxCategories->setEditable(true);
    ui->comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    QStringList categoriesList;
    categoriesList << "thesaurus"
            << fillWithCategoriesList();
    QString strCategories;
    foreach(strCategories,categoriesList){
        ui->comboBoxCategories->addItem(strCategories);
    }

    // progressBar for percentages
    int countPercentBar = m_hashPercents.count()-1;
    ui->percentBar->setRange(0,countPercentBar);
    ui->percentBar->setValue(countPercentBar);
    ui->percentLabel->setText(m_hashPercentType.values().last()+" : "+m_hashPercents.values().last());
    m_countMoreOrLess = countPercentBar;

    show();
    connect(ui->closeButton,SIGNAL(pressed()),      this,SLOT(close()));
    connect(ui->saveButton, SIGNAL(pressed()),      this,SLOT(save()));
    connect(ui->comboBoxCategories,SIGNAL(activated(const QString &)),this,SLOT(comboBoxCategories_changed(const QString &)));
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
    QMessageBox::information(0,"try","save",QMessageBox::Ok);
    /*QMultiHash<QString,QString> hash,
  m_recEng->insertInAccountDatabase(hash,);*/
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
    receiptsBaseManager rbm;
    QStringList listReceiptsBaseManager;
    listReceiptsBaseManager = rbm.getComboBoxesDatas(values,table);
    QString str;
    foreach(str,listReceiptsBaseManager){
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

QStringList ReceiptsGUI::fillWithCategoriesList()
{
    QList<QHash<QString,QString> > hashList;
    xmlCategoriesParser xml;
    hashList = xml.readXmlFile();  
    QStringList list;
    list = hashList[0].value("typesOfReceipts").trimmed().split(",");
    return list;
}

void ReceiptsGUI::comboBoxCategories_changed(const QString & comboItem){
    /*comboBoxChoice->clear();
    QString item = comboItem;
    QStringList choiceList = m_rbm->getChoiceFromCategories(item);
    comboBoxChoice->addItems(choiceList);*/
    if(comboItem == "thesaurus"){QMessageBox::information(0,"Info","item = "+comboItem,QMessageBox::Ok);}
    else{
        m_receiptsValues->exec();
        }
}

void ReceiptsGUI::getPatientDatas(const QString & name,const QString & firstname,const QString & uid,const QString & birthday)
{
    m_name = name;
    m_firstname = firstname;
    m_uid = uid;
    m_birthday = birthday;
}


