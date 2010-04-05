#ifndef IFORMITEMDATAFACTORY_H
#define IFORMITEMDATAFACTORY_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/iformitem.h>

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Core {
class IFormItemData;
class FormItem;

class CORE_EXPORT IFormItemDataFactory : public QObject
{
    Q_OBJECT
public:
    IFormItemDataFactory(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFormItemDataFactory() {}

    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    virtual bool extensionInitialized() = 0;
    virtual bool isInitialized() const = 0;

    virtual QStringList providedItemDatas() const = 0;
    virtual Core::IFormItemData *createItemData(Core::FormItem *parent) = 0;
};


class CORE_EXPORT IFormItemData : public QObject
{
    Q_OBJECT
    friend class IFormItemDataFactory;

public:
    virtual ~IFormItemData() {}
    inline static int columnCount() { return 4; } // to be improved+++

    virtual Core::FormItem *parentItem() const = 0;
    virtual bool isModified() const = 0;

    virtual void setData(const QVariant &data) = 0;
    virtual QVariant data() const = 0;

    virtual void setWidgetData(const QVariant &data) = 0;
    virtual QVariant widgetData() const = 0;

    virtual void setStorableData(const QVariant &data) = 0;
    virtual QVariant storableData() const = 0;

    virtual void setScriptData(const QVariant &data) = 0;
    virtual QVariant scriptData() const = 0;

protected:
    explicit IFormItemData(Core::FormItem *parent) : QObject(parent) {}
};
} // namespace Core

Q_DECLARE_METATYPE(Core::IFormItemData*);

#endif // IFORMITEMDATAFACTORY_H
