#ifndef FORMGLOBALMODEL_H
#define FORMGLOBALMODEL_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/iformitemdatafactory.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QAbstractItemModel>

namespace Core {
class FakeDataFactory;

class FakeData : public Core::IFormItemData
{
    friend class Core::FakeDataFactory;
public:
    virtual ~FakeData() {}

    virtual Core::FormItem *parentItem() const
    {
        Q_ASSERT(qobject_cast<Core::FormItem*>(parent()));
        return qobject_cast<Core::FormItem*>(parent());
    }

    virtual bool isModified() const {return false;}

    virtual void setData(const QVariant &data)  { m_data = data;}
    virtual QVariant data() const { return m_data; }

    virtual void setWidgetData(const QVariant &data) { m_data = data;}
    virtual QVariant widgetData() const {return m_data;}

    virtual void setStorableData(const QVariant &data) {}
    virtual QVariant storableData() const { return QVariant(); }

    virtual void setScriptData(const QVariant &data) {}
    virtual QVariant scriptData() const { return QVariant(); }

protected:
    explicit FakeData(Core::FormItem *parent) : IFormItemData(parent) {}

private:
    QVariant m_data;
};

class FakeDataFactory : public Core::IFormItemDataFactory
{
    Q_OBJECT
public:
    FakeDataFactory(QObject *parent = 0) : IFormItemDataFactory(parent) {}
    virtual ~FakeDataFactory() {}

    virtual bool initialize(const QStringList &arguments, QString *errorString) { return true; }
    virtual bool extensionInitialized() { return true; }
    virtual bool isInitialized() const { return true; }

    virtual QStringList providedItemDatas() const { return QStringList() << "fake"; }
    virtual Core::FakeData *createItemData(Core::FormItem *parent) { return new FakeData(parent); }
};

class CORE_EXPORT FormGlobalModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    FormGlobalModel(QObject *parent = 0);
    virtual ~FormGlobalModel();

//    virtual bool initialize(const QStringList &arguments, QString *errorString);
//    virtual bool extensionInitialized();

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

//    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex & index) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex & index) const;

//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

//    bool insertColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool insertColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool insertRow(int row, const QModelIndex & parent = QModelIndex());
//    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());

//    bool removeColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool removeColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool removeRow(int row, const QModelIndex & parent = QModelIndex());
//    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

//    virtual FormIdentifiantModel *createIdentifiantModel();
private:
    FakeDataFactory *plugs;
    Core::FormItem *root;
};

//class CORE_EXPORT FormIdentifiantModel : public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    FormIdentifiantModel(QObject *parent = 0) : QAbstractItemModel(parent) {}
//    virtual ~FormIdentifiantModel() {}
//
//    int columnCount(const QModelIndex & parent = QModelIndex()) const;
//    int rowCount(const QModelIndex & parent = QModelIndex()) const;
//
//    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
//    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
//
//    Qt::ItemFlags flags(const QModelIndex & index) const;
//    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
//    QModelIndex parent(const QModelIndex & index) const;
//
//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
//
//    bool insertColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool insertColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool insertRow(int row, const QModelIndex & parent = QModelIndex());
//    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
//
//    bool removeColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool removeColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool removeRow(int row, const QModelIndex & parent = QModelIndex());
//    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
//
////    virtual FormRecordModel *createRecordModel();
//
//};

//class CORE_EXPORT FormRecordModel : public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    FormRecordModel(QObject *parent = 0) : QObject(parent) {}
//    virtual ~FormRecordModel() {}
//
//
////    virtual FormItemModel *createItemModel();
//
//};

//class CORE_EXPORT FormItemModel : public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    FormItemModel(QObject *parent = 0) : QObject(parent) {}
//    virtual ~FormItemModel() {}
//
//
//};

} // namespace Core

#endif // FORMGLOBALMODEL_H
