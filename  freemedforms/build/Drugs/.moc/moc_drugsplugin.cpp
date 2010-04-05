/****************************************************************************
** Meta object code from reading C++ file 'drugsplugin.h'
**
** Created: Mon Feb 1 19:13:11 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugsplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'drugsplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DrugsPlugin[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      36,   35,   35,   35, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsPlugin[] = {
    "DrugsWidget::Internal::DrugsPlugin\0\0"
    "remoteArgument(QString)\0"
};

const QMetaObject DrugsWidget::Internal::DrugsPlugin::staticMetaObject = {
    { &ExtensionSystem::IPlugin::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsPlugin,
      qt_meta_data_DrugsWidget__Internal__DrugsPlugin, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsPlugin))
        return static_cast<void*>(const_cast< DrugsPlugin*>(this));
    typedef ExtensionSystem::IPlugin QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef ExtensionSystem::IPlugin QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: remoteArgument((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
