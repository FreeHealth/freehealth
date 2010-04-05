/****************************************************************************
** Meta object code from reading C++ file 'printer.h'
**
** Created: Tue Feb 9 13:55:25 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/printerplugin/printer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'printer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Print__Printer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   16,   15,   15, 0x0a,
      94,   88,   15,   15, 0x2a,
     135,  131,   15,   15, 0x2a,
     231,  163,   15,   15, 0x0a,
     367,  311,   15,   15, 0x2a,
     493,  443,   15,   15, 0x2a,
     607,  562,   15,   15, 0x2a,
     701,  670,   15,   15, 0x2a,
     762,  750,   15,   15, 0x2a,
     807,  797,   15,   15, 0x2a,
     871,  833,   15,   15, 0x0a,
     950,  924,   15,   15, 0x2a,
    1006,  999,   15,   15, 0x2a,
    1046, 1041,   15,   15, 0x2a,
    1097, 1090, 1072,   15, 0x0a,
    1132, 1117,   15,   15, 0x0a,
    1197, 1168,   15,   15, 0x0a,
    1287, 1243,   15,   15, 0x0a,
    1396, 1364,   15,   15, 0x2a,
    1482, 1469,   15,   15, 0x2a,
    1552, 1541,   15,   15, 0x2a,
    1647, 1602,   15,   15, 0x0a,
    1746, 1713,   15,   15, 0x2a,
    1822, 1808,   15,   15, 0x2a,
    1882, 1870,   15,   15, 0x2a,
    1971, 1921,   15,   15, 0x0a,
    2075, 2037,   15,   15, 0x2a,
    2156, 2137,   15,   15, 0x2a,
    2221, 2204,   15,   15, 0x2a,
    2260,   15,   15,   15, 0x0a,
    2293, 2282, 2277,   15, 0x0a,
    2320, 2314, 2277,   15, 0x0a,
    2345,   15, 2277,   15, 0x2a,
    2378, 2366, 2277,   15, 0x0a,
    2402, 2393, 2277,   15, 0x0a,
    2431, 2417, 2277,   15, 0x0a,
    2474, 2454, 2277,   15, 0x0a,
    2511, 2503, 2277,   15, 0x09,
    2528,   15, 2277,   15, 0x29,

       0        // eod
};

static const char qt_meta_stringdata_Print__Printer[] = {
    "Print::Printer\0\0pix,p,alignement\0"
    "addPixmapWatermark(QPixmap,Presence,Qt::AlignmentFlag)\0"
    "pix,p\0addPixmapWatermark(QPixmap,Presence)\0"
    "pix\0addPixmapWatermark(QPixmap)\0"
    "plainText,p,watermarkAlignment,textAlignment,font,color,orientation\0"
    "addTextWatermark(QString,Presence,Qt::Alignment,Qt::Alignment,QFont,QC"
    "olor,int)\0"
    "plainText,p,watermarkAlignment,textAlignment,font,color\0"
    "addTextWatermark(QString,Presence,Qt::Alignment,Qt::Alignment,QFont,QC"
    "olor)\0"
    "plainText,p,watermarkAlignment,textAlignment,font\0"
    "addTextWatermark(QString,Presence,Qt::Alignment,Qt::Alignment,QFont)\0"
    "plainText,p,watermarkAlignment,textAlignment\0"
    "addTextWatermark(QString,Presence,Qt::Alignment,Qt::Alignment)\0"
    "plainText,p,watermarkAlignment\0"
    "addTextWatermark(QString,Presence,Qt::Alignment)\0"
    "plainText,p\0addTextWatermark(QString,Presence)\0"
    "plainText\0addTextWatermark(QString)\0"
    "html,p,watermarkAlignment,orientation\0"
    "addHtmlWatermark(QString,Presence,Qt::Alignment,int)\0"
    "html,p,watermarkAlignment\0"
    "addHtmlWatermark(QString,Presence,Qt::Alignment)\0"
    "html,p\0addHtmlWatermark(QString,Presence)\0"
    "html\0addHtmlWatermark(QString)\0"
    "PrinterPreviewer*\0parent\0previewer(QWidget*)\0"
    "drawTo,printer\0previewToPixmap(QPixmap&,QPrinter*)\0"
    "drawTo,headerHtml,footerHtml\0"
    "previewHeaderFooter(QPixmap&,QString,QString)\0"
    "drawTo,doc,p,watermarkAlignment,orientation\0"
    "previewDocumentWatermark(QPixmap&,QTextDocument*,Presence,Qt::Alignmen"
    "t,int)\0"
    "drawTo,doc,p,watermarkAlignment\0"
    "previewDocumentWatermark(QPixmap&,QTextDocument*,Presence,Qt::Alignmen"
    "t)\0"
    "drawTo,doc,p\0"
    "previewDocumentWatermark(QPixmap&,QTextDocument*,Presence)\0"
    "drawTo,doc\0previewDocumentWatermark(QPixmap&,QTextDocument*)\0"
    "drawTo,html,p,watermarkAlignment,orientation\0"
    "previewHtmlWatermark(QPixmap&,QString,Presence,Qt::Alignment,int)\0"
    "drawTo,html,p,watermarkAlignment\0"
    "previewHtmlWatermark(QPixmap&,QString,Presence,Qt::Alignment)\0"
    "drawTo,html,p\0"
    "previewHtmlWatermark(QPixmap&,QString,Presence)\0"
    "drawTo,html\0previewHtmlWatermark(QPixmap&,QString)\0"
    "drawTo,plainText,p,watermarkAlignment,orientation\0"
    "previewTextWatermark(QPixmap&,QString,Presence,Qt::Alignment,int)\0"
    "drawTo,plainText,p,watermarkAlignment\0"
    "previewTextWatermark(QPixmap&,QString,Presence,Qt::Alignment)\0"
    "drawTo,plainText,p\0"
    "previewTextWatermark(QPixmap&,QString,Presence)\0"
    "drawTo,plainText\0"
    "previewTextWatermark(QPixmap&,QString)\0"
    "clearWatermark()\0bool\0docToPrint\0"
    "print(QTextDocument)\0state\0"
    "printWithDuplicata(bool)\0printWithDuplicata()\0"
    "htmlToPrint\0print(QString)\0fileName\0"
    "toPdf(QString)\0fileName,html\0"
    "toPdf(QString,QString)\0fileName,docToPrint\0"
    "toPdf(QString,QTextDocument)\0printer\0"
    "print(QPrinter*)\0print()\0"
};

const QMetaObject Print::Printer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Print__Printer,
      qt_meta_data_Print__Printer, 0 }
};

const QMetaObject *Print::Printer::metaObject() const
{
    return &staticMetaObject;
}

void *Print::Printer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Print__Printer))
        return static_cast<void*>(const_cast< Printer*>(this));
    return QObject::qt_metacast(_clname);
}

int Print::Printer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addPixmapWatermark((*reinterpret_cast< const QPixmap(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::AlignmentFlag(*)>(_a[3]))); break;
        case 1: addPixmapWatermark((*reinterpret_cast< const QPixmap(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2]))); break;
        case 2: addPixmapWatermark((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        case 3: addTextWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4])),(*reinterpret_cast< const QFont(*)>(_a[5])),(*reinterpret_cast< const QColor(*)>(_a[6])),(*reinterpret_cast< const int(*)>(_a[7]))); break;
        case 4: addTextWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4])),(*reinterpret_cast< const QFont(*)>(_a[5])),(*reinterpret_cast< const QColor(*)>(_a[6]))); break;
        case 5: addTextWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4])),(*reinterpret_cast< const QFont(*)>(_a[5]))); break;
        case 6: addTextWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4]))); break;
        case 7: addTextWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[3]))); break;
        case 8: addTextWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2]))); break;
        case 9: addTextWatermark((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: addHtmlWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 11: addHtmlWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[3]))); break;
        case 12: addHtmlWatermark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Presence(*)>(_a[2]))); break;
        case 13: addHtmlWatermark((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: { PrinterPreviewer* _r = previewer((*reinterpret_cast< QWidget*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PrinterPreviewer**>(_a[0]) = _r; }  break;
        case 15: previewToPixmap((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< QPrinter*(*)>(_a[2]))); break;
        case 16: previewHeaderFooter((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 17: previewDocumentWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< QTextDocument*(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4])),(*reinterpret_cast< const int(*)>(_a[5]))); break;
        case 18: previewDocumentWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< QTextDocument*(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4]))); break;
        case 19: previewDocumentWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< QTextDocument*(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3]))); break;
        case 20: previewDocumentWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< QTextDocument*(*)>(_a[2]))); break;
        case 21: previewHtmlWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4])),(*reinterpret_cast< const int(*)>(_a[5]))); break;
        case 22: previewHtmlWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4]))); break;
        case 23: previewHtmlWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3]))); break;
        case 24: previewHtmlWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 25: previewTextWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4])),(*reinterpret_cast< const int(*)>(_a[5]))); break;
        case 26: previewTextWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3])),(*reinterpret_cast< const Qt::Alignment(*)>(_a[4]))); break;
        case 27: previewTextWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const Presence(*)>(_a[3]))); break;
        case 28: previewTextWatermark((*reinterpret_cast< QPixmap(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 29: clearWatermark(); break;
        case 30: { bool _r = print((*reinterpret_cast< const QTextDocument(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 31: { bool _r = printWithDuplicata((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 32: { bool _r = printWithDuplicata();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 33: { bool _r = print((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 34: { bool _r = toPdf((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 35: { bool _r = toPdf((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 36: { bool _r = toPdf((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QTextDocument(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 37: { bool _r = print((*reinterpret_cast< QPrinter*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 38: { bool _r = print();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 39;
    }
    return _id;
}
static const uint qt_meta_data_Print__PrinterPreviewer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      32,   24, 0x0a195003,
      43,   24, 0x0a195003,

       0        // eod
};

static const char qt_meta_stringdata_Print__PrinterPreviewer[] = {
    "Print::PrinterPreviewer\0QString\0"
    "htmlHeader\0htmlFooter\0"
};

const QMetaObject Print::PrinterPreviewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Print__PrinterPreviewer,
      qt_meta_data_Print__PrinterPreviewer, 0 }
};

const QMetaObject *Print::PrinterPreviewer::metaObject() const
{
    return &staticMetaObject;
}

void *Print::PrinterPreviewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Print__PrinterPreviewer))
        return static_cast<void*>(const_cast< PrinterPreviewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int Print::PrinterPreviewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = headerToHtml(); break;
        case 1: *reinterpret_cast< QString*>(_v) = footerToHtml(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setHeader(*reinterpret_cast< QString*>(_v)); break;
        case 1: setFooter(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
