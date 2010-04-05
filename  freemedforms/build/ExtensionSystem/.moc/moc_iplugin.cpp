/****************************************************************************
** Meta object code from reading C++ file 'iplugin.h'
**
** Created: Sun Sep 13 10:27:14 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/extensionsystem/iplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'iplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExtensionSystem__IPlugin[] = {

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

static const char qt_meta_stringdata_ExtensionSystem__IPlugin[] = {
    "ExtensionSystem::IPlugin\0"
};

const QMetaObject ExtensionSystem::IPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ExtensionSystem__IPlugin,
      qt_meta_data_ExtensionSystem__IPlugin, 0 }
};

const QMetaObject *ExtensionSystem::IPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *ExtensionSystem::IPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExtensionSystem__IPlugin))
        return static_cast<void*>(const_cast< IPlugin*>(this));
    return QObject::qt_metacast(_clname);
}

int ExtensionSystem::IPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
