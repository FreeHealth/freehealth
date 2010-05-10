#ifndef IFORMITEMDATAFACTORY_H
#define IFORMITEMDATAFACTORY_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <formmanagerplugin/iformitem.h>

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Form {
class FormItem;

class FORM_EXPORT IFormItemData //: public QObject
{
//    Q_OBJECT
public:
//    explicit IFormItemData(Form::FormItem *parent) : QObject(parent) {}
    IFormItemData() {}
    virtual ~IFormItemData() {}

//    virtual void clear() = 0;

    virtual Form::FormItem *parentItem() const = 0;
    virtual bool isModified() const = 0;

    virtual void setData(const QVariant &data, const int role) = 0;
    virtual QVariant data(const int role) const = 0;

    virtual void setStorableData(const QVariant &data) = 0;
    virtual QVariant storableData() const = 0;
};
} // namespace Form

Q_DECLARE_METATYPE(Form::IFormItemData*);

#endif // IFORMITEMDATAFACTORY_H
