#ifndef TREEVIEWDELEGATE
#define TREEVIEWDELEGATE

#include <QStyledItemDelegate>
#include <QDateEdit>
#include <QWidget>
#include <QHash>

class DateEditTreeViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public :
    DateEditTreeViewDelegate(QObject * parent = 0);
    ~DateEditTreeViewDelegate();


    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class ComboTreeViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    enum Icons {//see enum in preventIO
          PREVENT_OK = 0,
          PREVENT_WARNING,
          PREVENT_PAST,
          Icons_MaxParam
          };
    public :
    ComboTreeViewDelegate(QObject * parent = 0);
    ~ComboTreeViewDelegate();


    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    private :
        QHash<int,QString> m_hashIcons;
};

#endif

