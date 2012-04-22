#include "treeviewdelegate.h"
#include "nextdateitemsdialog.h"

static inline QString pixmaps() 
{ 
    return QString(qApp->applicationDirPath()+"/../../global_resources/pixmap");
};

DateEditTreeViewFirstDelegate::DateEditTreeViewFirstDelegate(QObject * parent)
{
}

DateEditTreeViewFirstDelegate::~DateEditTreeViewFirstDelegate(){}

QSize DateEditTreeViewFirstDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(40,25);
}

QWidget *DateEditTreeViewFirstDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (  index.column() == PreventionEngine::IPreventionEngine::DATE_DONE_H)
    {
    	  QDateEdit * dateEdit = new QDateEdit(parent);
    	  dateEdit->setDisplayFormat("yyyy-MM-dd");
    	  return dateEdit;
        }
          return QStyledItemDelegate::createEditor(parent, option, index);

     
}

 void DateEditTreeViewFirstDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QDate date;
     date = index.model()->data(index,Qt::DisplayRole).toDate();
     dateEdit->setDate(date);
 }
 
  void DateEditTreeViewFirstDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QString value = dateEdit->date().toString("yyyy-MM-dd");

     model->setData(index, value, Qt::EditRole);
 }

 ////////////////////////////////////////////
 //DATEEDIT NEXT DELEGATE FOR TREEVIEW
 ///////////////////////////////////////////

DateEditTreeViewDelegate::DateEditTreeViewDelegate(QObject * parent)
{
    m_io = static_cast<PreventIO*>(parent);
    m_itemsParametrizedList = m_io->getListOfNextDateItems();
    m_tableNextDateModel = m_io->getNextDateModel();
    m_userUid = m_io->getUserUid();
}

DateEditTreeViewDelegate::~DateEditTreeViewDelegate(){}

QSize DateEditTreeViewDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(40,25);
}

QWidget *DateEditTreeViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (index.column() == PreventionEngine::IPreventionEngine::DATE_NEXT_H)
    {
    	  QDateEdit * dateEdit = new QDateEdit(parent);
    	  dateEdit->setDisplayFormat("yyyy-MM-dd");
    	  return dateEdit;
        }
          return QStyledItemDelegate::createEditor(parent, option, index);
    
}

 void DateEditTreeViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QDate date;
     int rowIndex = index.row();
     QString item = index.model()->data(index.model()->index(rowIndex,ITEM_H,index.parent()),Qt::DisplayRole).toString();
     qDebug() << __FILE__ << QString::number(__LINE__) << " item =" << item ;
     QStringList listOfItems;
     if (m_itemsParametrizedList.contains(item))
     {
     	  QString filter = QString("%1 LIKE '%2' AND %3 LIKE '%4'").arg("ITEM",item,"USER_UID",m_userUid);
     	  m_tableNextDateModel->setFilter(filter);
     	  m_tableNextDateModel->select();
     	  QStringList listOfNextDateItems;
     	  for (int row = 0; row < m_tableNextDateModel->rowCount(); ++row)
     	  {
     	      QString d = m_tableNextDateModel->data(m_tableNextDateModel->index(row,PreventIO::ND_ITEM),Qt::DisplayRole).toString();
     	      listOfItems << d;
     	      }
     	   if (listOfItems.size() > 1)
     	   {
     	   	  NextDateDialog *dialog = new NextDateDialog(this,listOfItems,m_tableNextDateModel);
     	   	  if (dialog->exec()== QDialog::Accepted)
     	   	  {
     	   	  	  listOfNextDateItems = dialog->getListForChosenItem();
     	   	      }
     	       }
     	   else
     	   {
     	   	for (int col = 0; col < m_tableNextDateModel->columnCount() ; ++col)
     	   	{
     	            listOfNextDateItems << m_tableNextDateModel->data(m_tableNextDateModel->index(0,col),Qt::DisplayRole).toString();
     	   	    }
     	       }
     	  date = m_io->getNextDate(listOfNextDateItems,index);
         }
     else
     {
         date = index.model()->data(index,Qt::DisplayRole).toDate();
     	 }
     dateEdit->setDate(date);
 }
 
  void DateEditTreeViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QString value = dateEdit->date().toString("yyyy-MM-dd");

     model->setData(index, value, Qt::EditRole);
 }
 
 ////////////////////////////////////////////
 //COMBOBOX DELEGATE FOR ICONS OF TREEVIEW
 ///////////////////////////////////////////
 
 ComboTreeViewDelegate::ComboTreeViewDelegate(QObject * parent)
 {
     Q_UNUSED(parent);
     m_hashIcons.insert(PREVENT_OK,pixmaps()+"/preventOk.png");
     m_hashIcons.insert(PREVENT_WARNING,pixmaps()+"/preventWarning.png");
     m_hashIcons.insert(PREVENT_PAST,pixmaps()+"/past.png");
 }

ComboTreeViewDelegate::~ComboTreeViewDelegate(){}

QSize ComboTreeViewDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(40,25);
}

QWidget *ComboTreeViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
   if (  index.column() == PreventionEngine::IPreventionEngine::ICON_H )
    {
   	  QComboBox * combo = new QComboBox(parent);
    	  return combo;
        }
   return QStyledItemDelegate::createEditor(parent, option, index);
}

 void ComboTreeViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
     QComboBox * combo = static_cast<QComboBox*>(editor);
     combo->setEditable(false);
     for (int row = 0; row < Icons_MaxParam; ++row )
     {
     	  combo->addItem(QIcon(m_hashIcons.value(row)),"");
         }
     QVariant icon = index.model()->data(index,Qt::DisplayRole);
     combo->setCurrentIndex(m_hashIcons.key(icon.toString()));
  
      
 }
 
 void ComboTreeViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
 {
     QComboBox * combo = static_cast<QComboBox*>(editor);
     int row = combo->currentIndex();
     QVariant value ;
     switch(row){
         case PREVENT_OK :
             value = m_hashIcons.value(PREVENT_OK);
             break;
         case PREVENT_WARNING :
             value = m_hashIcons.value(PREVENT_WARNING);
             break;
         case PREVENT_PAST :
             value = m_hashIcons.value(PREVENT_PAST);
             break;
         default :
             break;    
         }
      model->setData(index, value, Qt::EditRole);
      
 }
 
//////////////////////////////////////////////////////
//// COMBODELEGATE FOR ITEMS OF TREEVIEW
/////////////////////////////////////////////////////

ComboTreeViewItemDelegate::ComboTreeViewItemDelegate(QObject * parent)
{
    Q_UNUSED(parent);
    QSqlDatabase db = QSqlDatabase::database("prevention");    
    const QString filterUserUuid; 
    m_modelNextDate = new QSqlTableModel(this,db);
    m_modelNextDate->setTable("nextdate");
    m_modelNextDate->setFilter(filterUserUuid);
    m_modelNextDate->select();
    for (int row = 0; row < m_modelNextDate->rowCount(); ++row)
    {
    	  QString item = m_modelNextDate->data(m_modelNextDate->index(row,PreventIO::ND_ITEM),Qt::DisplayRole).toString();
    	  int primkey = m_modelNextDate->data(m_modelNextDate->index(row,PreventIO::ID_NEXTDATE),Qt::DisplayRole).toInt();
          m_mapNextdateItems.insert(primkey,item);
        }
     m_rowAndItemModel = new QStandardItemModel(0,2);
}

ComboTreeViewItemDelegate::~ComboTreeViewItemDelegate(){}

QSize ComboTreeViewItemDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(100,25);
}

QWidget *ComboTreeViewItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
   if (  index.column() == PreventionEngine::IPreventionEngine::ITEM_H )
    {
   	  QComboBox * combo = new QComboBox(parent);
    	  return combo;
        }
   return QStyledItemDelegate::createEditor(parent, option, index);
}

 void ComboTreeViewItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
     QComboBox * combo = static_cast<QComboBox*>(editor);
     combo->setEditable(true);
     int row = 0;
     const QString itemOfModel = index.model()->data(index,Qt::DisplayRole).toString();
     m_rowAndItemModel->insertRows(0,1);
     m_rowAndItemModel->setData(m_rowAndItemModel->index(0,ROW),row,Qt::EditRole);
     m_rowAndItemModel->setData(m_rowAndItemModel->index(0,ITEM),itemOfModel,Qt::EditRole);
     QMapIterator<int,QString> it(m_mapNextdateItems);
     while(it.hasNext())
     {
    	  ++row;
    	  it.next();
     	  QString item = it.value();
     	  combo->addItem(item);
          m_rowAndItemModel->setData(m_rowAndItemModel->index(row,ROW),row,Qt::EditRole);
          m_rowAndItemModel->setData(m_rowAndItemModel->index(row,ITEM),item,Qt::EditRole);
         }
     combo->setModel(m_rowAndItemModel);
     combo->setModelColumn(ITEM);
     int newIndex = combo->findText(itemOfModel);
     if (!itemOfModel.isEmpty())
     {
     	  combo->setCurrentIndex(newIndex);
         }
     else
     {
     	combo->setCurrentIndex(0);
         }
     
 }
 
 void ComboTreeViewItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
 {
     QComboBox * combo = static_cast<QComboBox*>(editor);
     QString value = combo->currentText();
     model->setData(index, value, Qt::EditRole);
 }
 
