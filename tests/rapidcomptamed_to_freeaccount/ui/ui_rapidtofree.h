/********************************************************************************
** Form generated from reading UI file 'rapidtofree.ui'
**
** Created: Fri Sep 21 13:47:47 2012
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAPIDTOFREE_H
#define UI_RAPIDTOFREE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RapidToFreeWidget
{
public:

    void setupUi(QWidget *RapidToFreeWidget)
    {
        if (RapidToFreeWidget->objectName().isEmpty())
            RapidToFreeWidget->setObjectName(QString::fromUtf8("RapidToFreeWidget"));
        RapidToFreeWidget->resize(539, 373);

        retranslateUi(RapidToFreeWidget);

        QMetaObject::connectSlotsByName(RapidToFreeWidget);
    } // setupUi

    void retranslateUi(QWidget *RapidToFreeWidget)
    {
        RapidToFreeWidget->setWindowTitle(QApplication::translate("RapidToFreeWidget", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RapidToFreeWidget: public Ui_RapidToFreeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAPIDTOFREE_H
