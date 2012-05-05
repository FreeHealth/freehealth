/********************************************************************************
** Form generated from reading UI file 'ShowAlertsWidget.ui'
**
** Created: Sat May 5 07:49:54 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWALERTSWIDGET_H
#define UI_SHOWALERTSWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShowAlertsWidget
{
public:

    void setupUi(QWidget *ShowAlertsWidget)
    {
        if (ShowAlertsWidget->objectName().isEmpty())
            ShowAlertsWidget->setObjectName(QString::fromUtf8("ShowAlertsWidget"));
        ShowAlertsWidget->resize(539, 373);

        retranslateUi(ShowAlertsWidget);

        QMetaObject::connectSlotsByName(ShowAlertsWidget);
    } // setupUi

    void retranslateUi(QWidget *ShowAlertsWidget)
    {
        ShowAlertsWidget->setWindowTitle(QApplication::translate("ShowAlertsWidget", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ShowAlertsWidget: public Ui_ShowAlertsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWALERTSWIDGET_H
