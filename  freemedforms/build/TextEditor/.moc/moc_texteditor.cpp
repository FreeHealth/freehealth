/****************************************************************************
** Meta object code from reading C++ file 'texteditor.h'
**
** Created: Mon Feb 1 11:53:35 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/texteditorplugin/texteditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'texteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Editor__TextEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       1,   72, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   20,   19,   19, 0x0a,
      46,   19,   19,   19, 0x09,
      57,   19,   19,   19, 0x09,
      66,   19,   19,   19, 0x09,
      79,   19,   19,   19, 0x09,
     101,   93,   19,   19, 0x09,
     116,   93,   19,   19, 0x09,
     136,   93,   19,   19, 0x09,
     153,   93,   19,   19, 0x09,
     170,   19,   19,   19, 0x09,
     182,   19,   19,   19, 0x09,
     199,  195,   19,   19, 0x09,

 // properties: name, type, flags
     227,  219, 0x0a195103,

       0        // eod
};

static const char qt_meta_stringdata_Editor__TextEditor[] = {
    "Editor::TextEditor\0\0state\0toogleToolbar(bool)\0"
    "fileOpen()\0saveAs()\0fontBigger()\0"
    "fontSmaller()\0checked\0textBold(bool)\0"
    "textUnderline(bool)\0textItalic(bool)\0"
    "textStrike(bool)\0textColor()\0fontFormat()\0"
    "pos\0contextMenu(QPoint)\0QString\0html\0"
};

const QMetaObject Editor::TextEditor::staticMetaObject = {
    { &TableEditor::staticMetaObject, qt_meta_stringdata_Editor__TextEditor,
      qt_meta_data_Editor__TextEditor, 0 }
};

const QMetaObject *Editor::TextEditor::metaObject() const
{
    return &staticMetaObject;
}

void *Editor::TextEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__TextEditor))
        return static_cast<void*>(const_cast< TextEditor*>(this));
    return TableEditor::qt_metacast(_clname);
}

int Editor::TextEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TableEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: toogleToolbar((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: fileOpen(); break;
        case 2: saveAs(); break;
        case 3: fontBigger(); break;
        case 4: fontSmaller(); break;
        case 5: textBold((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: textUnderline((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: textItalic((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: textStrike((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: textColor(); break;
        case 10: fontFormat(); break;
        case 11: contextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getHtml(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setHtml(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
