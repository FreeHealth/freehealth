/****************************************************************************
** Meta object code from reading C++ file 'icore.h'
**
** Created: Mon Feb 1 11:52:49 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/icore.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'icore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__ICore[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      31,   12,   12,   12, 0x05,
      44,   12,   12,   12, 0x05,
      68,   12,   12,   12, 0x05,
      93,   12,   12,   12, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Core__ICore[] = {
    "Core::ICore\0\0coreAboutToOpen()\0"
    "coreOpened()\0saveSettingsRequested()\0"
    "optionsDialogRequested()\0coreAboutToClose()\0"
};

const QMetaObject Core::ICore::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Core__ICore,
      qt_meta_data_Core__ICore, 0 }
};

const QMetaObject *Core::ICore::metaObject() const
{
    return &staticMetaObject;
}

void *Core::ICore::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__ICore))
        return static_cast<void*>(const_cast< ICore*>(this));
    return QObject::qt_metacast(_clname);
}

int Core::ICore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: coreAboutToOpen(); break;
        case 1: coreOpened(); break;
        case 2: saveSettingsRequested(); break;
        case 3: optionsDialogRequested(); break;
        case 4: coreAboutToClose(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Core::ICore::coreAboutToOpen()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Core::ICore::coreOpened()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Core::ICore::saveSettingsRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Core::ICore::optionsDialogRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Core::ICore::coreAboutToClose()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
