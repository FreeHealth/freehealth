#include "girdatafactory.h"

#include <formmanagerplugin/iformitem.h>
#include <medicalutils/aggir/girscore.h>

#include <QDebug>

using namespace Gir;

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Factory -------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//GirDataFactory::GirDataFactory(QObject *parent) :
//        Form::IFormItemDataFactory(parent)
//{
//}
//
//Form::IFormItemData *GirDataFactory::createItemData(Form::FormItem *parent)
//{
////    if ((name.compare("aggir",Qt::CaseInsensitive)==0) ||
////        (name.compare("gir",Qt::CaseInsensitive)==0)) {
////        return 0;//new GirWidget(object, parent);
////    }
//    return 0;
//}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- ItemData -------------------------------------------------
//--------------------------------------------------------------------------------------------------------
bool GirItemData::isModified() const
{
    return m_IsModified;
}

void GirItemData::setData(const QVariant &data, const int role)
{
}

QVariant GirItemData::data(const int role) const
{
    return QVariant();
}

void GirItemData::setStorableData(const QVariant &data)
{
}

QVariant GirItemData::storableData() const
{
    return QVariant();
}

