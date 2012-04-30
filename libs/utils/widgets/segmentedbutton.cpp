#include "segmentedbutton.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>

namespace {
const char *const FRAME_CSS =
        "QFrame {"
        "border-bottom: 1px solid #C2C7CB;"
//        "border-bottom-color:  #C2C7CB; /* same as the pane color */"
        "border-top-left-radius: 4px;"
        "border-bottom-left-radius: 4px;"
//        "min-width: 8ex;"
//        "padding: 2px;"
        "}"
        ;

//const char *const FIRST_BUTTON_CSS =
//        "QPushButton:pressed {"
//        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//        "                            stop: 0 #F1F1F1, stop: 0.4 #eeeeee,"
//        "                            stop: 0.5 #e8e8e8, stop: 1.0 #e3e3e3);"
//        "border: 1px solid black/*#C4C4C3*/;"
//        "border-bottom: 1px solid navy;"
////        "border-bottom-color:  #C2C7CB; /* same as the pane color */"
//        "border-top-left-radius: 4px;"
//        "border-bottom-left-radius: 4px;"
//        "min-width: 8ex;"
//        "padding: 2px;"
//        "}"
////        "QPushButton:pressed {"
////        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
////        "                            stop: 0 #c1c1c1, stop: 0.4 #dddddd,"
////        "                            stop: 0.5 #d8d8d8, stop: 1.0 #d3d3d3);"
////        "border: 1px solid black/*#C4C4C3*/;"
////        "border-bottom: 1px solid navy;"
////        "border-top-left-radius: 4px;"
////        "border-bottom-left-radius: 4px;"
////        "min-width: 8ex;"
////        "padding: 2px;"
////        "}"
//        "QPushButton {"
//        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//        "                            stop: 0 #F5F5F5, stop: 0.4 #f0f0f0,"
//        "                            stop: 0.5 #dfdfdf, stop: 1.0 #dbdbdb);"
//        "border: 1px solid black/*#C4C4C3*/;"
//        "border-bottom: 1px solid navy;"
//        "border-top-left-radius: 4px;"
//        "border-bottom-left-radius: 4px;"
//        "min-width: 8ex;"
//        "padding: 2px;"
//        "}"
//        ;

//        "QTabBar::tab:selected, QTabBar::tab:hover {"
//        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//        "                            stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
//        "                            stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
//        "}"

//        "QTabBar::tab:selected {"
//        "border-color: #9B9B9B;"
//        "border-bottom-color: #C2C7CB; /* same as pane color */"
//        "}"
//        ;

const char *const FIRST_BUTTON =
        "border-top-left-radius: 8px;"
        "border-bottom-left-radius: 8px;"
        ;

const char *const MIDDLE_BUTTON = ""
//        "border-left: 1px;"
        ;

const char *const LAST_BUTTON =
        "border-top-right-radius: 8px;"
        "border-bottom-right-radius: 8px;"
        ;

const char *const BUTTON_CSS =
        "QPushButton {"
        "border: 1px outset #777;"
        "background: qradialgradient(cx: 0.3, cy: -0.4,"
        "fx: 0.3, fy: -0.4,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #ccc);"
        "color: #333;"
        "%1"
        "padding: 5px;"
        "min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "background: qradialgradient(cx: 0.4, cy: -0.1,"
        "fx: 0.4, fy: -0.1,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
        "}"
        "QPushButton:pressed {"
        "border: 1px inset #666;"
        "background: qradialgradient(cx: 0.3, cy: -0.4,"
        "fx: 0.3, fy: -0.4,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #888);"
        "}"
        ;

}

SegmentedButton::SegmentedButton(QWidget *parent) :
    QWidget(parent),
    _first(0),
    _last(0)
{
    QHBoxLayout *lay = _buttonLayout = new QHBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);
    setLayout(lay);
}

void SegmentedButton::setFirstButton(QPushButton *but)
{
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::FIRST_BUTTON));
    _buttonLayout->addWidget(but);
    _first = but;
}
void SegmentedButton::addMiddleButton(QPushButton *but)
{
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::MIDDLE_BUTTON));
    _buttonLayout->addWidget(but);
    _buttons << but;
}
void SegmentedButton::setLastButton(QPushButton *but)
{
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::LAST_BUTTON));
    _buttonLayout->addWidget(but);
    _last = but;
}
