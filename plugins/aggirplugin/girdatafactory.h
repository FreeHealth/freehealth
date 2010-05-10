#ifndef GIRDATAFACTORY_H
#define GIRDATAFACTORY_H

#include <aggirplugin/aggir_exporter.h>

#include <formmanagerplugin/iformitemdatafactory.h>

#include <medicalutils/aggir/girscore.h>

#include <QtGui/QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

namespace Gir {

//class GirDataFactory : public Form::IFormItemDataFactory
//{
//    Q_OBJECT
//public:
//    GirDataFactory(QObject *parent = 0);
//    ~GirDataFactory() {}
//
//    bool initialize(const QStringList &, QString *) {return true;}
//    bool extensionInitialized() {return true;}
//    bool isInitialized() const {return true;}
//
//    QStringList providedItemDatas() const { return QStringList() << "gir" << "aggir"; }
//    Form::IFormItemData *createItemData(Form::FormItem *parent);
//};


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- GirWidget implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class AGGIR_EXPORT GirItemData : public Form::IFormItemData
{
public:
    explicit GirItemData(Form::FormItem *parent) : Form::IFormItemData(parent) {}
    virtual ~GirItemData() {}
    virtual Form::FormItem *parentItem() const
    {
        Q_ASSERT(qobject_cast<Form::FormItem*>(parent()));
        return qobject_cast<Form::FormItem*>(parent());
    }

    bool isModified() const;

    virtual void setData(const QVariant &data, const int role);
    virtual QVariant data(const int role) const;

    virtual void setStorableData(const QVariant &data);
    virtual QVariant storableData() const;

private:
    bool m_IsModified;
    MedicalUtils::AGGIR::GirScore m_Score;
};

} // End Gir

#endif // GIRDATAFACTORY_H
