/********************************************************************************
** Form generated from reading ui file 'baseformwidgetsoptionspage.ui'
**
** Created: Mon Feb 1 19:09:39 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_BASEFORMWIDGETSOPTIONSPAGE_H
#define UI_BASEFORMWIDGETSOPTIONSPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

namespace BaseWidgets {
namespace Internal {

class Ui_BaseFormWidgetsOptionsPage
{
public:
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *marginSpin;
    QLabel *label_2;
    QSpinBox *spacingSpin;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *BaseWidgets__Internal__BaseFormWidgetsOptionsPage)
    {
        if (BaseWidgets__Internal__BaseFormWidgetsOptionsPage->objectName().isEmpty())
            BaseWidgets__Internal__BaseFormWidgetsOptionsPage->setObjectName(QString::fromUtf8("BaseWidgets__Internal__BaseFormWidgetsOptionsPage"));
        BaseWidgets__Internal__BaseFormWidgetsOptionsPage->resize(400, 300);
        gridLayout_2 = new QGridLayout(BaseWidgets__Internal__BaseFormWidgetsOptionsPage);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        groupBox = new QGroupBox(BaseWidgets__Internal__BaseFormWidgetsOptionsPage);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        marginSpin = new QSpinBox(groupBox);
        marginSpin->setObjectName(QString::fromUtf8("marginSpin"));

        gridLayout->addWidget(marginSpin, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        spacingSpin = new QSpinBox(groupBox);
        spacingSpin->setObjectName(QString::fromUtf8("spacingSpin"));

        gridLayout->addWidget(spacingSpin, 1, 1, 1, 1);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 1, 0, 1, 1);


        retranslateUi(BaseWidgets__Internal__BaseFormWidgetsOptionsPage);

        QMetaObject::connectSlotsByName(BaseWidgets__Internal__BaseFormWidgetsOptionsPage);
    } // setupUi

    void retranslateUi(QWidget *BaseWidgets__Internal__BaseFormWidgetsOptionsPage)
    {
        BaseWidgets__Internal__BaseFormWidgetsOptionsPage->setWindowTitle(QApplication::translate("BaseWidgets::Internal::BaseFormWidgetsOptionsPage", "Form", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("BaseWidgets::Internal::BaseFormWidgetsOptionsPage", "Compact view", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BaseWidgets::Internal::BaseFormWidgetsOptionsPage", "set margin to", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("BaseWidgets::Internal::BaseFormWidgetsOptionsPage", "set spacing to", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(BaseWidgets__Internal__BaseFormWidgetsOptionsPage);
    } // retranslateUi

};

} // namespace Internal
} // namespace BaseWidgets

namespace BaseWidgets {
namespace Internal {
namespace Ui {
    class BaseFormWidgetsOptionsPage: public Ui_BaseFormWidgetsOptionsPage {};
} // namespace Ui
} // namespace Internal
} // namespace BaseWidgets

#endif // UI_BASEFORMWIDGETSOPTIONSPAGE_H
