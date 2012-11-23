/********************************************************************************
** Form generated from reading UI file 'moreIHM.ui'
**
** Created: Sun Mar 18 20:48:55 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOREIHM_H
#define UI_MOREIHM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MoreWidget
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *comboLabel;
    QComboBox *comboItemsSameParent;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *MoreWidget)
    {
        if (MoreWidget->objectName().isEmpty())
            MoreWidget->setObjectName(QString::fromUtf8("MoreWidget"));
        MoreWidget->resize(539, 373);
        gridLayout = new QGridLayout(MoreWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

        comboLabel = new QLabel(MoreWidget);
        comboLabel->setObjectName(QString::fromUtf8("comboLabel"));
        comboLabel->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(comboLabel, 0, 1, 1, 1);

        comboItemsSameParent = new QComboBox(MoreWidget);
        comboItemsSameParent->setObjectName(QString::fromUtf8("comboItemsSameParent"));
        comboItemsSameParent->setMinimumSize(QSize(250, 0));

        gridLayout->addWidget(comboItemsSameParent, 0, 2, 1, 1);

        textBrowser = new QTextBrowser(MoreWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

        gridLayout->addWidget(textBrowser, 1, 0, 1, 3);


        retranslateUi(MoreWidget);

        QMetaObject::connectSlotsByName(MoreWidget);
    } // setupUi

    void retranslateUi(QWidget *MoreWidget)
    {
        MoreWidget->setWindowTitle(QApplication::translate("MoreWidget", "More", 0, QApplication::UnicodeUTF8));
        comboLabel->setText(QApplication::translate("MoreWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MoreWidget: public Ui_MoreWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOREIHM_H
