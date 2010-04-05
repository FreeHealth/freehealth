/****************************************************************************
** Meta object code from reading C++ file 'usermodel.h'
**
** Created: Mon Feb 1 11:54:52 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/usermanagerplugin/usermodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'usermodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserPlugin__UserModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      49,   44,   22,   22, 0x05,
      77,   44,   22,   22, 0x05,
     100,   44,   22,   22, 0x05,
     131,   44,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     162,   22,  157,   22, 0x0a,
     174,   44,  157,   22, 0x0a,
     198,  194,  157,   22, 0x0a,
     213,   22,  157,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UserPlugin__UserModel[] = {
    "UserPlugin::UserModel\0\0memoryUsageChanged()\0"
    "uuid\0userAboutToConnect(QString)\0"
    "userConnected(QString)\0"
    "userAboutToDisconnect(QString)\0"
    "userDisconnected(QString)\0bool\0"
    "submitAll()\0submitUser(QString)\0row\0"
    "submitRow(int)\0revertAll()\0"
};

const QMetaObject UserPlugin::UserModel::staticMetaObject = {
    { &QSqlTableModel::staticMetaObject, qt_meta_stringdata_UserPlugin__UserModel,
      qt_meta_data_UserPlugin__UserModel, 0 }
};

const QMetaObject *UserPlugin::UserModel::metaObject() const
{
    return &staticMetaObject;
}

void *UserPlugin::UserModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserPlugin__UserModel))
        return static_cast<void*>(const_cast< UserModel*>(this));
    return QSqlTableModel::qt_metacast(_clname);
}

int UserPlugin::UserModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSqlTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: memoryUsageChanged(); break;
        case 1: userAboutToConnect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: userConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: userAboutToDisconnect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: userDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: { bool _r = submitAll();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { bool _r = submitUser((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: { bool _r = submitRow((*reinterpret_cast< const int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: { bool _r = revertAll();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void UserPlugin::UserModel::memoryUsageChanged()const
{
    QMetaObject::activate(const_cast< UserPlugin::UserModel *>(this), &staticMetaObject, 0, 0);
}

// SIGNAL 1
void UserPlugin::UserModel::userAboutToConnect(const QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< UserPlugin::UserModel *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void UserPlugin::UserModel::userConnected(const QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< UserPlugin::UserModel *>(this), &staticMetaObject, 2, _a);
}

// SIGNAL 3
void UserPlugin::UserModel::userAboutToDisconnect(const QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< UserPlugin::UserModel *>(this), &staticMetaObject, 3, _a);
}

// SIGNAL 4
void UserPlugin::UserModel::userDisconnected(const QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< UserPlugin::UserModel *>(this), &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
