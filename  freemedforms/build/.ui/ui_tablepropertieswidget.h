/********************************************************************************
** Form generated from reading ui file 'tablepropertieswidget.ui'
**
** Created: Mon Feb 1 11:53:06 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TABLEPROPERTIESWIDGET_H
#define UI_TABLEPROPERTIESWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>
#include <utils/widgets/colorbuttonchooser.h>

namespace Editor {
namespace Internal {

class Ui_TablePropertiesWidget
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tabBorders;
    QGridLayout *gridLayout_6;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QSpinBox *borderWidthSpin;
    QLabel *label_2;
    QComboBox *borderStyleCombo;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_5;
    QSpinBox *bottomMarginSpin;
    QSpinBox *topMarginSpin;
    QSpinBox *leftMarginSpin;
    QSpinBox *rightMarginSpin;
    QLabel *label_5;
    QLabel *label_3;
    QLabel *label_7;
    QSpinBox *cellSpacingSpin;
    QSpinBox *cellPaddingSpin;
    QWidget *tabColors;
    QGridLayout *gridLayout_2;
    QCheckBox *alternateColorsCheck;
    QLabel *label_4;
    Utils::ColorButtonChooser *borderColorButton;
    QLabel *label_6;
    Utils::ColorButtonChooser *cellBackgroundColorButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *Editor__Internal__TablePropertiesWidget)
    {
        if (Editor__Internal__TablePropertiesWidget->objectName().isEmpty())
            Editor__Internal__TablePropertiesWidget->setObjectName(QString::fromUtf8("Editor__Internal__TablePropertiesWidget"));
        Editor__Internal__TablePropertiesWidget->resize(524, 368);
        gridLayout = new QGridLayout(Editor__Internal__TablePropertiesWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(Editor__Internal__TablePropertiesWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabBorders = new QWidget();
        tabBorders->setObjectName(QString::fromUtf8("tabBorders"));
        gridLayout_6 = new QGridLayout(tabBorders);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        groupBox = new QGroupBox(tabBorders);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        borderWidthSpin = new QSpinBox(groupBox);
        borderWidthSpin->setObjectName(QString::fromUtf8("borderWidthSpin"));

        gridLayout_3->addWidget(borderWidthSpin, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 1, 0, 1, 1);

        borderStyleCombo = new QComboBox(groupBox);
        borderStyleCombo->setObjectName(QString::fromUtf8("borderStyleCombo"));

        gridLayout_3->addWidget(borderStyleCombo, 1, 1, 1, 1);


        gridLayout_6->addWidget(groupBox, 0, 0, 1, 2);

        checkBox_2 = new QCheckBox(tabBorders);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setEnabled(false);

        gridLayout_6->addWidget(checkBox_2, 2, 0, 1, 1);

        checkBox_3 = new QCheckBox(tabBorders);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));
        checkBox_3->setEnabled(false);

        gridLayout_6->addWidget(checkBox_3, 3, 0, 1, 1);

        groupBox_3 = new QGroupBox(tabBorders);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_7 = new QGridLayout(groupBox_3);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        bottomMarginSpin = new QSpinBox(groupBox_3);
        bottomMarginSpin->setObjectName(QString::fromUtf8("bottomMarginSpin"));

        gridLayout_5->addWidget(bottomMarginSpin, 2, 1, 1, 1);

        topMarginSpin = new QSpinBox(groupBox_3);
        topMarginSpin->setObjectName(QString::fromUtf8("topMarginSpin"));

        gridLayout_5->addWidget(topMarginSpin, 0, 1, 1, 1);

        leftMarginSpin = new QSpinBox(groupBox_3);
        leftMarginSpin->setObjectName(QString::fromUtf8("leftMarginSpin"));

        gridLayout_5->addWidget(leftMarginSpin, 1, 0, 1, 1);

        rightMarginSpin = new QSpinBox(groupBox_3);
        rightMarginSpin->setObjectName(QString::fromUtf8("rightMarginSpin"));

        gridLayout_5->addWidget(rightMarginSpin, 1, 2, 1, 1);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_5->addWidget(label_5, 1, 1, 1, 1);


        gridLayout_7->addLayout(gridLayout_5, 0, 0, 2, 1);

        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        gridLayout_7->addWidget(label_3, 0, 1, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_7->addWidget(label_7, 1, 1, 1, 1);

        cellSpacingSpin = new QSpinBox(groupBox_3);
        cellSpacingSpin->setObjectName(QString::fromUtf8("cellSpacingSpin"));

        gridLayout_7->addWidget(cellSpacingSpin, 0, 2, 1, 1);

        cellPaddingSpin = new QSpinBox(groupBox_3);
        cellPaddingSpin->setObjectName(QString::fromUtf8("cellPaddingSpin"));

        gridLayout_7->addWidget(cellPaddingSpin, 1, 2, 1, 1);


        gridLayout_6->addWidget(groupBox_3, 1, 0, 1, 1);

        tabWidget->addTab(tabBorders, QString());
        tabColors = new QWidget();
        tabColors->setObjectName(QString::fromUtf8("tabColors"));
        tabColors->setEnabled(true);
        gridLayout_2 = new QGridLayout(tabColors);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        alternateColorsCheck = new QCheckBox(tabColors);
        alternateColorsCheck->setObjectName(QString::fromUtf8("alternateColorsCheck"));

        gridLayout_2->addWidget(alternateColorsCheck, 0, 0, 1, 2);

        label_4 = new QLabel(tabColors);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 1, 0, 1, 1);

        borderColorButton = new Utils::ColorButtonChooser(tabColors);
        borderColorButton->setObjectName(QString::fromUtf8("borderColorButton"));
        borderColorButton->setMinimumSize(QSize(20, 20));
        borderColorButton->setMaximumSize(QSize(20, 20));

        gridLayout_2->addWidget(borderColorButton, 1, 1, 1, 1);

        label_6 = new QLabel(tabColors);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 2, 0, 1, 1);

        cellBackgroundColorButton = new Utils::ColorButtonChooser(tabColors);
        cellBackgroundColorButton->setObjectName(QString::fromUtf8("cellBackgroundColorButton"));
        cellBackgroundColorButton->setMinimumSize(QSize(20, 20));
        cellBackgroundColorButton->setMaximumSize(QSize(20, 20));

        gridLayout_2->addWidget(cellBackgroundColorButton, 2, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 3, 0, 1, 1);

        tabWidget->addTab(tabColors, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


        retranslateUi(Editor__Internal__TablePropertiesWidget);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(Editor__Internal__TablePropertiesWidget);
    } // setupUi

    void retranslateUi(QWidget *Editor__Internal__TablePropertiesWidget)
    {
        Editor__Internal__TablePropertiesWidget->setWindowTitle(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Form", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Border", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Width", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Style", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Adjust column size", 0, QApplication::UnicodeUTF8));
        checkBox_3->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Adjust row size", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Margins, Padding and spacing", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Margins (pix)", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Cell spacing", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Cell padding", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabBorders), QApplication::translate("Editor::Internal::TablePropertiesWidget", "Border / Space", 0, QApplication::UnicodeUTF8));
        alternateColorsCheck->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Alterning rows color", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Border color", 0, QApplication::UnicodeUTF8));
        borderColorButton->setText(QString());
        label_6->setText(QApplication::translate("Editor::Internal::TablePropertiesWidget", "Cells background color", 0, QApplication::UnicodeUTF8));
        cellBackgroundColorButton->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabColors), QApplication::translate("Editor::Internal::TablePropertiesWidget", "Colors", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Editor__Internal__TablePropertiesWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace Editor

namespace Editor {
namespace Internal {
namespace Ui {
    class TablePropertiesWidget: public Ui_TablePropertiesWidget {};
} // namespace Ui
} // namespace Internal
} // namespace Editor

#endif // UI_TABLEPROPERTIESWIDGET_H
