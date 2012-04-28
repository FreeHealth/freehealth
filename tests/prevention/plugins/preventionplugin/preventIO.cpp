#include "preventIO.h"
#include "preventIHM.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <utils/log.h>

#include <QAbstractItemModel>
#include <QMessageBox>


///////////////////////////////////////////////////////////////////
// Block pour l'utilisation des pixmaps du theme de FreeMedForms //
///////////////////////////////////////////////////////////////////
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

// Pour récupérer le chemin vers un pixmap
//     theme()->iconFullPath(Core::Constants::ICONABOUT); // sous entendu Core::ITheme::SmallSize
//     theme()->iconFullPath(Core::Constants::ICONABOUT, Core::ITheme::MediumSize);
//     theme()->iconFullPath(Core::Constants::ICONABOUT, Core::ITheme::BigSize);

// Pour récupérer l'icône
//     theme()->icon(Core::Constants::ICONABOUT); // sous entendu Core::ITheme::SmallSize
//     theme()->icon(Core::Constants::ICONABOUT, Core::ITheme::MediumSize);
//     theme()->icon(Core::Constants::ICONABOUT, Core::ITheme::BigSize);


static const QString PREVENT = QString("prevention");
static inline QString pixmaps() 
{ 
    return QString(qApp->applicationDirPath()+"/../../global_resources/pixmap");
};
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

static inline QStringList patientAndUserUuid() //TODO
{
     QStringList listOfUuids;
     QString patientUuid = "111111111111111111111111111111";
     QString userUuid = "1";
     if (patient())
     {
     	  //patientUuid = patient()->uuid();
         }
     if (user())
     {
     	  //userUuid = user()->uuid();
         }
     listOfUuids << patientUuid << userUuid;
     return listOfUuids;
}

//BEGIN OF TREEITEM = ITEMS FOR THE TREEVIEW

  TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
 {
     m_parentItem = parent;
     m_itemData = data;
     m_parenthood = data[PARENT_OR_CHILD_H].toInt();
 }

 TreeItem::~TreeItem()
 {
     qDeleteAll(m_childItems);
     qDeleteAll(m_treeItemsHash);
     //m_itemData.clear();
 }
 
  void TreeItem::appendChild(TreeItem *item)
 {
     m_childItems.append(item);
 }
 
 void TreeItem::fillParentsHash(const QString & parentName, TreeItem * item)
 {
     if(m_parenthood == PARENT || m_itemData[ID_ITEM_H].toString().contains(trUtf8("ID of Item")))
     {
         m_treeItemsHash.insert(parentName,item);
     }
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
             //m_treeItemsHash.insert(datasVector[VariantItemModel::ITEM].toString(),item);
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
     for (int row = position; row < position+count; ++row)
     {
         if(m_childItems.value(row)){
             
             if (child(row))
             {
             	  int parentOrChild = child(row)->data(PARENT_OR_CHILD_H).toInt();
             	  if(parentOrChild == PARENT)
             	  {
             	      m_treeItemsHash.remove(m_treeItemsHash.key(m_childItems.at(row)));
             	      }
                  if (!m_childItems.isEmpty())
                      delete m_childItems.takeAt(row);
                  }
              }
          }
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
 
 QHash<QString,TreeItem*> TreeItem::treeParentItems()
 {
     return m_treeItemsHash;
 }
 
 
//end of TREEITEM
//VARIANTITEMMODEL UNDER THE TREEVIEW
VariantItemModel::VariantItemModel(QSqlTableModel * model, QObject *parent)
     : QAbstractItemModel(parent)
 {
     m_modelSql = new QSqlTableModel;
     
     if (model){
         m_modelSql = model;
         m_modelSql->select();
         }
    
    QStringList listHeaders;
    listHeaders << trUtf8("Item") 
                << trUtf8("Type")
                << trUtf8("Parent")
                << trUtf8("parent or child")
                << trUtf8("Valid")
                << trUtf8("Date done")
                << trUtf8("Date next")
                << trUtf8("Abstract")
                << trUtf8("ID of Item")
                << trUtf8("Patient_uid")
                << trUtf8("User_uid")
                ;
     QVector<QVariant> rootData;
     for (int i = 0; i < listHeaders.size(); i += 1)
     {
     	  rootData << listHeaders[i];
         }
        
     m_rootItem = new TreeItem(rootData,0);
     m_db = QSqlDatabase::database("prevention");
     
     setupModelData(m_modelSql, m_rootItem);
     
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
        
     m_rootItem = new TreeItem(rootData,0);
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
     if (index.column() == PreventionEngine::IPreventionEngine::ICON_H)
     {
     	 if(role == Qt::DecorationRole || role == Qt::DisplayRole)
     	     return itemData;
     	  return QVariant();
    	 
     	     
         }

     return itemData;
 }
 
 bool VariantItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
     if (role != Qt::EditRole && (role == Qt::DecorationRole && index.column() != PreventionEngine::IPreventionEngine::ICON_H ) )
         return false;
     bool result = true;
     TreeItem *item = getItem(index);
     //TreeItem *parent = item->parent();
     QModelIndex parentIndex = index.parent();
     int idOfItem = 0;
     idOfItem = item->data(PreventionEngine::IPreventionEngine::ID_ITEM_H).toInt();
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
     QString patientUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::PATIENT];
     QString userUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::USER];
     QSqlDatabase db = QSqlDatabase::database("prevention");
     QSqlQuery q(db);
     QString req = QString("SELECT %1 FROM %2").arg("ID_ITEM","prevention");
     req += QString(" WHERE %1 LIKE '%2' AND %3 LIKE '%4'")
               .arg("PATIENT_UID",patientUuid,"USER_UID",userUuid);
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
      	  QVariant data = child->data(PreventionEngine::IPreventionEngine::ITEM_H);
      	  
      	  listHash.insert(i,data);
          }
      return listHash;
  }
  
QMap<int,QVector<QVariant> > VariantItemModel::childsAndItems(QModelIndex & parent)
{
    QMap<int,QVector<QVariant> > hash;
      TreeItem * parentItem = getItem(parent);
      int childCount = parentItem->childCount();
      for (int i = 0; i < childCount; i += 1)
      {
      	  TreeItem * child = parentItem->child(i);
      	  QString data = child->data(PreventionEngine::IPreventionEngine::ITEM_H).toString();
      	  QString id = child->data(PreventionEngine::IPreventionEngine::ID_ITEM_H).toString();
      	  QVector<QVariant> data_id ;
      	  data_id << data << id;
      	  hash.insert(i,data_id);
          }    
     return hash;
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
     success = parentItem->insertChildren(position, rows, PreventionEngine::IPreventionEngine::Prevention_Items_MaxParam);
     for (int row = 0; row < rows; row += 1)
     {
     	  for (int col = 0; col < PreventionEngine::IPreventionEngine::Prevention_Items_MaxParam; col += 1)
     	  {
     	  	  createIndex(position+row,col,parentItem);
     	      }
         }
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
     if (!parent.isValid())
     {
     	  parentItem = getRootItem();
         }
     beginRemoveRows(parent, position, position + rows - 1);
     for (int pos = position; pos < rows+position ; pos += 1)
     {
     	  QString itemId = parentItem->child(position)->data(PreventionEngine::IPreventionEngine::ID_ITEM_H).toString();
          QSqlQuery q(m_db);
          QString req = QString("DELETE FROM %1 WHERE %2 LIKE '%3'")
          .arg("prevention","ID_ITEM",itemId);
          if (!q.exec(req))
          {
          	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
              }
          success = parentItem->removeChildren(position, 1);
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
     //ID ITEM TYPE_OF_ITEM PARENT_ITEM PARENT_OR_CHILD ICON DATE_DONE DATE_NEXT ABSTRACT ID_ITEM PATIENT_UID USER_UID RESULT 
     //N  Y    N            Y           N               Y    Y         Y         Y        Y       N
     QString patientUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::PATIENT];
     QString userUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::USER];
     QString patientAndUserFilter = QString("%1 LIKE '%2' AND %3 LIKE '%4'")
                                   .arg("PATIENT_UID",patientUuid,"USER_UID",userUuid);
     model->setFilter(patientAndUserFilter);
     model->select();
     if (model->rowCount()< 1)
     {
     	  return;
         }
     for (int row = 0; row < model->rowCount(); row += 1)
     {
     	 QVector<QVariant> items;
     	 for (int column = 0; column < model->columnCount(); column += 1)
     	 {
     	 	  QVariant item = model->data(model->index(row,column),Qt::DisplayRole);
     	 	  items << item;
     	     }
     	 int parentOrChild = 0;
     	 parentOrChild = items.value(PreventionEngine::IPreventionEngine::PARENT_OR_CHILD).toInt();
     	 TreeItem * itemParent;
     	 QString parentName;
     	 int childCount = 0;
     	 switch(parentOrChild){
     	     case PreventionEngine::IPreventionEngine::PARENT :
     	         parentName = items[PreventionEngine::IPreventionEngine::ITEM].toString();
     	         if(parent){
     	             childCount = parent->childCount();
     	             parent->insertChildren(childCount, 1, PreventionEngine::IPreventionEngine::Prevention_Items_MaxParam);
     	             parent->fillParentsHash(parentName,parent->child(parent->childCount()-1));
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
     	                  }
     	               
     	         break;
     	     case PreventionEngine::IPreventionEngine::CHILD :
     	         
     	         if(parent)
     	         itemParent = parent->child(items[PreventionEngine::IPreventionEngine::PARENT_ITEM].toString());
     	         
     	         if(itemParent)     	         
     	             {
     	              childCount = itemParent->childCount();
     	              itemParent->insertChildren(childCount,1,PreventionEngine::IPreventionEngine::Prevention_Items_MaxParam);
     	              for (int i = 1; i < itemParent->columnCount()-1; i += 1)
     	              {
     	         	  itemParent->child(itemParent->childCount() -1)->setData(i-1,items[i]);
     	                  }
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
    	  	  QVariant idOfThisItem = item->data(PreventionEngine::IPreventionEngine::ID_ITEM_H);
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
    index = VariantItemModel::index(rowChild,int(PreventionEngine::IPreventionEngine::ICON_H),parentIndex);
    return index;
}

void VariantItemModel::setIconWarning(QVariant & idItem)
{
    QVariant preventWarningIcon = QVariant(pixmaps()+"/preventWarning.png");
    const QString filter = QString("%1='%2'").arg("ID_ITEM",idItem.toString());
    m_modelSql->setFilter(filter);
    m_modelSql->select();
    m_modelSql->setData(m_modelSql->index(0,PreventionEngine::IPreventionEngine::ICON),preventWarningIcon,Qt::EditRole);
    m_modelSql->submitAll();
    m_modelSql->setFilter("");
    m_modelSql->select();
    
    emit layoutAboutToBeChanged();
    TreeItem * itemChanged = findTreeItemWithIdItem(idItem);
    itemChanged->setData(PreventionEngine::IPreventionEngine::ICON_H,preventWarningIcon);
    QModelIndex index = indexOfIconOfItem(itemChanged);
    emit dataChanged(index,index);
    emit layoutChanged();
}

bool VariantItemModel::addAnItemAccordingToIndex(QModelIndex & index,QModelIndex & parent, QObject * parentObject)
{
    bool success = true;
    PreventIHM * parentOfIO = static_cast<PreventIHM*>(m_modelSql->QObject::parent());
    TreeViewOfPrevention *treeView = static_cast<TreeViewOfPrevention*>(parentObject);
    QString patientUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::PATIENT];
    QString userUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::USER];    
    TreeItem * parentItem = getItem(parent);
    TreeItem * child = getItem(index);
    TreeItem * item;
    TreeItem * itemNew;
    
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
         
    	 int variantModelRowCount = rowCount(inIndex);
    	 int nextIdPrimkey = getNextIdPrimkey();
    	 success = insertRows(variantModelRowCount,1,inIndex);
    	 if (!success)
    	 {
    	 	  qWarning() << __FILE__ << QString::number(__LINE__) 
    	 	  << "unable to insert rows in the treeview" ;
    	     }
    	 int newModelRow = m_modelSql->rowCount()-1;
    	 if (!m_modelSql->setData(m_modelSql->index(newModelRow,PreventionEngine::IPreventionEngine::ID_PREVENTION),
    	          nextIdPrimkey,Qt::EditRole))
    	 {
    	 	  qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO SET PRIMKEY" ;
    	     }
    	 itemNew = item->child(item->childCount() -1);
    	 for (int col = 0; col < PreventionEngine::IPreventionEngine::Headers_MaxParam; col += 1)
    	 {
    	         int role = Qt::EditRole;
    	 	 QVariant value;
    	 	 switch(col){
    	 	     case PreventionEngine::IPreventionEngine::ITEM_H:
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::TYPE_OF_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(PreventionEngine::IPreventionEngine::PRIMARY_PREVENTION_ITEM);
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::PARENT_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = item->data(PreventionEngine::IPreventionEngine::ITEM_H);
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::PARENT_OR_CHILD_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(PreventionEngine::IPreventionEngine::CHILD);
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::ICON_H :
    	 	         role = Qt::EditRole;
    	 	         value = pixmaps()+"/preventOk.png";
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::DATE_DONE_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::DATE_NEXT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::ABSTRACT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::ID_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(findNextId());
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::PATIENT_UID_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(patientUuid);
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::USER_UID_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(userUuid);
    	 	         break;      	 	         
    	 	     case PreventionEngine::IPreventionEngine::RESULT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     default :
    	 	         break;    
    	 	     }
    	     if (!itemNew->setData(col,value))
    	     {
    	         qWarning() << __FILE__ << QString::number(__LINE__) << "unable to set data" ;
    	         }   
  	  	  if (!m_modelSql->setData(m_modelSql->index(newModelRow,col+1),value,role))
    	 	          {
    	 	  	   qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO SET NEW DATA"
    	 	  	         << " for value =" << value.toString() << " and col :" << QString::number(col);
    	 	  	      }
    	 	      
    	          }
    if (!m_modelSql->submitAll())
    {
        qWarning() << __FILE__ << QString::number(__LINE__) 
    	          	       << "CANNOT SUBMIT ALL : "
    	          	       << m_modelSql->lastError().text() ;
    	}
     if (!submit())
     {
     	  qWarning() << __FILE__ << QString::number(__LINE__) << "treeview model cannot submit" ;
         }
    	 qDebug() << __FILE__ << QString::number(__LINE__) << "next variantModelRowCount =" << QString::number(variantModelRowCount) ;          
    	 treeView->collapse(inIndex);
    	 treeView->getModel(this);
    	 treeView->expand(inIndex);
    	 parentOfIO->changeIconWidget();
    return success;
}

bool VariantItemModel::deleteItemAccordingToIndex(QModelIndex & index,QModelIndex & parent, QObject * parentObject)
{
    Q_UNUSED(parentObject);
    bool success = true;
    //TreeViewOfPrevention *treeView = static_cast<TreeViewOfPrevention*>(parentObject);
    TreeItem * parentItem = getItem(parent);
    TreeItem * child = getItem(index);
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
    int itemChildCount = item->childCount();
    int rowToDelete = item->child(itemChildCount-1)->childNumber();
    if (!removeRows(rowToDelete, 1, inIndex))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to remove row" ;
        }        
    return success;
}

bool VariantItemModel::addAGroupItem(QModelIndex & index,QModelIndex & parent,QObject * parentObject)
{
    Q_UNUSED(index);
    QString patientUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::PATIENT];
    QString userUuid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::USER];
    //PreventIHM * parentOfIO = static_cast<PreventIHM*>(m_modelSql->QObject::parent());
    TreeViewOfPrevention *treeView = static_cast<TreeViewOfPrevention*>(parentObject);
    TreeItem * item = getRootItem();
    QModelIndex rootIndex;
    if (!parent.isValid())
    {
    	  rootIndex = parent;
        }
    else
    {
    	rootIndex = parent.parent();
        }
    int variantModelRowCount = rowCount(rootIndex);
    QString parentName = trUtf8("new group");
    bool success = true;
    	 int nextIdPrimkey = getNextIdPrimkey();
    	 success = insertRows(variantModelRowCount,1,rootIndex);
    	 if (!success)
    	 {
    	 	  qWarning() << __FILE__ << QString::number(__LINE__)
    	 	   << "unable to insert row in treeview" ;
    	     }
    	 TreeItem * newItem = item->child(item->childCount() -1);
         item->fillParentsHash(parentName,newItem );
    	 int newModelRow = m_modelSql->rowCount()-1;
    	 if (!m_modelSql->setData(m_modelSql->index(newModelRow,PreventionEngine::IPreventionEngine::ID_PREVENTION),
    	          nextIdPrimkey,Qt::EditRole))
    	 {
    	 	  qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO SET PRIMKEY" ;
    	 	  success = false;
    	     }
    	 for (int col = 0; col < PreventionEngine::IPreventionEngine::Headers_MaxParam; col += 1)
    	 {
    	         int role = Qt::EditRole;
    	 	 QVariant value;
    	 	 switch(col){
    	 	     case PreventionEngine::IPreventionEngine::ITEM_H:
    	 	         role = Qt::EditRole;
    	 	         value = QVariant("new group");
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::TYPE_OF_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(PreventionEngine::IPreventionEngine::PRIMARY_PREVENTION_ITEM);
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::PARENT_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant("ROOT");
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::PARENT_OR_CHILD_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(PreventionEngine::IPreventionEngine::PARENT);
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::ICON_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::DATE_DONE_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::DATE_NEXT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::ABSTRACT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::ID_ITEM_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(findNextId());
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::PATIENT_UID_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(patientUuid);
    	 	         break;
    	 	     case PreventionEngine::IPreventionEngine::USER_UID_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant(userUuid);
    	 	         break;     	 	         
    	 	     case PreventionEngine::IPreventionEngine::RESULT_H :
    	 	         role = Qt::EditRole;
    	 	         value = QVariant();
    	 	         break;
    	 	     default :
    	 	         break;    
    	 	     }
    	 	  if (!newItem->setData(col,value))
    	 	  {
    	 	  	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to set item data" ;
    	 	      }

  	  	  if (!m_modelSql->setData(m_modelSql->index(newModelRow,col+1),value,role))
    	 	          {
    	 	  	      qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO SET NEW DATA" ;
    	 	              }
    	 	      
    	              }//for
    	          if (!m_modelSql->submitAll())
    	          {
    	          	  qWarning() << __FILE__ << QString::number(__LINE__) 
    	          	             << "CANNOT SUBMIT ALL : "
    	          	             << m_modelSql->lastError().text() ;
    	          	  success = false;
    	          	  
    	              }
    qWarning() << __FILE__ << QString::number(__LINE__) << "group is added" ;
    treeView->setModel(this);
    treeView->getModel(this);
    return success;
}

bool VariantItemModel::deleteGroupOfItems(QModelIndex & index)
{
    bool success = true;
    TreeItem * itemParent = getItem(index);
    int position = itemParent->childNumber();
    int childCount = itemParent->childCount();
    if (!removeRows(0,childCount,index))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to delete rows of this parent." ;
        }
    else
    {
    	if (!removeRows(position,1,QModelIndex()))
    	{
    		  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to remove this group." ;
    	    }
    	else{
    	    qWarning() << __FILE__ << QString::number(__LINE__) << "rows deleted" ;
    	    }
        }
    return success;
}
/////////////////////////////////////////
//////////PREVENTIO
/////////////////////////////////////////////////


PreventIO::PreventIO(QObject * parent){
    Q_UNUSED(parent);
    m_db = QSqlDatabase::database("prevention");
    m_model = new QSqlTableModel(parent,m_db);
    m_model->setTable("prevention");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_variantModel = new VariantItemModel(m_model,parent);
    m_NextDateModel = new QSqlTableModel(this,m_db);
    m_NextDateModel->setTable("nextdate");
    m_NextDateModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_NextDateModel->select();
    m_userUid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::USER];
}

PreventIO::~PreventIO(){}

QString PreventIO::correct(QVariant variant)
{
    QByteArray b = variant.toByteArray();
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString string = codec->toUnicode(b);
    return string;
}

QSqlTableModel * PreventIO::getModel()
{
    return m_model;
}

QSqlTableModel * PreventIO::getNextDateModel()
{
    m_NextDateModel->setFilter("");
    m_NextDateModel->select();
    return m_NextDateModel;
}


QSqlDatabase PreventIO::getDatabase()
{
    return m_db;
}

VariantItemModel * PreventIO::getVariantItemModel()
{
    return m_variantModel;
}

QStringList PreventIO::getListOfNextDateItems()
{
    QStringList list;
    QString userUid = patientAndUserUuid()[PreventionEngine::IPreventionEngine::USER] ;
    if (m_NextDateModel->rowCount()>0)
    {
    	  QString filter = QString("%1 LIKE '%2'").arg("USER_UID",userUid);
          m_NextDateModel->setFilter(filter);
        }
    
    m_NextDateModel->select();
    for (int i = 0; i < m_NextDateModel->rowCount(); ++i)
    {
    	  list << m_NextDateModel->data(m_NextDateModel->index(i,PreventionEngine::IPreventionEngine::ND_ITEM),Qt::DisplayRole).toString();
        }
    return list;
}

QString PreventIO::getUserUid()
{
    return m_userUid;
}

QDate PreventIO::getNextDate(const QStringList & listOfDatas, QModelIndex index)
{
    QDate date;
    QDate newDate;
    int years = 0;
    int months = 0;
    int days = 0;
    if (listOfDatas.size()>0)
    {
    	  years = listOfDatas[PreventionEngine::IPreventionEngine::ND_YEAR].toInt();
    	  months = listOfDatas[PreventionEngine::IPreventionEngine::ND_MONTH].toInt();
    	  days = listOfDatas[PreventionEngine::IPreventionEngine::ND_DAY].toInt();
        }
    date = m_variantModel->data(m_variantModel->index(index.row(),PreventionEngine::IPreventionEngine::DATE_DONE_H,index.parent()),Qt::DisplayRole).toDate();
    newDate = date.addYears(years);
    newDate = newDate.addMonths(months);
    newDate = newDate.addDays(days);
    return newDate;
}

bool PreventIO::recordDocument(const QTextDocument *document,const QString & id)
{
    bool success = true;
    const QString text = document->toPlainText();
    QSqlQuery query(m_db);
    const QString req = QString("UPDATE %1 SET %2 = '%3' WHERE %4 LIKE '%5'")
                        .arg("prevention","RESULT",text,"ID_ITEM",id);
    if (!query.exec(req))
    {
    	LOG_ERROR(query.lastError().text()+" "__FILE__+QString::number(__LINE__));
    	qDebug() << __FILE__ << QString::number(__LINE__) << query.lastQuery() ;
    	success = false;
    }
    return success;
}

QString PreventIO::getDocumentRelativeToIdItem(const QString & id_item)
{
    QString text;
    QStringList listTest;
    QSqlQuery query(m_db);
    const QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4' ")
                        .arg("RESULT","prevention","ID_ITEM",id_item);
    if (!query.exec(req))
    {
    	LOG_ERROR(query.lastError().text()+" "__FILE__+QString::number(__LINE__));
    }
    while (query.next())
    {
    	text = query.value(0).toString();
    	listTest << text;
    }
    if (listTest.size()>1)
    {
    	/*Utils::warningMessageBox( trUtf8("ERROR"),
    	                          trUtf8("There is two results for the same ID"), 
    	                          __FILE__+" "+QString::number(__LINE__), 
    	                          trUtf8("Warning") );*/
    }
    return text;
}

QString PreventIO::getIdItem(const QModelIndex & indexInRow)
{
    QString idItem;
    QModelIndex parent = indexInRow.parent();
    idItem = m_variantModel->data(m_variantModel->index(indexInRow.row(),PreventionEngine::IPreventionEngine::ID_ITEM_H, parent),Qt::DisplayRole).toString();
    return idItem;
}

