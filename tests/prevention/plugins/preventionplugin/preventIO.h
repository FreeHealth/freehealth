#ifndef PREVENTIO_H
#define PREVENTIO_H

#include "IPrevention.h"

#include <QtSql>
#include <QtCore>
#include <QAbstractItemModel>
#include <QDebug>
#include <QTextDocument>

using namespace PreventionEngine; // TODO remove using directive inside header file

 class TreeItem : public IPreventionEngine
 {
 Q_OBJECT

 public:
     TreeItem(const QVector<QVariant> &data = QVector<QVariant>(), TreeItem *parent = 0);
     ~TreeItem();
     
     void appendChild(TreeItem *item);//new
     void fillParentsHash(const QString & parentName, TreeItem * item);//new
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
     QHash<QString,TreeItem*> treeParentItems();
     QList<TreeItem*> m_childItems;
     QHash<QString,TreeItem*> m_treeItemsHash;
     QVector<QVariant> m_itemData;
     TreeItem *m_parentItem;
     int m_parenthood;
 };

class VariantItemModel : public QAbstractItemModel
 {
     Q_OBJECT

  public:

     VariantItemModel(QSqlTableModel *model = 0, QObject *parent = 0);
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
     void setupModelData(QSqlTableModel *model = 0, TreeItem *parent = 0);
     void setRootItem (const QStringList headers);
     TreeItem *getItem(const QModelIndex &index) const;
     int findNextId();
     int getNextIdPrimkey();
     //change icon
     void setIconWarning(QVariant & idItem);
     //add and delete rows     
     bool addAnItemAccordingToIndex(QModelIndex & index,QModelIndex & parent, QObject * parentObject);
     bool addAGroupItem(QModelIndex & index,QModelIndex & parent,QObject * parentObject);
     bool deleteItemAccordingToIndex(QModelIndex & index,QModelIndex & parent, QObject * parentObject);
     bool deleteGroupOfItems(QModelIndex & index);
     //get childsAndItems
     QMap<int,QVector<QVariant> > childsAndItems(QModelIndex & parent);
 private:
     int getSqlTableRow(int idOfItem);
     TreeItem * findTreeItemWithIdItem(QVariant & idItem);
     QModelIndex indexOfIconOfItem(TreeItem * item);
     TreeItem *m_rootItem;
     QSqlTableModel *m_modelSql;
     QSqlDatabase m_db;
 };

class PreventIO : public QObject {
    Q_OBJECT
    enum Uuid_List {
      PATIENT = 0,
      USER
      };
    public:
        enum NextDateItems {
            ID_NEXTDATE = 0,
            ND_ITEM,
            ND_YEAR,
            ND_MONTH,
            ND_DAY,
            ND_HOURS,
            ND_MIN,
            ND_USER_UID,
            NextDateItems_MaxParam
            };
        PreventIO(QObject * object = 0);
        ~PreventIO();
        QSqlDatabase getDatabase();
        QString correct(QVariant variant);
        //bool addAnItemAccordingToIndex(QModelIndex & index,QModelIndex & parent, QObject * parentObject);
        //bool addAGroupItem(QModelIndex & index,QModelIndex & parent,QObject * parentObject);
        QSqlTableModel * getModel();
        VariantItemModel *getVariantItemModel();
        QSqlTableModel * getNextDateModel();
        QStringList getListOfNextDateItems();
        QString getUserUid();
        QDate getNextDate(const QStringList & listOfDatas, QModelIndex index);
        bool recordDocument(const QTextDocument *document, const QString & id);
        QString getDocumentRelativeToIdItem(const QString & id_item);
        QString getIdItem(const QModelIndex & indexInRow);
        QSqlDatabase m_db;
        VariantItemModel *m_variantModel;
        QSqlTableModel *m_NextDateModel;
        QString m_userUid;
    private:
        QSqlTableModel * m_model;
    
};

#endif

