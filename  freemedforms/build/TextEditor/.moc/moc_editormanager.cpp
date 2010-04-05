/****************************************************************************
** Meta object code from reading C++ file 'editormanager.h'
**
** Created: Mon Feb 1 11:53:31 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/texteditorplugin/editormanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editormanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Editor__Internal__EditorManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      41,   33,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Editor__Internal__EditorManager[] = {
    "Editor::Internal::EditorManager\0\0"
    "context\0updateContext(Core::IContext*)\0"
};

const QMetaObject Editor::Internal::EditorManager::staticMetaObject = {
    { &EditorActionHandler::staticMetaObject, qt_meta_stringdata_Editor__Internal__EditorManager,
      qt_meta_data_Editor__Internal__EditorManager, 0 }
};

const QMetaObject *Editor::Internal::EditorManager::metaObject() const
{
    return &staticMetaObject;
}

void *Editor::Internal::EditorManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__Internal__EditorManager))
        return static_cast<void*>(const_cast< EditorManager*>(this));
    return EditorActionHandler::qt_metacast(_clname);
}

int Editor::Internal::EditorManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EditorActionHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateContext((*reinterpret_cast< Core::IContext*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
