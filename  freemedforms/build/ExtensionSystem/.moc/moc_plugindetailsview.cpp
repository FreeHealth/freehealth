/****************************************************************************
** Meta object code from reading C++ file 'plugindetailsview.h'
**
** Created: Sun Sep 13 10:27:13 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/extensionsystem/plugindetailsview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plugindetailsview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExtensionSystem__PluginDetailsView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_ExtensionSystem__PluginDetailsView[] = {
    "ExtensionSystem::PluginDetailsView\0"
};

const QMetaObject ExtensionSystem::PluginDetailsView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ExtensionSystem__PluginDetailsView,
      qt_meta_data_ExtensionSystem__PluginDetailsView, 0 }
};

const QMetaObject *ExtensionSystem::PluginDetailsView::metaObject() const
{
    return &staticMetaObject;
}

void *ExtensionSystem::PluginDetailsView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExtensionSystem__PluginDetailsView))
        return static_cast<void*>(const_cast< PluginDetailsView*>(this));
    return QWidget::qt_metacast(_clname);
}

int ExtensionSystem::PluginDetailsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
