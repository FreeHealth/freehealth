#include "movementsviewer.h"
#include "ui_movementsviewer.h"
	
movementsViewer::movementsViewer(QWidget * parent):QWidget(parent),ui(new Ui::MovementsViewerWidget){
    ui->setupUi(this);
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
}

movementsViewer::~movementsViewer(){}
