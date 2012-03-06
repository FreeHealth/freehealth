#include "preventIO.h"
#include "preventIHM.h"

#include <QAbstractItemModel>

static const QString PREVENT = QString("prevention");

//BEGIN OF TREEITEM = ITEMS FOR THE TREEVIEW

  TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
 {
     m_parentItem = parent;
     m_itemData = data;
 }

 TreeItem::~TreeItem()
 {
     qDeleteAll(m_childItems);
     qDeleteAll(m_treeItemsHash);
 }
 
  void TreeItem::appendChild(TreeItem *item)
 {
     m_childItems.append(item);
 }
 
 void TreeItem::fillParentsHash(const QString & parentName, TreeItem * item){
     m_treeItemsHash.insert(parentName,item);
 }
 

 TreeItem *TreeItem::child(int number)
 {
     return m_childItems.value(number);
 }
 
 TreeItem *TreeItem::child(const QString & parentName)
 {
     QString p = parentName.trimmed();
     return m_treeItemsHash.value(p);
 }
 
 int TreeItem::childCount() const
 {
     return m_childItems.count();
 }

 int TreeItem::childNumber() const
 {
     if (m_parentItem)
         return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
     return 0;
 }

 int TreeItem::columnCount() const
 {
     return m_itemData.count();
 }

 QVariant TreeItem::data(int column) const
 {
     QVariant data = m_itemData.value(column);
     return data;
 }
 
 QVector<QVariant> TreeItem::datas(int columns){
     QVector<QVariant> datas;
     for (int i = 0; i < columns; i += 1)
     {
     	datas << data(i);
         }
     return datas;
 }

 bool TreeItem::insertChildren(int position, int count, int columns, bool empty)
 {
     if (position < 0 || position > m_childItems.size())
         return false;
     if (!empty)
     {
     	 for (int row = 0; row < count; ++row) {
             QVector<QVariant> datasVector = datas(columns);
             qDebug() << __FILE__ << QString::number(__LINE__) << " dataVector =" 
             << datasVector[VariantItemModel::ITEM_H].toString() ;
             TreeItem *item = new TreeItem(datasVector, this);
             m_childItems.insert(position, item);
             m_treeItemsHash.insert(datasVector[VariantItemModel::ITEM].toString(),item);
         }
         }

     if (empty)
     {
     	  qDebug() << __FILE__ << QString::number(__LINE__) << " in empty "  ;
     	  if (!insertANewLineInModel())
     	  {
     	  	  qWarning() << __FILE__ << QString::number(__LINE__) 
     	  	  << "unable to insert a line in the model." ;
     	      }
     	  else
     	  {
     	  	for (int row = 0; row < count; ++row) {
             QVector<QVariant> datasVector = datas(columns);
             qDebug() << __FILE__ << QString::number(__LINE__) << " dataVector =" 
             << datasVector[VariantItemModel::ITEM_H].toString() ;
             TreeItem *item = new TreeItem(datasVector, this);
             m_childItems.insert(position, item);
             m_treeItemsHash.insert(datasVector[VariantItemModel::ITEM].toString(),item);
         }
     	      }
     	   /*else
     	   {
     	   	QVector<QVariant> datasVector;
     	   	QSqlDatabase db = QSqlDatabase::database("prevention");
     	   	QSqlTableModel model(this,db);
     	   	model.select();
     	   	int row = model.rowCount() -1;
     	   	qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
     	   	qDebug() << __FILE__ << QString::number(__LINE__) << " COL =" << QString::number(model.columnCount()) ;
     	   	for (int col = 1; col < model.columnCount()-1; col += 1)
     	   	{
     	   		  QVariant v = model.data(model.index(row,col),Qt::DisplayRole);
     	   		  datasVector << v;
     	   		  qDebug() << __FILE__ << QString::number(__LINE__) << " v =" << v.toString() ;
     	   	    }
     	   	 TreeItem *item = new TreeItem(datasVector, this);
                 m_childItems.insert(position, item);
                 m_treeItemsHash.insert(datasVector[VariantItemModel::ITEM].toString(),item);
                 
     	       }*/
     	   
         }

     return true;
 }

 bool TreeItem::insertColumns(int position, int columns)
 {
     if (position < 0 || position > m_itemData.size())
         return false;

     for (int column = 0; column < columns; ++column)
         m_itemData.insert(position, QVariant());

     foreach (TreeItem *child, m_childItems)
         child->insertColumns(position, columns);

     return true;
 }

 TreeItem *TreeItem::parent()
 {
     return m_parentItem;
 }

 bool TreeItem::removeChildren(int position, int count)
 {
     if (position < 0 || position + count > m_childItems.size())
         return false;

     for (int row = 0; row < count; ++row)
         delete m_childItems.takeAt(position);

     return true;
 }

 bool TreeItem::removeColumns(int position, int columns)
 {
     if (position < 0 || position + columns > m_itemData.size())
         return false;

     for (int column = 0; column < columns; ++column)
         m_itemData.remove(position);

     foreach (TreeItem *child, m_childItems)
         child->removeColumns(position, columns);

     return true;
 }

 bool TreeItem::setData(int column, const QVariant &value)
 {
     if (column < 0 || column >= m_itemData.size())
         return false;

     m_itemData[column] = value;
     return true;
 }
 
 bool TreeItem::insertANewLineInModel()
 {
     QString parentName = parent()->data(VariantItemModel::ITEM_H).toString();
     if (parentName.contains(trUtf8("Item")))
     {
     	  parentName = data(VariantItemModel::ITEM_H).toString();
         }
     QSqlDatabase db = QSqlDatabase::database("prevention");
     QString newIdPrimkey = QString::number(getNextIdPrimkey());
     QString idOfItem = QString::number(findNextId());
     QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");
     QString child = QString::number(VariantItemModel::CHILD);
     QSqlQuery q(db);
     const QString req = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(
     "prevention",
     "ID_Primkey,ITEM,TYPE,PARENT,PARENT_OR_CHILD,ICON,DATE_DONE,DATE_NEXT,ABSTRACT,ID_ITEM,RESULT ",
     QString("'"+newIdPrimkey+"',"//ID_Primkey
     "'""',"//ITEM
     "'0',"//TYPE
     "'"+parentName+"',"//PARENT
     "'"+child+"',"//PARENT_OR_CHILD
     "'"+qApp->applicationDirPath()+"/preventWarning.png',"//ICON 
     "'"+currentDate+"',"//DATE_DONE
     "'"+currentDate+"',"//DATE_NEXT
     "'"+QString("")+"',"//ABSTRACT
     "'"+idOfItem+"',"//ID_ITEM
     "'"+QString("")+"'"//RESULT
     )
     );
     if (!q.exec(req))
     {
     	  qDebug() << __FILE__ << QString::number(__LINE__) << q.lastQuery()  ;
     	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
     	  return false;
         }
     return true;    
 }
 
  int TreeItem::getNextIdPrimkey()
 {
      QSqlDatabase db = QSqlDatabase::database("prevention");
      QStringList listOfId;
      QSqlQuery q(db);
      QString req = QString("SELECT %1 FROM %2").arg("ID_Primkey","prevention");
     	  if (!q.exec(req))
     	  {
     	  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
     	      }
     	  while (q.next())
     	  {
     	  	QString id = q.value(0).toString();
     	  	qDebug() << __FILE__ << QString::number(__LINE__) << " id =" <<  id;
     	  	listOfId << id;
     	      }
     	   //listOfId.sort();
     	   qDebug() << __FILE__ << QString::number(__LINE__) << " listOfId last =" << listOfId.last() ;
     	   int idPrev = listOfId.last().toInt() +1 ;
     	   
      return idPrev;     
 }
 
  int TreeItem::findNextId()
 {
      QSqlDatabase db = QSqlDatabase::database("prevention");
      QStringList listOfId;
      QSqlQuery q(db);
      QString req = QString("SELECT %1 FROM %2").arg("ID_ITEM","prevention");
     	  if (!q.exec(req))
     	  {
     	  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
     	      }
     	  while (q.next())
     	  {
     	  	QString id = q.value(0).toString();
     	  	qDebug() << __FILE__ << QString::number(__LINE__) << " id =" <<  id;
     	  	listOfId << id;
     	      }
     	   //listOfId.sort();
     	   qDebug() << __FILE__ << QString::number(__LINE__) 
     	   << " listOfId last =" << listOfId.last() ;
     	   int idOfItem = listOfId.last().toInt() +1;
     	   
      return idOfItem;
 }
 
//end of TREEITEM
//VARIANTITEMMODEL UNDER THE TREEVIEW
VariantItemModel::VariantItemModel(const QStringList headers, QSqlTableModel * model,
                      QObject *parent)
     : QAbstractItemModel(parent)
 {
     QVector<QVariant> rootData;
     for (int i = 0; i < headers.size(); i += 1)
     {
     	  rootData << headers[i];
         }
        
     m_rootItem = new TreeItem(rootData);
     setupModelData(model, m_rootItem);
     m_modelSql = new QSqlTableModel;
     m_modelSql = model;
     m_modelSql->select();
  }

 VariantItemModel::~VariantItemModel()
 {
     delete m_rootItem;
 }
 
 void VariantItemModel::setRootItem (const QStringList headers)
 {
     QVector<QVariant> rootData;
     for (int i = 0; i < headers.size(); i += 1)
     {
     	  rootData << headers[i];
         }
        
     m_rootItem = new TreeItem(rootData);
 }
 
 bool VariantItemModel::clear()
 {
     bool result = true;
     //for (int it = 0; it < m_rootItem->childCount(); it += 1)
     //{
     	  qDebug() << __FILE__ << QString::number(__LINE__)  ;
     	  removeRows(1,m_rootItem->childCount(),QModelIndex());
     	  qDebug() << __FILE__ << QString::number(__LINE__) << "m_rootItem->childCount()= " << QString::number(m_rootItem->childCount()) ;
     	  //delete m_rootItem;
     	  qDebug() << __FILE__ << QString::number(__LINE__)  ;
         //}
     return result;
 }
 

 int VariantItemModel::columnCount(const QModelIndex & /* parent */) const
 {
     return m_rootItem->columnCount();
 }

 QVariant VariantItemModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid()){
         return QVariant();
         }
     if (role != Qt::DisplayRole  && index.column()!= ICON_H){
         return QVariant();
         }
     TreeItem *item = getItem(index);
     QVariant itemData = item->data(index.column());
     if (index.column() == ICON_H)
     {
     	  if(role == Qt::DecorationRole)
     	     return QIcon(itemData.toString());
     	  return QVariant();
         }

     return itemData;
 }
 
 bool VariantItemModel::setData(const QModelIndex &index, const QVariant &value,
                         int role)
 {
     if (role != Qt::EditRole && (role == Qt::DecorationRole && index.column() != ICON_H ) )
         return false;
     bool result = true;
     TreeItem *item = getItem(index);
     int idOfItem = 0;
     idOfItem = item->data(ID_ITEM_H).toInt();
     qDebug() << __FILE__ << QString::number(__LINE__) << " idOfItem =" << QString::number(idOfItem) ;
     qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value.toString() ;
     result = item->setData(index.column(), value);
     QModelIndex indexSql;
     int rowOfSqlTable = getSqlTableRow(idOfItem);
     qDebug() << __FILE__ << QString::number(__LINE__) << " rowOfSqlTable =" << QString::number(rowOfSqlTable) ;
     if (idOfItem == 0)
     {
     	  return false;
         }
     qDebug() << __FILE__ << QString::number(__LINE__)  ;
     if (result){
         emit dataChanged(index, index);
         qDebug() << __FILE__ << QString::number(__LINE__)  ;
         result = m_modelSql->setData(m_modelSql->index(rowOfSqlTable,index.column()+1),value,Qt::EditRole);
         qDebug() << __FILE__ << QString::number(__LINE__)  ;
         result = m_modelSql->submitAll();
         }
         
     qDebug() << __FILE__ << QString::number(__LINE__)  ;
     return result;
 }
 
 int VariantItemModel::getSqlTableRow(int idOfItem){
     int tableRow = 0;
     int row = 0; 
     QSqlDatabase db = QSqlDatabase::database("prevention");
     QSqlQuery q(db);
     QString req = QString("SELECT %1 FROM %2").arg("ID_ITEM","prevention");
     if (!q.exec(req))
     {
     	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
         }
     while (q.next())
     {
     	int id = q.value(0).toInt();
     	qDebug() << __FILE__ << QString::number(__LINE__) << " id =" << QString::number(id) ;
     	if (id == idOfItem)
     	{
     		  tableRow = row ;
     		  qDebug() << __FILE__ << QString::number(__LINE__) << " tableRow =" << QString::number(tableRow) ;
     	    }
     	else
     	{
     		row++;
     	    }
         }
     return tableRow;    
 }
 
 int VariantItemModel::getNextIdPrimkey()
 {
      QSqlDatabase db = QSqlDatabase::database("prevention");
      QStringList listOfId;
      QSqlQuery q(db);
      QString req = QString("SELECT %1 FROM %2").arg("ID_Primkey","prevention");
     	  if (!q.exec(req))
     	  {
     	  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
     	      }
     	  while (q.next())
     	  {
     	  	QString id = q.value(0).toString();
     	  	qDebug() << __FILE__ << QString::number(__LINE__) << " id =" <<  id;
     	  	listOfId << id;
     	      }
     	   //listOfId.sort();
     	   qDebug() << __FILE__ << QString::number(__LINE__) << " listOfId last =" << listOfId.last() ;
     	   int idPrev = listOfId.last().toInt() +1 ;
     	   
      return idPrev;     
 }

 Qt::ItemFlags VariantItemModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return 0;

     return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
 }

 TreeItem *VariantItemModel::getItem(const QModelIndex &index) const
 {
     if (index.isValid()) {
         TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
         if (item) return item;
     }
     return m_rootItem;
 }

 QVariant VariantItemModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
         return m_rootItem->data(section);
         }
     return QVariant();
 }

 QModelIndex VariantItemModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

     TreeItem *parentItem = getItem(parent);

     TreeItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }
 
QHash<int,QVariant> VariantItemModel::childs(QModelIndex &parent){
      QHash<int,QVariant> listHash;
      TreeItem * parentItem = getItem(parent);
      int childCount = parentItem->childCount();
      for (int i = 0; i < childCount; i += 1)
      {
      	  TreeItem * child = parentItem->child(i);
      	  QVariant data = child->data(ITEM_H);
      	  
      	  listHash.insert(i,data);
          }
      return listHash;
  }

 bool VariantItemModel::insertColumns(int position, int columns, const QModelIndex &parent)
 {
     bool success;

     beginInsertColumns(parent, position, position + columns - 1);
     success = m_rootItem->insertColumns(position, columns);
     endInsertColumns();

     return success;
 }

 bool VariantItemModel::insertRows(int position, int rows, const QModelIndex &parent)
 {
     bool success;
     TreeItem *parentItem = getItem(parent);
     beginInsertRows(parent, position, position + rows - 1);
     success = parentItem->insertChildren(position, rows, Prevention_Items_MaxParam,true);
     endInsertRows();
     return success;
 }
 
 int VariantItemModel::findNextId()
 {
      QSqlDatabase db = QSqlDatabase::database("prevention");
      QStringList listOfId;
      QSqlQuery q(db);
      QString req = QString("SELECT %1 FROM %2").arg("ID_ITEM","prevention");
     	  if (!q.exec(req))
     	  {
     	  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
     	      }
     	  while (q.next())
     	  {
     	  	QString id = q.value(0).toString();
     	  	qDebug() << __FILE__ << QString::number(__LINE__) << " id =" <<  id;
     	  	listOfId << id;
     	      }
     	   //listOfId.sort();
     	   qDebug() << __FILE__ << QString::number(__LINE__) << " listOfId last =" << listOfId.last() ;
     	   int idOfItem = listOfId.last().toInt() +1;
     	   
      return idOfItem;
 }

 QModelIndex VariantItemModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     TreeItem *childItem = getItem(index);
     TreeItem *parentItem = childItem->parent();

     if (parentItem == m_rootItem)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }
 
 bool VariantItemModel::removeColumns(int position, int columns, const QModelIndex &parent)
 {
     bool success;

     beginRemoveColumns(parent, position, position + columns - 1);
     success = m_rootItem->removeColumns(position, columns);
     endRemoveColumns();

     if (m_rootItem->columnCount() == 0)
         removeRows(0, rowCount());

     return success;
 }

 bool VariantItemModel::removeRows(int position, int rows, const QModelIndex &parent)
 {
     TreeItem *parentItem = getItem(parent);
     bool success = true;

     beginRemoveRows(parent, position, position + rows - 1);
     success = parentItem->removeChildren(position, rows);
     endRemoveRows();

     return success;
 }

 int VariantItemModel::rowCount(const QModelIndex &parent) const
 {
     TreeItem *parentItem = getItem(parent);

     return parentItem->childCount();
 }

 bool VariantItemModel::setHeaderData(int section, Qt::Orientation orientation,
                               const QVariant &value, int role)
 {
     if (role != Qt::EditRole || orientation != Qt::Horizontal)
         return false;

     bool result = m_rootItem->setData(section, value);

     if (result)
         emit headerDataChanged(orientation, section, section);

     return result;
 }
//FEEL THE MODEL WITH :
 void VariantItemModel::setupModelData(QSqlTableModel *model, TreeItem *parent)
 {
     //ID ITEM TYPE_OF_ITEM PARENT_ITEM PARENT_OR_CHILD ICON DATE_DONE DATE_NEXT ABSTRACT ID_ITEM RESULT 
     //N  Y    N            N           N               Y    Y         Y         Y        Y       N

     for (int row = 0; row < model->rowCount(); row += 1)
     {
     	 QVector<QVariant> items;
     	 for (int column = 0; column < model->columnCount(); column += 1)
     	 {
     	 	  QVariant item = model->data(model->index(row,column),Qt::DisplayRole);
     	 	  items << item;
     	     }

     	 int parentOrChild = 0;
     	 parentOrChild = items.value(PARENT_OR_CHILD).toInt();
    	 
     	 //insertChildren(int position, int count, int columns)
     	 TreeItem * itemParent;
     	 QString parentName;
     	 switch(parentOrChild){
     	     case PARENT :
     	         parentName = items[ITEM].toString();
     	         parent->insertChildren(parent->childCount(), 1, Prevention_Items_MaxParam,false);
     	         parent->fillParentsHash(parentName, parent->child(parent->childCount() - 1));
     	         for (int i = 1; i < parent->columnCount()+1; i += 1)
     	         {
     	         	QVariant itemData;
     	         	if (i>items.size() -1 )
     	         	{
     	         		  itemData = QVariant();
     	         	    }
     	         	else
     	         	{
     	         		itemData = items[i];
     	         		}
     	         	parent->child(parent->childCount() - 1)->setData(i-1,itemData);
     	               }
     	         //parent->appendChild(new TreeItem(items,parent));
     	         break;
     	     case CHILD :
     	         itemParent = parent->child(items[PARENT_ITEM].toString());
     	         itemParent->insertChildren(itemParent->childCount(),1,Prevention_Items_MaxParam,false);
     	         for (int i = 1; i < itemParent->columnCount()-1; i += 1)
     	         {
     	         	itemParent->child(itemParent->childCount() -1)->setData(i-1,items[i]);
     	             }
     	         break;
     	     default :
     	         break;    
     	     }
         }
     
 }

QStringList VariantItemModel::getListOfMainItems(){
    QStringList list;
    //list << "Vaccinations" << "Coloscopies" << "Mammographies";
    return list;
}

TreeItem * VariantItemModel::getRootItem()
{
    return m_rootItem;
}


PreventIO::PreventIO(QObject * parent){
    m_db = QSqlDatabase::database("prevention");
    m_model = new QSqlTableModel(this,m_db);
    m_model->setTable("prevention");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    const QStringList headers = setHeadersDatas();
    m_variantModel = new VariantItemModel(headers,m_model);
}

PreventIO::~PreventIO(){}

QString PreventIO::correct(QVariant variant)
{
    QByteArray b = variant.toByteArray();
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString string = codec->toUnicode(b);
    return string;
}

const QStringList PreventIO::setHeadersDatas(){
    QStringList list;
    list << trUtf8("Item") 
         << trUtf8("Type")
         << trUtf8("Parent")
         << trUtf8("parent or child")
         << trUtf8("Valid")
         << trUtf8("Date done")
         << trUtf8("Date next")
         << trUtf8("Abstract")
         << trUtf8("ID of Item")
         ;
    return list;
}

bool PreventIO::addAnItemAccordingToIndex(QModelIndex & index,QModelIndex & parent, QObject * parentObject)
{
    bool success = true;
    TreeViewOfPrevention * treeView = static_cast<TreeViewOfPrevention*>(parentObject);
    if(!parent.isValid()){
         int modelLastRowCount = m_model->rowCount();
         qDebug() << __FILE__ << QString::number(__LINE__) << "modelLastRowCount  =" << QString::number(modelLastRowCount) ;
         qDebug() << __FILE__ << QString::number(__LINE__) << " m_variantModel->rowCount(index) =" << QString::number(m_variantModel->rowCount(index)) ;
    	 int variantModelRowCount = m_variantModel->rowCount(index);
    	 success = m_variantModel->insertRows(variantModelRowCount,1,index);
    	 m_model->select();
    	 int modelNewRowCount = m_model->rowCount();
    	 qDebug() << __FILE__ << QString::number(__LINE__) << " modelNewRowCount =" << QString::number(modelNewRowCount) ;
    	 treeView->collapse(index);
    	 treeView->expand(index);
    	 TreeItem * parentItem = m_variantModel->getItem(index);
    	 TreeItem * child = parentItem->child(parentItem->childCount()-1);
    	 qDebug() << __FILE__ << QString::number(__LINE__) << " parent = ROOT"   ;
    	 for (int col = 1; col < m_model->columnCount()-1; col += 1)
         {
    	    QVariant value = m_model->data(m_model->index(modelNewRowCount-1,col),Qt::DisplayRole);
    	    qDebug() << __FILE__ << QString::number(__LINE__) << " value  =" << value.toString() ;
    	    if (!child->setData(col-1,value))
    	    {
    	    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to set variantmodel data" ;
    	        }
            }
    }
    else
    {
    	qDebug() << __FILE__ << QString::number(__LINE__) << " m_variantModel->rowCount(parent) =" << QString::number(m_variantModel->rowCount(parent)) ;
    	int modelLastRowCount = m_model->rowCount();
    	int variantModelRowCount = m_variantModel->rowCount(parent);
    	success = m_variantModel->insertRows(variantModelRowCount,1,parent);
    	m_model->select();
    	int modelNewRowCount = m_model->rowCount();
    	treeView->collapse(parent);
    	treeView->expand(parent);
    	TreeItem * parentItem = m_variantModel->getItem(parent);
    	TreeItem * child = parentItem->child(parentItem->childCount()-1);
    	qDebug() << __FILE__ << QString::number(__LINE__) << " parent = parent"   ;
    	for (int col = 1; col < m_model->columnCount()-1; col += 1)
         {
    	    QVariant value = m_model->data(m_model->index(m_model->rowCount()-1,col),Qt::DisplayRole);
    	    qDebug() << __FILE__ << QString::number(__LINE__) << " value  =" << value.toString() ;
    	    if (!child->setData(col-1,value))
    	    {
    	    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to set variantmodel data" ;
    	        }
            }
        }
    
    
    /*if (!m_variantModel->clear())
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to clear VariantItemModel !" ;
        }
    else
    {
    	//QStringList headers = setHeadersDatas();
    	//m_variantModel->setRootItem(headers);
    	TreeItem * rootItem = m_variantModel->getRootItem();
    	m_variantModel->setupModelData(m_model,rootItem);
        }*/
    return success;
}

QSqlTableModel * PreventIO::getModel()
{
    return m_model;
}

