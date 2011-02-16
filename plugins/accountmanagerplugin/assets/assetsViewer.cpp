#include "assetsViewer.h"
#include "assetsmanager.h"
#include "assetsIO.h"

#include "ui_assetsviewer.h"

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
using namespace Constants;
*/

assetsViewer::assetsViewer(QWidget * parent):QWidget(parent),ui(new Ui::AssetsViewerWidget){
    ui->setupUi(this);
    //instanciate
    fillModeComboBox();
    ui->dateEdit->setDate(QDate::currentDate());
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordAsset()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteAsset()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    
}

assetsViewer::~assetsViewer(){}

void assetsViewer::recordAsset(){
    QHash<int,QVariant>  hashValues;
    assetsManager manager;
    assetsIO as(this);
    QString userUid;
    int accountId = 0;
    QString label = ui->detailsEdit->text();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");
    double duration = ui->durationDoubleSpinBox->value();
    QString choiceModeComboBox = ui->modeComboBox->currentText();
    int mode = 0;
    mode = manager.getHashForModeComboBox().key(choiceModeComboBox);
    double value = ui->valueDoubleSpinBox->value();
    double taxActs = 0.00;
    double residualValue = manager.getResidualValue(date,value,mode,duration);
    QVariant result = QVariant();
    QVariant assetement = QVariant();
    QString comments = ui->commentsEdit->text();
    QVariant trace = QVariant();
    hashValues = manager.getHashOfValues(userUid,
                                         accountId,
                                         label,
                                         date,
                                         duration,
                                         mode,
                                         value,
                                         taxActs,
                                         residualValue,
                                         result,
                                         assetement,
                                         comments,
                                         trace);
    if (!as.insertIntoAssets(hashValues))
    {
    	  QMessageBox::warning(0,trUtf8("Error"),trUtf8("Asset not recorded."),QMessageBox::Ok);
        }
    else{
          QMessageBox::warning(0,trUtf8("Error"),trUtf8("Asset recorded."),QMessageBox::Ok);
    }
}

void assetsViewer::deleteAsset(){
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()){
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("You forgot to select a line."),QMessageBox::Ok);
    }
    int row = index.row(); 
    assetsIO  asset(this) ;
    if (!asset.deleteAsset(row))
    {
    	QMessageBox::warning(0,trUtf8("Error"),trUtf8("Asset is not deleted."),QMessageBox::Ok);
        }
    else{
        QMessageBox::information(0,trUtf8("Information"),trUtf8("Asset is deleted."),QMessageBox::Ok);
    }
}

void assetsViewer::fillModeComboBox(){
    assetsManager manager;
    QStringList list = manager.getHashForModeComboBox().values();
    ui->modeComboBox->addItems(list);
}

