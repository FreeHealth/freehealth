#include "movementsviewer.h"
#include "ui_movementsviewer.h"
#include "movementsIO.h"
#include "movementsmanager.h"

#include <QMessageBox>
	
movementsViewer::movementsViewer(QWidget * parent):QWidget(parent),ui(new Ui::MovementsViewerWidget){
    ui->setupUi(this);
    //instanciate
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->dateEdit->setDate(QDate::currentDate());
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordMovement()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteMovement()));
    connect(ui->validButton,SIGNAL(pressed()),this,SLOT(validMovement()));
    connect(ui->valAndRecButton,SIGNAL(pressed()),this,SLOT(validAndRecord()));
}

movementsViewer::~movementsViewer(){}

void movementsViewer::showMovements(){
    movementsIODb  mov(this) ;
    ui->tableView->setModel(mov.getModelMovements());
}

void movementsViewer::recordMovement(){
    movementsIODb  mov(this) ;
    movementsManager manager;
    QHash<int,QVariant>  hashValues;
    QString availableMovement = ui->movementsComboBox->currentText();
    int acMovId = mov.getAvailableMovementId(availableMovement);
    QString userUid;
    int bankId = 0;
    int type = 0;
    QString label;
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString dateValue = ui->dateEdit->date().toString("yyyy-MM-dd");
    double valueCalculated = 0.00;
    QString comment;
    int validity = 0;
    QString trace;
    int isValid = 0;
    QString details;
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

}

void movementsViewer::validAndRecord(){
    
}

void movementsViewer::fillMovementsComboBox(){
    movementsIODb mov(this);
    ui->movementsComboBox->setModel(mov.getMovementsComboBoxModel(this));
    
}
