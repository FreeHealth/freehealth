#ifndef DATAPACKMODEL_H
#define DATAPACKMODEL_H

#include <accountbaseplugin/accountbase_exporter.h>
#include <utils/database.h>
#include <QAbstractTableModel>
#include <QSqlError>

namespace AccountDB {

class DatapackBase:public QObject,public Utils::Database
{
    Q_OBJECT
    public :
        DatapackBase(QObject * parent);
        ~DatapackBase();
        static DatapackBase *instance();
        
        bool initialize();
        
    private:
        static DatapackBase *m_Instance;
        bool _init;
};


namespace Internal {
class DatapackMPModelPrivate;
}

class ACCOUNTBASE_EXPORT DatapackMPModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DatapackMPModel(QObject *parent);
    ~DatapackMPModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void setFilter(const QString & filter);
    QString filter();
    void setUserUuid(const QString &uuid);
    QSqlError lastError();
    bool isDirty() const;

public Q_SLOTS:
    bool submit();
    void revert();

private:
    Internal::DatapackMPModelPrivate *d;
    
};

}  // End namespace AccountDB


#endif

