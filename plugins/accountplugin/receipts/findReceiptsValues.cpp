#include "findReceiptsValues.h"

using namespace AccountDB;
using namespace Constants;

findReceiptsValues::findReceiptsValues(QWidget * parent):QDialog(parent){
  ui = new Ui::findValueDialog;
  ui->setupUi(this);
  fillComboCategories();
  initialize();
  QString comboValue = ui->comboBoxCategories->currentText().trimmed();
  emit fillListViewValues(comboValue);
  connect(ui->comboBoxCategories,SIGNAL(activated(const QString&)),this,SLOT(fillListViewValues(const QString&)));
  connect(ui->tableViewOfValues,SIGNAL(pressed(const QModelIndex&)),this,SLOT(chooseValue(const QModelIndex&)));
  connect(ui->listChoosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(supprItemChoosen(QListWidgetItem *)));
}

findReceiptsValues::~findReceiptsValues(){
  delete m_xmlParser;
  //delete m_rbm;
  //delete m_mpmodel;
  ui->listChoosenWidget->clear();
}

void findReceiptsValues::initialize(){
    m_xmlParser = new xmlCategoriesParser;
    //m_rbm = new receiptsManager;
    //m_mpmodel = new AccountDB::MedicalProcedureModel(this);
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
    MedicalProcedureModel * m_mpmodel = new MedicalProcedureModel(this);
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
    QString filter = QString("%1= '%2'").arg("TYPE",comboItem);
    if (!((itemModel = new QStandardItemModel(this)) == NULL) )
    {
        itemModel->clear();
        }
    for (int i = 0; i < itemModel->rowCount(); i += 1)
    {
    	qDebug() << __FILE__ << QString::number(__LINE__) << " data one =" << itemModel->data(itemModel->index(i,0),Qt::DisplayRole).toString();
        }

    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    QVariant act = QVariant(trUtf8("Name"));
    QVariant value = QVariant(trUtf8("Value"));
    MedicalProcedureModel * model = new MedicalProcedureModel(this);
    model->setFilter(filter);
    int count =   model->rowCountWithFilter(QModelIndex(),filter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filtre =" << model->filter() ;
    for (int i = 0; i < count; i += 1)
    {
    	QString name = model->data(model->index(i,MP_NAME),Qt::DisplayRole).toString();
    	QString value = model->data(model->index(i,MP_AMOUNT),Qt::DisplayRole).toString();
    	qDebug() << __FILE__ << QString::number(__LINE__) << " data MP =" << name;
    	QStandardItem *itemName = new QStandardItem(name);
    	QStandardItem *itemValue = new QStandardItem(value);
    	QList<QStandardItem*> list;
    	list << itemName << itemValue;
    	itemModel->appendRow(list);
        }
    delete model;
    if (!itemModel->setHeaderData(0,Qt::Horizontal,act,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
    	  }
    if (!itemModel->setHeaderData(1,Qt::Horizontal,value,Qt::EditRole)	)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
        } 
    for (int i = 0; i < itemModel->rowCount(); i += 1)
    {
    	qDebug() << __FILE__ << QString::number(__LINE__) << " data=" << itemModel->data(itemModel->index(i,0),Qt::DisplayRole).toString();
        }
    ui->tableViewOfValues->setModel(itemModel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tableViewOfValues->setColumnHidden(MP_ID,true);//setColumnHidden()
    //ui->tableViewOfValues->setColumnHidden(MP_UID,true);
    //ui->tableViewOfValues->setColumnHidden(MP_USER_UID,true);
    //ui->tableViewOfValues->setColumnHidden(MP_ABSTRACT,true);
    //ui->tableViewOfValues->setColumnHidden(MP_TYPE,true);
    //ui->tableViewOfValues->setColumnHidden(6,true); //amount
    //ui->tableViewOfValues->setColumnHidden(MP_REIMBOURSEMENT,true);
    //ui->tableViewOfValues->setColumnHidden(MP_DATE,true);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
    //ui->tableViewOfValues->show();
    //update();
   
}

void findReceiptsValues::chooseValue(const QModelIndex& index){
    QModelIndex inIndex(index);
    //get datas
    int row = inIndex.row();
    QAbstractItemModel * model = ui->tableViewOfValues->model();
    QModelIndex indexData = model->index(row,0,QModelIndex());
    QModelIndex indexAmount = model->index(row,1,QModelIndex());
    QString data = model->data(indexData,Qt::DisplayRole).toString();//NAME
    QString amount = model->data(indexAmount,Qt::DisplayRole).toString();//AMOUNT
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
