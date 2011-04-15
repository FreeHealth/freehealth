#include "findReceiptsValues.h"

using namespace AccountDB;
using namespace Constants;

findReceiptsValues::findReceiptsValues(QWidget * parent):QDialog(parent){
  ui = new Ui::findValueDialog;
  ui->setupUi(this);
  ui->nextButton->hide();
  model = new MedicalProcedureModel(parent);
  fillComboCategories();
  initialize();
  QString comboValue = ui->comboBoxCategories->currentText().trimmed();
  emit fillListViewValues(comboValue);
  connect(ui->comboBoxCategories,SIGNAL(activated(const QString&)),this,SLOT(fillListViewValues(const QString&)));
  connect(ui->tableViewOfValues,SIGNAL(pressed(const QModelIndex&)),this,SLOT(chooseValue(const QModelIndex&)));
  connect(ui->listChoosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(supprItemChoosen(QListWidgetItem *)));
  connect(ui->nextButton,SIGNAL(pressed()),this,SLOT(showNext()));
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
    QHash<QString,QString> hashCategories = m_xmlParser->readXmlFile()[0];
    choiceList = hashCategories.value("typesOfReceipts").split(",");
    int MPRows = model->rowCount(QModelIndex());
    for (int i = 0; i < MPRows; i += 1)
    {
        QString typeData = model->data(model->index(i,MP_TYPE)).toString();
        if(!choiceList.contains(typeData)){
           choiceList << typeData;
           }
    }
    ui->comboBoxCategories->setEditable(true);
    ui->comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBoxCategories->addItems(choiceList);
}

void findReceiptsValues::fillListViewValues(const QString & comboItem){
    QString filter = QString("%1 LIKE '%%2%'").arg("TYPE",comboItem);
    if (!((itemModel = new QStandardItemModel(this)) == NULL) )
    {
        itemModel->clear();
        }
    QVariant act = QVariant(trUtf8("Name"));
    QVariant value = QVariant(trUtf8("Value"));
    model->setFilter(filter);
    int count =   model->rowCountWithFilter(QModelIndex(),filter);
    for (int i = 0; i < count; i += 1)
    {
    	QString name = model->dataWithFilter(model->index(i,MP_NAME),Qt::DisplayRole,filter).toString();
    	QString value = model->dataWithFilter(model->index(i,MP_AMOUNT),Qt::DisplayRole,filter).toString();
    	qDebug() << __FILE__ << QString::number(__LINE__) << " names =" << name ;
    	QStandardItem *itemName = new QStandardItem(name);
    	QStandardItem *itemValue = new QStandardItem(value);
    	QList<QStandardItem*> list;
    	list << itemName << itemValue;
    	itemModel->appendRow(list);
        }
    model->setFilter("");
    if (!itemModel->setHeaderData(0,Qt::Horizontal,act,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
    	  }
    if (!itemModel->setHeaderData(1,Qt::Horizontal,value,Qt::EditRole)	)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
        } 
    ui->tableViewOfValues->setModel(itemModel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
    if (tableViewIsFull(ui->tableViewOfValues->model()))
    {
    	  enableShowNextTable();
        }
   
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

void findReceiptsValues::on_lineEditFilter_textChanged(const QString & text){
    if (!((itemModel = new QStandardItemModel(this)) == NULL) )
    {
        itemModel->clear();
        }
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString filterText = ""+text+"%";
    QString filter = QString("%1 LIKE '%2' AND %3 LIKE '%4'").arg("TYPE",comboChoice,"NAME",filterText);
    QVariant act = QVariant(trUtf8("Name"));
    QVariant value = QVariant(trUtf8("Value"));
    model->setFilter(filter);
    int count =   model->rowCountWithFilter(QModelIndex(),filter);
    for (int i = 0; i < count; i += 1)
    {
    	QString name = model->dataWithFilter(model->index(i,MP_NAME),Qt::DisplayRole,filter).toString();
    	qDebug() << __FILE__ << QString::number(__LINE__) << " names =" << name ;
    	QString value = model->dataWithFilter(model->index(i,MP_AMOUNT),Qt::DisplayRole,filter).toString();
    	QStandardItem *itemName = new QStandardItem(name);
    	QStandardItem *itemValue = new QStandardItem(value);
    	QList<QStandardItem*> list;
    	list << itemName << itemValue;
    	itemModel->appendRow(list);
        }
    model->setFilter("");
    if (!itemModel->setHeaderData(0,Qt::Horizontal,act,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
    	  }
    if (!itemModel->setHeaderData(1,Qt::Horizontal,value,Qt::EditRole)	)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
        } 
    ui->tableViewOfValues->setModel(itemModel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
    if (tableViewIsFull(ui->tableViewOfValues->model()))
    {
    	  enableShowNextTable();
        }
}

bool findReceiptsValues::tableViewIsFull(QAbstractItemModel * model){
    bool ret = false;
    int count = model->rowCount();
    if (count > 255)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "table view is full" ;
    	  ret = true;
        }
    return ret;
}

void findReceiptsValues::enableShowNextTable(){
    qDebug() << __FILE__ << QString::number(__LINE__) << " enableshownet "   ;
    ui->nextButton->show();
}

void findReceiptsValues::showNext(){
    QAbstractItemModel * abModel = ui->tableViewOfValues->model();
    int rows = abModel->rowCount();
    qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(rows) ;
    int numberOfLastRow = abModel->headerData(rows -1,Qt::Vertical,Qt::DisplayRole).toInt();
    QString lastData = abModel->data(abModel->index(numberOfLastRow -1,0 ),Qt::DisplayRole).toString();
    qDebug() << __FILE__ << QString::number(__LINE__) << " numberOfLastRow =" << QString::number(numberOfLastRow) ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " shownext data =" <<  lastData;
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString afterSqlFilter = QString("%1 LIKE '%2' AND %3 >= '%4'").arg("TYPE",comboChoice,"NAME",lastData);
    model->setFilter(afterSqlFilter);
    int count =   model->rowCountWithFilter(QModelIndex(),afterSqlFilter);
    for (int i = 0; i < count; i += 1)
    {
    	QString name = model->dataWithFilter(model->index(i,MP_NAME),Qt::DisplayRole,afterSqlFilter).toString();
    	qDebug() << __FILE__ << QString::number(__LINE__) << " names =" << name ;
    	QString value = model->dataWithFilter(model->index(i,MP_AMOUNT),Qt::DisplayRole,afterSqlFilter).toString();
    	QStandardItem *itemName = new QStandardItem(name);
    	QStandardItem *itemValue = new QStandardItem(value);
    	QList<QStandardItem*> list;
    	list << itemName << itemValue;
    	itemModel->appendRow(list);
        }
    model->setFilter("");
    QVariant act = QVariant(trUtf8("Name"));
    QVariant value = QVariant(trUtf8("Value"));
    if (!itemModel->setHeaderData(0,Qt::Horizontal,act,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
    	  }
    if (!itemModel->setHeaderData(1,Qt::Horizontal,value,Qt::EditRole)	)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
        } 
    ui->tableViewOfValues->setModel(itemModel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
}
