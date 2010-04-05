/****************************************************************************
** Meta object code from reading C++ file 'command_p.h'
**
** Created: Tue Feb 9 09:15:40 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/actionmanager/command_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'command_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__Internal__CommandPrivate[] = {

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

static const char qt_meta_stringdata_Core__Internal__CommandPrivate[] = {
    "Core::Internal::CommandPrivate\0"
};

const QMetaObject Core::Internal::CommandPrivate::staticMetaObject = {
    { &Core::Command::staticMetaObject, qt_meta_stringdata_Core__Internal__CommandPrivate,
      qt_meta_data_Core__Internal__CommandPrivate, 0 }
};

const QMetaObject *Core::Internal::CommandPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::CommandPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__CommandPrivate))
        return static_cast<void*>(const_cast< CommandPrivate*>(this));
    typedef Core::Command QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Core::Internal::CommandPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::Command QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Core__Internal__Shortcut[] = {

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

static const char qt_meta_stringdata_Core__Internal__Shortcut[] = {
    "Core::Internal::Shortcut\0"
};

const QMetaObject Core::Internal::Shortcut::staticMetaObject = {
    { &CommandPrivate::staticMetaObject, qt_meta_stringdata_Core__Internal__Shortcut,
      qt_meta_data_Core__Internal__Shortcut, 0 }
};

const QMetaObject *Core::Internal::Shortcut::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::Shortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__Shortcut))
        return static_cast<void*>(const_cast< Shortcut*>(this));
    return CommandPrivate::qt_metacast(_clname);
}

int Core::Internal::Shortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandPrivate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Core__Internal__Action[] = {

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

static const char qt_meta_stringdata_Core__Internal__Action[] = {
    "Core::Internal::Action\0"
};

const QMetaObject Core::Internal::Action::staticMetaObject = {
    { &CommandPrivate::staticMetaObject, qt_meta_stringdata_Core__Internal__Action,
      qt_meta_data_Core__Internal__Action, 0 }
};

const QMetaObject *Core::Internal::Action::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::Action::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__Action))
        return static_cast<void*>(const_cast< Action*>(this));
    return CommandPrivate::qt_metacast(_clname);
}

int Core::Internal::Action::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandPrivate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Core__Internal__OverrideableAction[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      36,   35,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Core__Internal__OverrideableAction[] = {
    "Core::Internal::OverrideableAction\0\0"
    "actionChanged()\0"
};

const QMetaObject Core::Internal::OverrideableAction::staticMetaObject = {
    { &Action::staticMetaObject, qt_meta_stringdata_Core__Internal__OverrideableAction,
      qt_meta_data_Core__Internal__OverrideableAction, 0 }
};

const QMetaObject *Core::Internal::OverrideableAction::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::OverrideableAction::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__OverrideableAction))
        return static_cast<void*>(const_cast< OverrideableAction*>(this));
    return Action::qt_metacast(_clname);
}

int Core::Internal::OverrideableAction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Action::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: actionChanged(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
