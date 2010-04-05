/****************************************************************************
** Meta object code from reading C++ file 'mainwindowactionhandler.h'
**
** Created: Mon Feb 1 19:08:22 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/actionmanager/mainwindowactionhandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindowactionhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__Internal__MainWindowActionHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      46,   40,   41,   40, 0x0a,
      66,   60,   40,   40, 0x0a,
      89,   40,   41,   40, 0x0a,
      99,   40,   41,   40, 0x0a,
     110,   40,   41,   40, 0x0a,
     121,   40,   41,   40, 0x0a,
     134,   40,   41,   40, 0x0a,
     142,   40,   41,   40, 0x0a,
     157,   40,   41,   40, 0x0a,
     182,   40,   41,   40, 0x0a,
     202,   40,   41,   40, 0x0a,
     221,   40,   41,   40, 0x0a,
     236,   40,   41,   40, 0x0a,
     254,   40,   41,   40, 0x0a,
     264,   40,   41,   40, 0x0a,
     278,   40,   41,   40, 0x0a,
     299,  292,   40,   40, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Core__Internal__MainWindowActionHandler[] = {
    "Core::Internal::MainWindowActionHandler\0"
    "\0bool\0updateFound()\0error\0"
    "updateCheckerEnd(bool)\0newFile()\0"
    "openFile()\0saveFile()\0saveAsFile()\0"
    "print()\0printPreview()\0applicationPreferences()\0"
    "configureMedintux()\0aboutApplication()\0"
    "aboutPlugins()\0applicationHelp()\0"
    "aboutQt()\0debugDialog()\0checkUpdate()\0"
    "action\0switchLanguage(QAction*)\0"
};

const QMetaObject Core::Internal::MainWindowActionHandler::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Core__Internal__MainWindowActionHandler,
      qt_meta_data_Core__Internal__MainWindowActionHandler, 0 }
};

const QMetaObject *Core::Internal::MainWindowActionHandler::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::MainWindowActionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__MainWindowActionHandler))
        return static_cast<void*>(const_cast< MainWindowActionHandler*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Core::Internal::MainWindowActionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = updateFound();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: updateCheckerEnd((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: { bool _r = newFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = openFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: { bool _r = saveFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { bool _r = saveAsFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { bool _r = print();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: { bool _r = printPreview();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: { bool _r = applicationPreferences();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: { bool _r = configureMedintux();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { bool _r = aboutApplication();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = aboutPlugins();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { bool _r = applicationHelp();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = aboutQt();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: { bool _r = debugDialog();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: { bool _r = checkUpdate();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 16: switchLanguage((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
