#include "movementsviewer.h"
#include "movementsIO.h"
#include "movementsmanager.h"

#include <accountbaseplugin/movementmodel.h>
#include <accountbaseplugin/constants.h>

//#include <coreplugin/icore.h>
//#include <coreplugin/iuser.h>
//#include <coreplugin/ipatient.h>

#include "ui_movementsviewer.h"

#include <QMessageBox>
#include <QDebug>

//using namespace Core;
//static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
//static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
//using namespace ReceiptsConstants;
//using namespace InternalAmount;
//using namespace Constants;

/*********************/
//todo bank system
/********************/

using namespace AccountDB;
using namespace Constants;

MovementsViewer::MovementsViewer(QWidget * parent) :
        QWidget(parent),ui(new Ui::MovementsViewerWidget)
{
    ui->setupUi(this);
    //instanciate
    m_valid = 0; //bill not received
    ui->valueDoubleSpinBox->setRange(0.00,999999999999999.00);
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->dateEdit->setDate(QDate::currentDate());
    fillMovementsComboBox();
    fillYearComboBox();
    fillBankComboBox();
    ui->valAndRecButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    showMovements();
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordMovement()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteMovement()));
    connect(ui->validButton,SIGNAL(pressed()),this,SLOT(validMovement()));
    connect(ui->valAndRecButton,SIGNAL(pressed()),this,SLOT(validAndRecord()));
    connect(ui->movementsComboBox,SIGNAL(highlighted(int)),this,
                                  SLOT(setMovementsComboBoxToolTips(int)));
    
}

MovementsViewer::~MovementsViewer()
{
}

void MovementsViewer::showMovements()
{
    MovementsIODb  mov(this) ;
    QString year = ui->yearComboBox->currentText();
    MovementModel * model = mov.getModelMovements(year);
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setStretchLastSection ( true );
    ui->tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
    ui->tableView->setColumnHidden(MOV_ID,true);
    ui->tableView->setColumnHidden(MOV_AV_MOVEMENT_ID,true);
    ui->tableView->setColumnHidden(MOV_USER_UID,true);
    ui->tableView->setColumnHidden(MOV_ACCOUNT_ID,true);
    ui->tableView->setColumnHidden(MOV_TYPE,true);
    ui->tableView->setColumnHidden(MOV_TRACE,true);
    ui->tableView->setColumnHidden(MOV_COMMENT,true);
}

void MovementsViewer::recordMovement()
{
    MovementsIODb  mov(this) ;
    MovementsManager manager;
    QHash<int,QVariant>  hashValues;
    QString availableMovement = ui->movementsComboBox->currentText();
    int acMovId = mov.getAvailableMovementId(availableMovement);
    QString userUid = mov.getUserUid();
    QString bank = ui->bankComboBox->currentText();
    int bankId = mov.getBankId(bank);//todo
    int type = mov.getTypeOfMovement(availableMovement);
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
    } else {
        QMessageBox::information(0,trUtf8("Information"),trUtf8("Movement is inserted."),QMessageBox::Ok);
    }
    showMovements();
}

void MovementsViewer::deleteMovement()
{
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()) {
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("You forgot to select a line."),QMessageBox::Ok);
    }
    int row = index.row(); 
    qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
    MovementsIODb  mov(this) ;
    if (!mov.deleteMovement(row)) {
    	QMessageBox::warning(0,trUtf8("Error"),trUtf8("Movement is not deleted."),QMessageBox::Ok);
    }  else {
        QMessageBox::information(0,trUtf8("Information"),trUtf8("Movement is deleted."),QMessageBox::Ok);
    }
    showMovements();
}

void MovementsViewer::validMovement()
{
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("You forgot to select a line."),QMessageBox::Ok);
    }
    int row = index.row(); 
    MovementsIODb  mov(this) ;
    if (!mov.validMovement(row)) {
    	QMessageBox::warning(0,trUtf8("Error"),trUtf8("Movement is not validated."),QMessageBox::Ok);
    } else {
        QMessageBox::information(0,trUtf8("Information"),trUtf8("Movement is validated."),QMessageBox::Ok);
    }
    showMovements();
}

void MovementsViewer::validAndRecord()
{
    m_valid = 1;
    recordMovement();// 1 = invoice,bill received
    m_valid = 0;
}

void MovementsViewer::fillMovementsComboBox()
{
    MovementsIODb mov(this);
    ui->movementsComboBox->setModel(mov.getMovementsComboBoxModel(this));
}

void MovementsViewer::setMovementsComboBoxToolTips(int row){
    QHash<QString,QString> hashChildrenAndParents;
    MovementsIODb mov(this);
    hashChildrenAndParents = mov.hashChildrenAndParentsAvailableMovements();
    QString text = ui->movementsComboBox->itemText(row);
    QString parent = hashChildrenAndParents.value(text);
    QString toolTipText = QString("Parent = %1").arg(parent);
    QAbstractItemModel *m = new QStandardItemModel;
    m = ui->movementsComboBox->model();
    QStandardItemModel *model = qobject_cast<QStandardItemModel*> (m);
    QStandardItem *i = model->item(row);
    i->setToolTip(toolTipText);
}

void MovementsViewer::fillYearComboBox()
{
    MovementsIODb mov(this);
    QStringList listOfYears;
    listOfYears = mov.getYearComboBoxModel();
    if (listOfYears.isEmpty()) {
        listOfYears << QString::number(QDate::currentDate().year());
    }
    ui->yearComboBox->addItems(listOfYears);
}

void MovementsViewer::fillBankComboBox(){
    MovementsIODb mov(this);
    ui->bankComboBox->setModel(mov.getBankComboBoxModel(this));
}
