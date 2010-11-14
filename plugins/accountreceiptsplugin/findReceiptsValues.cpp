#include "findReceiptsValues.h"



findReceiptsValues::findReceiptsValues(QWidget * parent):QDialog(parent){
  //QSqlDatabase db = QSqlDatabase::database("freeaccount");
  ui = new Ui::findValueDialog;
  ui->setupUi(this);
  fillComboCategories();
  initialize();
  //m_model = new QSqlTableModel(this,db);
  m_mpmodel = new MedicalProcedureModel(this);
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
    ui->comboBoxCategories->setEditable(true);
    ui->comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBoxCategories->addItems(choiceList);
}

void findReceiptsValues::fillListViewValues(const QString & comboItem){
    QString filter = QString("TYPE = '%1'").arg(comboItem);
    //m_model->setTable("medical_procedure");
    m_mpmodel->setFilter(filter);
    //m_mpmodel->select();
    //qDebug() << __FILE__ << QString::number(__LINE__) << m_model->lastError().text();
    //ui->tableViewOfValues->setModel(m_mpmodel);
    ui->tableViewOfValues->setModel(m_mpmodel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->setColumnHidden(0,true);//setColumnHidden()
    ui->tableViewOfValues->setColumnHidden(1,true);
    ui->tableViewOfValues->setColumnHidden(2,true);
    ui->tableViewOfValues->setColumnHidden(4,true);
    ui->tableViewOfValues->setColumnHidden(5,true);
    //ui->tableViewOfValues->setColumnHidden(6,true); //amount
    ui->tableViewOfValues->setColumnHidden(7,true);
    ui->tableViewOfValues->setColumnHidden(8,true);
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
    /*QSqlRecord record = m_mpmodel->record(row);
    QString data = record.value(3).toString();
    QString amount = record.value(6).toString();*/
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
   // ui->listChoosenWidget->removeItemWidget(item);
}

QHash<QString,QString> findReceiptsValues::getChoosenValues(){
    return m_hashValuesChoosen;
}
