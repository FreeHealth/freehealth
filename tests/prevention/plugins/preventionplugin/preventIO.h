#ifndef PREVENTIO_H
#define PREVENTIO_H

#include <QtSql>
#include <QtCore>
#include <QAbstractItemModel>
#include <QDebug>

 class TreeItem:public QObject
 {
 Q_OBJECT
 public:
     TreeItem(const QVector<QVariant> &data, TreeItem *parent = 0);
     ~TreeItem();
     
     void appendChild(TreeItem *item);
     void fillParentsHash(const QString & parentName, TreeItem * item);
     TreeItem *child(int number);
     TreeItem *child(const QString & parentName);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;
     QVector<QVariant> datas(int columns);
     bool insertChildren(int position, int count, int columns);
     bool insertColumns(int position, int columns);
     TreeItem *parent();
     bool removeChildren(int position, int count);
     bool removeColumns(int position, int columns);
     int childNumber() const;
     bool setData(int column, const QVariant &value);

 private:
     //int getNextIdPrimkey();
     //int findNextId();
     //bool insertANewLineInModel();
     QList<TreeItem*> m_childItems;
     QHash<QString,TreeItem*> m_treeItemsHash;
     QVector<QVariant> m_itemData;
     TreeItem *m_parentItem;
 };

class VariantItemModel : public QAbstractItemModel
 {
     Q_OBJECT

  public:
      enum Parenthood {
          PARENT = 0,
          CHILD,
          Parenthood_MaxParam
          };
      enum Prevention_Items {
          ID_PREVENTION = 0,
          ITEM,
          TYPE_OF_ITEM,
          PARENT_ITEM,
          PARENT_OR_CHILD,
          ICON,
          DATE_DONE,
          DATE_NEXT,
          ABSTRACT,
          ID_ITEM,
          RESULT,
          Prevention_Items_MaxParam
          };
      enum Headers {
          ITEM_H = 0,
          TYPE_OF_ITEM_H,
          PARENT_ITEM_H,
          PARENT_OR_CHILD_H,
          ICON_H,
          DATE_DONE_H,
          DATE_NEXT_H,
          ABSTRACT_H,
          ID_ITEM_H,
          RESULT_H,
          Headers_MaxParam
          };
      enum Types_Of_Items {
          PRIMARY_PREVENTION_ITEM = 0,
          SECONDARY_PREVENTION_ITEM,
          Types_Of_Items_MaxParam
          };
      enum Icons {
          PREVENT_OK = 0,
          PREVENT_WARNING,
          PREVENT_PAST,
          Icons_MaxParam
          };
     VariantItemModel(const QStringList headers = QStringList(), QSqlTableModel *model = 0,
               QObject *parent = 0);
     ~VariantItemModel();

     QVariant data(const QModelIndex &index, int role) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;

     QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;
     
     QHash<int,QVariant> childs(QModelIndex &parent);

     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;

     Qt::ItemFlags flags(const QModelIndex &index) const;
     bool setData(const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole);
     bool setHeaderData(int section, Qt::Orientation orientation,
                        const QVariant &value, int role = Qt::EditRole);

     bool insertColumns(int position, int columns,
                        const QModelIndex &parent = QModelIndex());
     bool removeColumns(int position, int columns,
                        const QModelIndex &parent = QModelIndex());
     bool insertRows(int position, int rows,
                     const QModelIndex &parent = QModelIndex());
     bool removeRows(int position, int rows,
                     const QModelIndex &parent = QModelIndex());
     
     //get datas
     QStringList getListOfMainItems();
     bool clear();
     TreeItem *getRootItem();
     void setupModelData(QSqlTableModel *model, TreeItem *parent);
     void setRootItem (const QStringList headers);
     TreeItem *getItem(const QModelIndex &index) const;
     int findNextId();
     int getNextIdPrimkey();
 private:
     //TreeItem *getItem(const QModelIndex &index) const;
     int getSqlTableRow(int idOfItem);
     TreeItem *m_rootItem;
     QSqlTableModel *m_modelSql;
     QSqlDatabase m_db;
 };

class PreventIO : public QObject {
    Q_OBJECT
    public:
        PreventIO(QObject * object = 0);
        ~PreventIO();
        QSqlDatabase getDatabase();
        QString correct(QVariant variant);
        const QStringList setHeadersDatas();
        bool addAnItemAccordingToIndex(QModelIndex & index,QModelIndex & parent, QObject * parentObject);
        QSqlTableModel * getModel();
        QSqlDatabase m_db;
        VariantItemModel *m_variantModel;
    private:
        QSqlTableModel * m_model;
    
};

#endif

