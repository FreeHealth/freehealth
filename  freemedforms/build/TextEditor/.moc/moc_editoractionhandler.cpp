/****************************************************************************
** Meta object code from reading C++ file 'editoractionhandler.h'
**
** Created: Mon Feb 1 11:53:29 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/texteditorplugin/editoractionhandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editoractionhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Editor__Internal__EditorActionHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      37,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      39,   38,   38,   38, 0x0a,
      55,   38,   38,   38, 0x0a,
      74,   38,   38,   38, 0x0a,
      93,   38,   38,   38, 0x0a,
     112,   38,   38,   38, 0x0a,
     133,   38,   38,   38, 0x0a,
     161,  154,   38,   38, 0x08,
     203,   38,   38,   38, 0x08,
     229,  227,   38,   38, 0x08,
     250,  248,   38,   38, 0x08,
     273,  271,   38,   38, 0x08,
     305,   38,   38,   38, 0x08,
     328,   38,   38,   38, 0x08,
     344,   38,   38,   38, 0x08,
     355,   38,   38,   38, 0x08,
     364,   38,   38,   38, 0x08,
     371,   38,   38,   38, 0x08,
     377,   38,   38,   38, 0x08,
     385,   38,   38,   38, 0x08,
     392,   38,   38,   38, 0x08,
     399,   38,   38,   38, 0x08,
     412,   38,   38,   38, 0x08,
     426,   38,   38,   38, 0x08,
     437,   38,   38,   38, 0x08,
     453,   38,   38,   38, 0x08,
     466,   38,   38,   38, 0x08,
     479,   38,   38,   38, 0x08,
     491,   38,   38,   38, 0x08,
     504,   38,   38,   38, 0x08,
     516,   38,   38,   38, 0x08,
     527,   38,   38,   38, 0x08,
     545,   38,   38,   38, 0x08,
     559,   38,   38,   38, 0x08,
     573,   38,   38,   38, 0x08,
     590,   38,   38,   38, 0x08,
     607,   38,   38,   38, 0x08,
     625,   38,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Editor__Internal__EditorActionHandler[] = {
    "Editor::Internal::EditorActionHandler\0"
    "\0updateActions()\0updateRedoAction()\0"
    "updateUndoAction()\0updateCopyAction()\0"
    "updateTableActions()\0updateColorActions()\0"
    "format\0currentCharFormatChanged(QTextCharFormat)\0"
    "cursorPositionChanged()\0f\0fontChanged(QFont)\0"
    "c\0colorChanged(QColor)\0a\0"
    "alignmentChanged(Qt::Alignment)\0"
    "clipboardDataChanged()\0toogleToolbar()\0"
    "fileOpen()\0saveAs()\0copy()\0cut()\0"
    "paste()\0undo()\0redo()\0fontBigger()\0"
    "fontSmaller()\0textBold()\0textUnderline()\0"
    "textItalic()\0textStrike()\0textColor()\0"
    "fontFormat()\0textAlign()\0addTable()\0"
    "tableProperties()\0tableAddRow()\0"
    "tableAddCol()\0tableRemoveCol()\0"
    "tableRemoveRow()\0tableMergeCells()\0"
    "tableSplitCells()\0"
};

const QMetaObject Editor::Internal::EditorActionHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Editor__Internal__EditorActionHandler,
      qt_meta_data_Editor__Internal__EditorActionHandler, 0 }
};

const QMetaObject *Editor::Internal::EditorActionHandler::metaObject() const
{
    return &staticMetaObject;
}

void *Editor::Internal::EditorActionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__Internal__EditorActionHandler))
        return static_cast<void*>(const_cast< EditorActionHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Editor::Internal::EditorActionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateActions(); break;
        case 1: updateRedoAction(); break;
        case 2: updateUndoAction(); break;
        case 3: updateCopyAction(); break;
        case 4: updateTableActions(); break;
        case 5: updateColorActions(); break;
        case 6: currentCharFormatChanged((*reinterpret_cast< const QTextCharFormat(*)>(_a[1]))); break;
        case 7: cursorPositionChanged(); break;
        case 8: fontChanged((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 9: colorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 10: alignmentChanged((*reinterpret_cast< Qt::Alignment(*)>(_a[1]))); break;
        case 11: clipboardDataChanged(); break;
        case 12: toogleToolbar(); break;
        case 13: fileOpen(); break;
        case 14: saveAs(); break;
        case 15: copy(); break;
        case 16: cut(); break;
        case 17: paste(); break;
        case 18: undo(); break;
        case 19: redo(); break;
        case 20: fontBigger(); break;
        case 21: fontSmaller(); break;
        case 22: textBold(); break;
        case 23: textUnderline(); break;
        case 24: textItalic(); break;
        case 25: textStrike(); break;
        case 26: textColor(); break;
        case 27: fontFormat(); break;
        case 28: textAlign(); break;
        case 29: addTable(); break;
        case 30: tableProperties(); break;
        case 31: tableAddRow(); break;
        case 32: tableAddCol(); break;
        case 33: tableRemoveCol(); break;
        case 34: tableRemoveRow(); break;
        case 35: tableMergeCells(); break;
        case 36: tableSplitCells(); break;
        default: ;
        }
        _id -= 37;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
