#include "movementsviewer.h"
#include "ui_movementsviewer.h"
	
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
    
}

movementsViewer::~movementsViewer(){}

void movementsViewer::recordMovement(){
    
}

void movementsViewer::deleteMovement(){
    
}

void movementsViewer::validMovement(){

}
