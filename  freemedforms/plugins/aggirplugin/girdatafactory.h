#ifndef GIRDATAFACTORY_H
#define GIRDATAFACTORY_H

#include <coreplugin/iformitemdatafactory.h>

#include <medicalutils/aggir/girscore.h>

#include <QtGui/QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

namespace Gir {

class GirDataFactory : public Core::IFormItemDataFactory
{
    Q_OBJECT
public:
    GirDataFactory(QObject *parent = 0);
    ~GirDataFactory() {}

    bool initialize(const QStringList &, QString *) {return true;}
    bool extensionInitialized() {return true;}
    bool isInitialized() const {return true;}

    QStringList providedItemDatas() const { return QStringList() << "gir" << "aggir"; }
    Core::IFormItemData *createItemData(Core::FormItem *parent);
};


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- GirWidget implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class CORE_EXPORT GirItemData : public Core::IFormItemData
{
    Q_OBJECT
    friend class GirDataFactory;

public:
    virtual ~GirItemData() {}
    virtual Core::FormItem *parentItem() const
    {
        Q_ASSERT(qobject_cast<Core::FormItem*>(parent()));
        return qobject_cast<Core::FormItem*>(parent());
    }

    bool isModified() const;

    virtual void setData(const QVariant &data);
    virtual QVariant data() const;

    virtual void setWidgetData(const QVariant &data);
    virtual QVariant widgetData() const;

    virtual void setStorableData(const QVariant &data);
    virtual QVariant storableData() const;

    virtual void setScriptData(const QVariant &data);
    virtual QVariant scriptData() const;

protected:
    explicit GirItemData(Core::FormItem *parent) : Core::IFormItemData(parent) {}

private:
    bool m_IsModified;
    MedicalUtils::AGGIR::GirScore m_Score;
};

} // End Gir

#endif // GIRDATAFACTORY_H
