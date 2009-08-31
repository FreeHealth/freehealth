/********************************************************************************
** Form generated from reading ui file 'main.ui'
**
** Created: Sat Aug 8 20:07:41 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainClass
{
public:
    QVBoxLayout *vboxLayout;
    QComboBox *comboBox;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hboxLayout;
    QLabel *label;
    QLabel *text1;
    QHBoxLayout *hboxLayout1;
    QLabel *label_3;
    QLabel *text2;
    QHBoxLayout *hboxLayout2;
    QLabel *label_5;
    QLabel *text3;
    QSpacerItem *spacerItem;

    void setupUi(QWidget *mainClass)
    {
        if (mainClass->objectName().isEmpty())
            mainClass->setObjectName(QString::fromUtf8("mainClass"));
        mainClass->resize(399, 176);
        vboxLayout = new QVBoxLayout(mainClass);
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        comboBox = new QComboBox(mainClass);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        vboxLayout->addWidget(comboBox);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(6);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        label = new QLabel(mainClass);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        hboxLayout->addWidget(label);

        text1 = new QLabel(mainClass);
        text1->setObjectName(QString::fromUtf8("text1"));
        text1->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(text1->sizePolicy().hasHeightForWidth());
        text1->setSizePolicy(sizePolicy1);

        hboxLayout->addWidget(text1);


        vboxLayout1->addLayout(hboxLayout);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        label_3 = new QLabel(mainClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        hboxLayout1->addWidget(label_3);

        text2 = new QLabel(mainClass);
        text2->setObjectName(QString::fromUtf8("text2"));
        text2->setEnabled(false);
        sizePolicy1.setHeightForWidth(text2->sizePolicy().hasHeightForWidth());
        text2->setSizePolicy(sizePolicy1);

        hboxLayout1->addWidget(text2);


        vboxLayout1->addLayout(hboxLayout1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        label_5 = new QLabel(mainClass);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        hboxLayout2->addWidget(label_5);

        text3 = new QLabel(mainClass);
        text3->setObjectName(QString::fromUtf8("text3"));
        text3->setEnabled(false);
        sizePolicy1.setHeightForWidth(text3->sizePolicy().hasHeightForWidth());
        text3->setSizePolicy(sizePolicy1);

        hboxLayout2->addWidget(text3);


        vboxLayout1->addLayout(hboxLayout2);


        vboxLayout->addLayout(vboxLayout1);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem);


        retranslateUi(mainClass);

        QMetaObject::connectSlotsByName(mainClass);
    } // setupUi

    void retranslateUi(QWidget *mainClass)
    {
        mainClass->setWindowTitle(QApplication::translate("mainClass", "main", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("mainClass", "Text1:", 0, QApplication::UnicodeUTF8));
        text1->setText(QApplication::translate("mainClass", "N/A", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("mainClass", "Text2:", 0, QApplication::UnicodeUTF8));
        text2->setText(QApplication::translate("mainClass", "N/A", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("mainClass", "Text3:", 0, QApplication::UnicodeUTF8));
        text3->setText(QApplication::translate("mainClass", "N/A", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(mainClass);
    } // retranslateUi

};

namespace Ui {
    class mainClass: public Ui_mainClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_H
