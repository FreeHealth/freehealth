#include "girdatafactory.h"

#include <coreplugin/iformitem.h>
#include <medicalutils/aggir/girscore.h>

#include <QDebug>

using namespace Gir;

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Factory -------------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirDataFactory::GirDataFactory(QObject *parent) :
        Core::IFormItemDataFactory(parent)
{
}

Core::IFormItemData *GirDataFactory::createItemData(Core::FormItem *parent)
{
//    if ((name.compare("aggir",Qt::CaseInsensitive)==0) ||
//        (name.compare("gir",Qt::CaseInsensitive)==0)) {
//        return 0;//new GirWidget(object, parent);
//    }
    return 0;
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- ItemData -------------------------------------------------
//--------------------------------------------------------------------------------------------------------
bool GirItemData::isModified() const
{
}

void GirItemData::setData(const QVariant &data)
{
}

QVariant GirItemData::data() const
{
}

void GirItemData::setWidgetData(const QVariant &data)
{
}

QVariant GirItemData::widgetData() const
{
}

void GirItemData::setStorableData(const QVariant &data)
{
}

QVariant GirItemData::storableData() const
{
}

void GirItemData::setScriptData(const QVariant &data)
{
}

QVariant GirItemData::scriptData() const
{
}

