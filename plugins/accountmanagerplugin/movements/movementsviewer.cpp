#include "movementsviewer.h"
#include "ui_movementsviewer.h"
#include "movementsIO.h"
#include "movementsmanager.h"
/*
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
*/

#include <QMessageBox>
/*
using namespace Core;
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
using namespace ReceiptsConstants;
using namespace InternalAmount;
using namespace Constants;
*/
/*********************/
//todo bank system
/********************/

movementsViewer::movementsViewer(QWidget * parent):QWidget(parent),ui(new Ui::MovementsViewerWidget){
    ui->setupUi(this);
    //instanciate
    m_valid = 0; //bill not received
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->dateEdit->setDate(QDate::currentDate());
    fillMovementsComboBox();
    fillYearComboBox();
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordMovement()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteMovement()));
    connect(ui->validButton,SIGNAL(pressed()),this,SLOT(validMovement()));
    connect(ui->valAndRecButton,SIGNAL(pressed()),this,SLOT(validAndRecord()));
}

movementsViewer::~movementsViewer(){}

void movementsViewer::showMovements(){
    movementsIODb  mov(this) ;
    QString year = ui->yearComboBox->currentText();
    ui->tableView->setModel(mov.getModelMovements(year));
}

void movementsViewer::recordMovement(){
    movementsIODb  mov(this) ;
    movementsManager manager;
    QHash<int,QVariant>  hashValues;
    QString availableMovement = ui->movementsComboBox->currentText();
    int acMovId = mov.getAvailableMovementId(availableMovement);
    QString userUid;//todo
    int bankId = 0;//todo
    int type = 0;
    QString label = availableMovement;
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString dateValue = ui->dateEdit->date().toString("yyyy-MM-dd");
    double valueCalculated = 0.00;
    valueCalculated = manager.getCalculatedValue(ui->valueDoubleSpinBox->value(),ui->percentDoubleSpinBox->value());    
    QString comment;//no comment
    int validity = 0;
    QString trace;// ??
    int isValid = m_valid;
    QString details = ui->detailsEdit->text();
    hashValues = manager.getHashOfValues(acMovId ,
                                         userUid,
                                         bankId,
                                         type,
                                         label,
                                         date,
                                         dateValue ,
                                         valueCalculated ,
                                         comment,
                                         validity ,
                                         trace ,
                                         isValid ,
                                         details);
    
    if (!mov.insertIntoMovements(hashValues)) {
    	  QMessageBox::warning(0,trUtf8("Error"),trUtf8("Movement not inserted."),QMessageBox::Ok);
        }
    else{
          QMessageBox::information(0,trUtf8("Information"),trUtf8("Movement is inserted."),QMessageBox::Ok);
    }
                           
}

void movementsViewer::deleteMovement(){
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()){
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("You forgot to select a line."),QMessageBox::Ok);
    }
    int row = index.row(); 
    movementsIODb  mov(this) ;
    if (!mov.deleteMovement(row))
    {
    	QMessageBox::warning(0,trUtf8("Error"),trUtf8("Movement is not deleted."),QMessageBox::Ok);
        }
    else{
        QMessageBox::information(0,trUtf8("Information"),trUtf8("Movement is deleted."),QMessageBox::Ok);
    }
}

void movementsViewer::validMovement(){
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()){
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("You forgot to select a line."),QMessageBox::Ok);
    }
    int row = index.row(); 
    movementsIODb  mov(this) ;
    if (!mov.validMovement(row))
    {
    	QMessageBox::warning(0,trUtf8("Error"),trUtf8("Movement is not validated."),QMessageBox::Ok);
        }
    else{
        QMessageBox::information(0,trUtf8("Information"),trUtf8("Movement is validated."),QMessageBox::Ok);
    }
}

void movementsViewer::validAndRecord(){
    m_valid = 1;
    recordMovement();// 1 = invoice,bill received
    m_valid = 0;
}

void movementsViewer::fillMovementsComboBox(){
    movementsIODb mov(this);
    ui->movementsComboBox->setModel(mov.getMovementsComboBoxModel(this));
}

void movementsViewer::fillYearComboBox(){
    movementsIODb mov(this);
    QStringList listOfYears;
    listOfYears = mov.getYearComboBoxModel();
    if (listOfYears.size()== 0)
    {
    	  listOfYears << QString::number(QDate::currentDate().year());
        }
    ui->yearComboBox->addItems(listOfYears);
}
