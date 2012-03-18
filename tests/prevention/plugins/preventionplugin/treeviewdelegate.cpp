#include "treeviewdelegate.h"
#include "preventIO.h"

static inline QString pixmaps() 
{ 
    return QString(qApp->applicationDirPath()+"/../../global_resources/pixmap");
};

 ////////////////////////////////////////////
 //DATEEDIT DELEGATE FOR TREEVIEW
 ///////////////////////////////////////////

DateEditTreeViewDelegate::DateEditTreeViewDelegate(){}

DateEditTreeViewDelegate::~DateEditTreeViewDelegate(){}

QSize DateEditTreeViewDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
   
    return QSize(40,25);

}

QWidget *DateEditTreeViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (  index.column() == VariantItemModel::DATE_DONE_H || index.column() == VariantItemModel::DATE_NEXT_H)
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
     QDate date = index.model()->data(index,Qt::EditRole).toDate();
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
 //COMBOBOX DELEGATE FOR TREEVIEW
 ///////////////////////////////////////////
 
 ComboTreeViewDelegate::ComboTreeViewDelegate()
 {
     m_hashIcons.insert(PREVENT_OK,pixmaps()+"/preventOk.png");
     m_hashIcons.insert(PREVENT_WARNING,pixmaps()+"/preventWarning.png");
     m_hashIcons.insert(PREVENT_PAST,pixmaps()+"/past.png");
 }

ComboTreeViewDelegate::~ComboTreeViewDelegate(){}

QSize ComboTreeViewDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
   
    return QSize(40,25);

}

QWidget *ComboTreeViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
   if (  index.column() == VariantItemModel::ICON_H )
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
 

