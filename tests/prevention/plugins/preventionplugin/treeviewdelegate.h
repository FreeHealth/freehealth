#ifndef TREEVIEWDELEGATE
#define TREEVIEWDELEGATE

#include <QStyledItemDelegate>
#include <QDateEdit>
#include <QWidget>


class TreeViewDelegate : public QStyledItemDelegate{
    Q_OBJECT
    public :
    TreeViewDelegate();
    ~TreeViewDelegate();


    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};



#endif

