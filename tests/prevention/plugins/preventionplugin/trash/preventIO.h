#ifndef PREVENTIO_H
#define PREVENTIO_H

#include <QtSql>
#include <QtCore>
#include <QDebug>

class VariantItemModel{
    Q_OBJECT
        public:
        enum ColumnRepresentation {
            COL_ITEM = 0,
            COL_MaxParam
        };

        VariantItemModel(QObject * parent);
        ~VariantItemModel();
        
        int rowCount(const QModelIndex &parent ) const ;
            
        int columnCount(const QModelIndex &parent) const ;
        
        QModelIndex index ( int row, int column, const QModelIndex & parent  ) const ;
        
        QModelIndex parent (const QModelIndex & child ) const;
        
        bool insertRows( int position, int count, const QModelIndex & parent  );
        
        bool removeRows(int position, int count, const QModelIndex & parent );
        
        bool submit();


        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        
        bool setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );

        //QSqlError lastError();
        
        Qt::ItemFlags flags(const QModelIndex &index) const;
        private:
            QVector<QList<QVariant> > * m_listsOfValuesbyRows;
};

class PreventIO : public QObject {
    Q_OBJECT
    public:
        PreventIO(QObject * object = 0);
        ~PreventIO();
    VariantItemModel * getVariantItemModel();
    
};

#endif

