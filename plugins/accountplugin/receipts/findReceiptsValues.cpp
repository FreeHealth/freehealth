#include "findReceiptsValues.h"

using namespace AccountDB;
using namespace Constants;

findReceiptsValues::findReceiptsValues(QWidget * parent):QDialog(parent){
  m_mpmodel = new MedicalProcedureModel(this);
  ui = new Ui::findValueDialog;
  ui->setupUi(this);
  fillComboCategories();
  initialize();
  QString comboValue = ui->comboBoxCategories->currentText();
  emit fillListViewValues(comboValue);
  connect(ui->comboBoxCategories,SIGNAL(activated(const QString&)),this,SLOT(fillListViewValues(const QString&)));
  connect(ui->tableViewOfValues,SIGNAL(pressed(const QModelIndex&)),this,SLOT(chooseValue(const QModelIndex&)));
  connect(ui->listChoosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(supprItemChoosen(QListWidgetItem *)));
}

findReceiptsValues::~findReceiptsValues(){
  delete m_xmlParser;
  delete m_rbm;
  delete m_mpmodel;
  ui->listChoosenWidget->clear();
}

void findReceiptsValues::initialize(){
    m_xmlParser = new xmlCategoriesParser;
    m_rbm = new receiptsManager;
    m_mpmodel = new AccountDB::MedicalProcedureModel(this);
    if(m_hashValuesChoosen.size()>0){
        m_hashValuesChoosen.clear();
        }
        
}

void findReceiptsValues::clear(){
    ui->listChoosenWidget->clear();
    m_hashValuesChoosen.clear();
}

void findReceiptsValues::fillComboCategories(){
    QStringList choiceList ;
    QHash<QString,QString> hashCategories = m_xmlParser->readXmlFile()[0];
    choiceList = hashCategories.value("typesOfReceipts").split(",");
    int MPRows = m_mpmodel->rowCount(QModelIndex());
    for (int i = 0; i < MPRows; i += 1)
    {
        QString typeData = m_mpmodel->data(m_mpmodel->index(i,MP_TYPE)).toString();
        if(!choiceList.contains(typeData)){
           choiceList << typeData;
           }
    }
    ui->comboBoxCategories->setEditable(true);
    ui->comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBoxCategories->addItems(choiceList);
}

void findReceiptsValues::fillListViewValues(const QString & comboItem){
    QString filter = comboItem;
    m_mpmodel->setTypeFilter(filter);
    QVariant act = QVariant(trUtf8("Name"));
    QVariant value = QVariant(trUtf8("Value"));
    if (!m_mpmodel->setHeaderData(MP_NAME,Qt::Horizontal,act,Qt::DisplayRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << m_mpmodel->lastError().text();
    	  }
    if (!m_mpmodel->setHeaderData(MP_AMOUNT,Qt::Horizontal,value,Qt::DisplayRole)	)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << m_mpmodel->lastError().text();
        }
    
    ui->tableViewOfValues->setModel(m_mpmodel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->setColumnHidden(MP_ID,true);//setColumnHidden()
    ui->tableViewOfValues->setColumnHidden(MP_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_USER_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_ABSTRACT,true);
    ui->tableViewOfValues->setColumnHidden(MP_TYPE,true);
    //ui->tableViewOfValues->setColumnHidden(6,true); //amount
    ui->tableViewOfValues->setColumnHidden(MP_REIMBOURSEMENT,true);
    ui->tableViewOfValues->setColumnHidden(MP_DATE,true);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
    ui->tableViewOfValues->show();
    
}

void findReceiptsValues::chooseValue(const QModelIndex& index){
    QModelIndex inIndex(index);
    //get datas
    int row = inIndex.row();
    QModelIndex indexData = m_mpmodel->index(row,3,QModelIndex());
    QModelIndex indexAmount = m_mpmodel->index(row,6,QModelIndex());
    QString data = m_mpmodel->data(indexData,Qt::DisplayRole).toString();//NAME
    QString amount = m_mpmodel->data(indexAmount,Qt::DisplayRole).toString();//AMOUNT
    qDebug() << __FILE__ << QString::number(__LINE__) << " data = " << data;
    ui->listChoosenWidget->addItem(data);
    m_hashValuesChoosen.insert(data,amount);
}

void findReceiptsValues::supprItemChoosen(QListWidgetItem * item){
    qDebug() << __FILE__ << QString::number(__LINE__) << " item = " << item->text();
    QString dataToRemove = item->data(Qt::DisplayRole).toString();
    m_hashValuesChoosen.remove(dataToRemove);
    delete item;
}

QHash<QString,QString> findReceiptsValues::getChoosenValues(){
    return m_hashValuesChoosen;
}
