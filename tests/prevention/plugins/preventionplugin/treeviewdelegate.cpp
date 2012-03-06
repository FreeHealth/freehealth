#include "treeviewdelegate.h"
#include "preventIO.h"

TreeViewDelegate::TreeViewDelegate(){}

TreeViewDelegate::~TreeViewDelegate(){}

QSize TreeViewDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
   
    return QSize(40,25);

}

/*void TreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const{}*/

QWidget *TreeViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (  index.column() == VariantItemModel::DATE_DONE_H || index.column() == VariantItemModel::DATE_NEXT_H)
    {
    	  qDebug() << __FILE__ << QString::number(__LINE__) << " index.column() =" << QString::number(index.column()) ;
    	  QDateEdit * dateEdit = new QDateEdit(parent);
    	  dateEdit->setDisplayFormat("yyyy-MM-dd");
    	  return dateEdit;
        }
          return QStyledItemDelegate::createEditor(parent, option, index);

     
}

 void TreeViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QDate date = index.model()->data(index,Qt::EditRole).toDate();
     dateEdit->setDate(date);
 }
 
  void TreeViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QString value = dateEdit->date().toString("yyyy-MM-dd");

     model->setData(index, value, Qt::EditRole);
 }

