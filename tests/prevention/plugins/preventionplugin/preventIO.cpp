#include "preventIO.h"
#include "preventIHM.h"

#include <QAbstractItemModel>

static const QString PREVENT = QString("prevention");
static inline QString pixmaps() 
{ 
    return QString(qApp->applicationDirPath()+"/../../global_resources/pixmap");
};

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
 
 void TreeItem::fillParentsHash(const QString & parentName, TreeItem * item)
 {
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
 
 QVector<QVariant> TreeItem::datas(int columns)
 {
     QVector<QVariant> datas;
     for (int i = 0; i < columns; i += 1)
     {
     	datas << data(i);
         }
     return datas;
 }

 bool TreeItem::insertChildren(int position, int count, int columns)
 {
     if (position < 0 || position > m_childItems.size())
         return false;
     	 for (int row = 0; row < count; ++row) {
             QVector<QVariant> datasVector = datas(columns);
             TreeItem *item = new TreeItem(datasVector, this);
             m_childItems.insert(position, item);
             m_treeItemsHash.insert(datasVector[VariantItemModel::ITEM].toString(),item);
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
     m_db = QSqlDatabase::database("prevention");
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

     	  removeRows(1,m_rootItem->childCount(),QModelIndex());

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
     if (role != Qt::DisplayRole /* && index.column()!= ICON_H*/){
         return QVariant();
         }
     TreeItem *item = getItem(index);
     QVariant itemData = item->data(index.column());
     if (index.column() == ICON_H)
     {
     	 if(role == Qt::DecorationRole || role == Qt::DisplayRole)
     	     return itemData;
     	  return QVariant();
    	 
     	     
         }

     return itemData;
 }
 
 bool VariantItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
     if (role != Qt::EditRole && (role == Qt::DecorationRole && index.column() != ICON_H ) )
         return false;
     bool result = true;
     TreeItem *item = getItem(index);
     TreeItem *parent = item->parent();
     QModelIndex parentIndex = index.parent();
     
     int idOfItem = 0;
     idOfItem = item->data(ID_ITEM_H).toInt();
     result = item->setData(index.column(), value);
     int rowOfSqlTable = getSqlTableRow(idOfItem);
     if (idOfItem == 0)
     {
     	  return false;
         }
     if (result){
         emit dataChanged(index, index);
         result = m_modelSql->setData(m_modelSql->index(rowOfSqlTable,index.column()+1),value,Qt::EditRole);
         result = m_modelSql->submitAll();
         }
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
     	if (id == idOfItem)
     	{
     		  tableRow = row ;
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
     	  	listOfId << id;
     	      }
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
     int actualModelRowCount = m_modelSql->rowCount();
     beginInsertRows(parent, position, position + rows - 1);
     success = parentItem->insertChildren(position, rows, Prevention_Items_MaxParam);
     success = m_modelSql->insertRows(actualModelRowCount,rows,QModelIndex());
     m_modelSql->submit();
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
     	  	listOfId << id;
     	      }
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
     for (int position = 0; position < position +rows -1; position += 1)
     {
     	  QString itemId = parentItem->child(position)->data(ID_ITEM_H).toString();
          QSqlQuery q(m_db);
          QString req = QString("DELETE FROM %1 WHERE %2 LIKE '%3'")
          .arg("prevention",QString::number(ID_ITEM),itemId);
          if (!q.exec(req))
          {
          	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
              }
         }

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
     //N  Y    N            Y           N               Y    Y         Y         Y        Y       N

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
     	 TreeItem * itemParent;
     	 QString parentName;
     	 switch(parentOrChild){
     	     case PARENT :
     	         parentName = items[ITEM].toString();
     	         parent->insertChildren(parent->childCount(), 1, Prevention_Items_MaxParam);
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
     	         break;
     	     case CHILD :
     	         itemParent = parent->child(items[PARENT_ITEM].toString());
     	         itemParent->insertChildren(itemParent->childCount(),1,Prevention_Items_MaxParam);
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
    return list;
}

TreeItem * VariantItemModel::getRootItem()
{
    return m_rootItem;
}

TreeItem * VariantItemModel::findTreeItemWithIdItem(QVariant & idItem)
{
    TreeItem * itemWithIdItem;
    for (int rowParent = 0; rowParent < rowCount(QModelIndex()); rowParent += 1)
    {
    	  QModelIndex parent = VariantItemModel::index(rowParent,0,QModelIndex());
    	  TreeItem * parentItem = m_rootItem->child(rowParent);
    	  for (int rowChild = 0; rowChild < rowCount(parent); rowChild += 1)
    	  {
    	  	  TreeItem * item = parentItem->child(rowChild);
    	  	  QVariant idOfThisItem = item->data(ID_ITEM_H);
    	  	  if (idOfThisItem == idItem)
    	  	  {
    	  	  	  itemWithIdItem = item;
    	  	      }
    	      }
        }
    return itemWithIdItem;
}

QModelIndex VariantItemModel::indexOfIconOfItem(TreeItem * item)
{
    QModelIndex index;
    TreeItem * parentItem = item->parent();
    int rowParent = parentItem->childNumber();
    QModelIndex parentIndex = VariantItemModel::index(rowParent,0) ; 
    int rowChild = item->childNumber();
    index = VariantItemModel::index(rowChild,int(VariantItemModel::ICON_H),parentIndex);
    return index;
}

void VariantItemModel::setIconWarning(QVariant & idItem)
{
    QVariant preventWarningIcon = QVariant(pixmaps()+"/preventWarning.png");
    const QString filter = QString("%1='%2'").arg("ID_ITEM",idItem.toString());
    m_modelSql->setFilter(filter);
    m_modelSql->select();
    m_modelSql->setData(m_modelSql->index(0,ICON),preventWarningIcon,Qt::EditRole);
    m_modelSql->submitAll();
    m_modelSql->setFilter("");
    m_modelSql->select();
    
    emit layoutAboutToBeChanged();
    TreeItem * itemChanged = findTreeItemWithIdItem(idItem);
    itemChanged->setData(ICON_H,preventWarningIcon);
    QModelIndex index = indexOfIconOfItem(itemChanged);
    emit dataChanged(index,index);
    emit layoutChanged();
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
    TreeItem * parentItem = m_variantModel->getItem(parent);
    TreeItem * child = m_variantModel->getItem(index);
    TreeItem * item;
    QModelIndex inIndex;
    if(!parent.isValid()){
        inIndex = index;
        item = child;
        }
    else
    {
    	inIndex = parent;
        item = parentItem;
        }
         int modelLastRowCount = m_model->rowCount();
         
    	 int variantModelRowCount = m_variantModel->rowCount(inIndex);
    	 int nextIdPrimkey = m_variantModel->getNextIdPrimkey();
    	 success = m_variantModel->insertRows(variantModelRowCount,1,inIndex);
    	 int newModelRow = m_model->rowCount()-1;
    	 if (!m_model->setData(m_model->index(newModelRow,VariantItemModel::ID_PREVENTION),
    	          nextIdPrimkey,Qt::EditRole))
    	 {
    	 	  qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO SET PRIMKEY" ;
    	     }
    	 for (int col = 0; col < VariantItemModel::Headers_MaxParam; col += 1)
    	 {
    	         int role = Qt::EditRole;
    	 	 QVariant value;
    	 	 switch(col){
    	 	     case VariantItemModel::ITEM_H:
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     case VariantItemModel::TYPE_OF_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(VariantItemModel::PRIMARY_PREVENTION_ITEM);
    	 	         break;
    	 	     case VariantItemModel::PARENT_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = item->data(VariantItemModel::ITEM_H);
    	 	         break;
    	 	     case VariantItemModel::PARENT_OR_CHILD_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(VariantItemModel::CHILD);
    	 	         break;
    	 	     case VariantItemModel::ICON_H :
    	 	         role = Qt::DecorationRole;
    	 	         value = QIcon(pixmaps()+"/preventOk.png");
    	 	         break;
    	 	     case VariantItemModel::DATE_DONE_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
    	 	         break;
    	 	     case VariantItemModel::DATE_NEXT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
    	 	         break;
    	 	     case VariantItemModel::ABSTRACT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     case VariantItemModel::ID_ITEM_H :
    	 	         qDebug() << __FILE__ << QString::number(__LINE__) << "m_variantModel->findNextId()  =" 
    	 	                  << QString::number(m_variantModel->findNextId()) ;
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(m_variantModel->findNextId());
    	 	         break;
    	 	     case VariantItemModel::RESULT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     default :
    	 	         break;    
    	 	     }
    	 	  
    	 	  item->child(item->childCount() -1)->setData(col,value);   

  	  	  if (!m_model->setData(m_model->index(newModelRow,col+1),value,role))
    	 	          {
    	 	  	      qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO SET NEW DATA" ;
    	 	              }
    	 	      
    	          }
    	          if (!m_model->submitAll())
    	          {
    	          	  qWarning() << __FILE__ << QString::number(__LINE__) 
    	          	             << "CANNOT SUBMIT ALL : "
    	          	             << m_model->lastError().text() ; ;
    	          	  
    	              }

    	 treeView->collapse(inIndex);
    	 treeView->expand(inIndex);

    return success;
}

QSqlTableModel * PreventIO::getModel()
{
    return m_model;
}

QSqlDatabase PreventIO::getDatabase()
{
    return m_db;
}

