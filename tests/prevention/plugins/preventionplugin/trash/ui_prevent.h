/********************************************************************************
** Form generated from reading UI file 'prevent.ui'
**
** Created: Sun Mar 18 20:48:55 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREVENT_H
#define UI_PREVENT_H

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHeaderView>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreventWidget
{
public:

    void setupUi(QWidget *PreventWidget)
    {
        if (PreventWidget->objectName().isEmpty())
            PreventWidget->setObjectName(QString::fromUtf8("PreventWidget"));
        PreventWidget->resize(543, 373);

        retranslateUi(PreventWidget);

        QMetaObject::connectSlotsByName(PreventWidget);
    } // setupUi

    void retranslateUi(QWidget *PreventWidget)
    {
        PreventWidget->setWindowTitle(QApplication::translate("PreventWidget", "Prevention", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PreventWidget: public Ui_PreventWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREVENT_H
