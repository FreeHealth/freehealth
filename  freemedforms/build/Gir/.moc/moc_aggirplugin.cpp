/****************************************************************************
** Meta object code from reading C++ file 'aggirplugin.h'
**
** Created: Mon Feb 1 19:09:59 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/aggirplugin/aggirplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'aggirplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Gir__GirPlugin[] = {

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

static const char qt_meta_stringdata_Gir__GirPlugin[] = {
    "Gir::GirPlugin\0"
};

const QMetaObject Gir::GirPlugin::staticMetaObject = {
    { &ExtensionSystem::IPlugin::staticMetaObject, qt_meta_stringdata_Gir__GirPlugin,
      qt_meta_data_Gir__GirPlugin, 0 }
};

const QMetaObject *Gir::GirPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *Gir::GirPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Gir__GirPlugin))
        return static_cast<void*>(const_cast< GirPlugin*>(this));
    typedef ExtensionSystem::IPlugin QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Gir::GirPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef ExtensionSystem::IPlugin QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
