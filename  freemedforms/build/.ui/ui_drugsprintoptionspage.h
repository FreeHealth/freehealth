/********************************************************************************
** Form generated from reading ui file 'drugsprintoptionspage.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DRUGSPRINTOPTIONSPAGE_H
#define UI_DRUGSPRINTOPTIONSPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFontComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include <texteditorplugin/texteditor.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DrugsPrintWidget
{
public:
    QGridLayout *gridLayout_4;
    QLabel *label;
    QFrame *line;
    QSpacerItem *verticalSpacer;
    QGroupBox *oldGroupBox;
    QGridLayout *gridLayout;
    QLabel *label_11;
    QFontComboBox *drugFontCombo;
    QHBoxLayout *horizontalLayout_4;
    QSpinBox *drugFontSizeSpin;
    QPushButton *drugBoldButton;
    QPushButton *drugUnderlineButton;
    QPushButton *drugItalicButton;
    QLabel *label_12;
    QFontComboBox *prescriptionFontCombo;
    QHBoxLayout *horizontalLayout_5;
    QSpinBox *prescriptionFontSizeSpin;
    QPushButton *prescrBoldButton;
    QPushButton *prescrUnderlineButton;
    QPushButton *prescrItalicButton;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_9;
    Editor::TextEditor *prescriptionFormatting;
    QTextEdit *formatingSample;
    QCheckBox *lineBreakCheck;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QPushButton *defaultFormattingButton;
    QCheckBox *printDuplicataCheck;

    void setupUi(QWidget *DrugsWidget__Internal__DrugsPrintWidget)
    {
        if (DrugsWidget__Internal__DrugsPrintWidget->objectName().isEmpty())
            DrugsWidget__Internal__DrugsPrintWidget->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DrugsPrintWidget"));
        DrugsWidget__Internal__DrugsPrintWidget->resize(513, 481);
        gridLayout_4 = new QGridLayout(DrugsWidget__Internal__DrugsPrintWidget);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label = new QLabel(DrugsWidget__Internal__DrugsPrintWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label, 0, 0, 1, 1);

        line = new QFrame(DrugsWidget__Internal__DrugsPrintWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_4->addWidget(line, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer, 5, 0, 1, 1);

        oldGroupBox = new QGroupBox(DrugsWidget__Internal__DrugsPrintWidget);
        oldGroupBox->setObjectName(QString::fromUtf8("oldGroupBox"));
        gridLayout = new QGridLayout(oldGroupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_11 = new QLabel(oldGroupBox);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label_11->setFont(font1);

        gridLayout->addWidget(label_11, 0, 0, 1, 1);

        drugFontCombo = new QFontComboBox(oldGroupBox);
        drugFontCombo->setObjectName(QString::fromUtf8("drugFontCombo"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(drugFontCombo->sizePolicy().hasHeightForWidth());
        drugFontCombo->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(drugFontCombo, 1, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(12);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        drugFontSizeSpin = new QSpinBox(oldGroupBox);
        drugFontSizeSpin->setObjectName(QString::fromUtf8("drugFontSizeSpin"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(drugFontSizeSpin->sizePolicy().hasHeightForWidth());
        drugFontSizeSpin->setSizePolicy(sizePolicy2);

        horizontalLayout_4->addWidget(drugFontSizeSpin);

        drugBoldButton = new QPushButton(oldGroupBox);
        drugBoldButton->setObjectName(QString::fromUtf8("drugBoldButton"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(drugBoldButton->sizePolicy().hasHeightForWidth());
        drugBoldButton->setSizePolicy(sizePolicy3);
        drugBoldButton->setMinimumSize(QSize(22, 22));
        drugBoldButton->setMaximumSize(QSize(22, 22));
        drugBoldButton->setCheckable(true);
        drugBoldButton->setFlat(true);

        horizontalLayout_4->addWidget(drugBoldButton);

        drugUnderlineButton = new QPushButton(oldGroupBox);
        drugUnderlineButton->setObjectName(QString::fromUtf8("drugUnderlineButton"));
        sizePolicy3.setHeightForWidth(drugUnderlineButton->sizePolicy().hasHeightForWidth());
        drugUnderlineButton->setSizePolicy(sizePolicy3);
        drugUnderlineButton->setMinimumSize(QSize(22, 22));
        drugUnderlineButton->setMaximumSize(QSize(22, 22));
        drugUnderlineButton->setCheckable(true);
        drugUnderlineButton->setFlat(true);

        horizontalLayout_4->addWidget(drugUnderlineButton);

        drugItalicButton = new QPushButton(oldGroupBox);
        drugItalicButton->setObjectName(QString::fromUtf8("drugItalicButton"));
        sizePolicy3.setHeightForWidth(drugItalicButton->sizePolicy().hasHeightForWidth());
        drugItalicButton->setSizePolicy(sizePolicy3);
        drugItalicButton->setMinimumSize(QSize(22, 22));
        drugItalicButton->setMaximumSize(QSize(22, 22));
        drugItalicButton->setCheckable(true);
        drugItalicButton->setFlat(true);

        horizontalLayout_4->addWidget(drugItalicButton);


        gridLayout->addLayout(horizontalLayout_4, 1, 1, 1, 1);

        label_12 = new QLabel(oldGroupBox);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font1);

        gridLayout->addWidget(label_12, 2, 0, 1, 1);

        prescriptionFontCombo = new QFontComboBox(oldGroupBox);
        prescriptionFontCombo->setObjectName(QString::fromUtf8("prescriptionFontCombo"));
        sizePolicy1.setHeightForWidth(prescriptionFontCombo->sizePolicy().hasHeightForWidth());
        prescriptionFontCombo->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(prescriptionFontCombo, 3, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(12);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        prescriptionFontSizeSpin = new QSpinBox(oldGroupBox);
        prescriptionFontSizeSpin->setObjectName(QString::fromUtf8("prescriptionFontSizeSpin"));
        sizePolicy2.setHeightForWidth(prescriptionFontSizeSpin->sizePolicy().hasHeightForWidth());
        prescriptionFontSizeSpin->setSizePolicy(sizePolicy2);

        horizontalLayout_5->addWidget(prescriptionFontSizeSpin);

        prescrBoldButton = new QPushButton(oldGroupBox);
        prescrBoldButton->setObjectName(QString::fromUtf8("prescrBoldButton"));
        sizePolicy3.setHeightForWidth(prescrBoldButton->sizePolicy().hasHeightForWidth());
        prescrBoldButton->setSizePolicy(sizePolicy3);
        prescrBoldButton->setMinimumSize(QSize(22, 22));
        prescrBoldButton->setMaximumSize(QSize(22, 22));
        prescrBoldButton->setCheckable(true);
        prescrBoldButton->setFlat(true);

        horizontalLayout_5->addWidget(prescrBoldButton);

        prescrUnderlineButton = new QPushButton(oldGroupBox);
        prescrUnderlineButton->setObjectName(QString::fromUtf8("prescrUnderlineButton"));
        sizePolicy3.setHeightForWidth(prescrUnderlineButton->sizePolicy().hasHeightForWidth());
        prescrUnderlineButton->setSizePolicy(sizePolicy3);
        prescrUnderlineButton->setMinimumSize(QSize(22, 22));
        prescrUnderlineButton->setMaximumSize(QSize(22, 22));
        prescrUnderlineButton->setCheckable(true);
        prescrUnderlineButton->setFlat(true);

        horizontalLayout_5->addWidget(prescrUnderlineButton);

        prescrItalicButton = new QPushButton(oldGroupBox);
        prescrItalicButton->setObjectName(QString::fromUtf8("prescrItalicButton"));
        sizePolicy3.setHeightForWidth(prescrItalicButton->sizePolicy().hasHeightForWidth());
        prescrItalicButton->setSizePolicy(sizePolicy3);
        prescrItalicButton->setMinimumSize(QSize(22, 22));
        prescrItalicButton->setMaximumSize(QSize(22, 22));
        prescrItalicButton->setCheckable(true);
        prescrItalicButton->setFlat(true);

        horizontalLayout_5->addWidget(prescrItalicButton);


        gridLayout->addLayout(horizontalLayout_5, 3, 1, 1, 1);


        gridLayout_4->addWidget(oldGroupBox, 2, 0, 1, 1);

        groupBox_3 = new QGroupBox(DrugsWidget__Internal__DrugsPrintWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        gridLayout_9 = new QGridLayout(groupBox_3);
        gridLayout_9->setMargin(10);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        prescriptionFormatting = new Editor::TextEditor(groupBox_3);
        prescriptionFormatting->setObjectName(QString::fromUtf8("prescriptionFormatting"));

        gridLayout_9->addWidget(prescriptionFormatting, 3, 1, 1, 2);

        formatingSample = new QTextEdit(groupBox_3);
        formatingSample->setObjectName(QString::fromUtf8("formatingSample"));
        formatingSample->setEnabled(true);
        formatingSample->setReadOnly(true);

        gridLayout_9->addWidget(formatingSample, 4, 1, 1, 2);

        lineBreakCheck = new QCheckBox(groupBox_3);
        lineBreakCheck->setObjectName(QString::fromUtf8("lineBreakCheck"));

        gridLayout_9->addWidget(lineBreakCheck, 5, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);

        horizontalLayout->addWidget(label_2);

        defaultFormattingButton = new QPushButton(groupBox_3);
        defaultFormattingButton->setObjectName(QString::fromUtf8("defaultFormattingButton"));

        horizontalLayout->addWidget(defaultFormattingButton);


        gridLayout_9->addLayout(horizontalLayout, 2, 1, 1, 2);

        printDuplicataCheck = new QCheckBox(groupBox_3);
        printDuplicataCheck->setObjectName(QString::fromUtf8("printDuplicataCheck"));

        gridLayout_9->addWidget(printDuplicataCheck, 6, 1, 1, 1);


        gridLayout_4->addWidget(groupBox_3, 3, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DrugsPrintWidget);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DrugsPrintWidget);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DrugsPrintWidget)
    {
        DrugsWidget__Internal__DrugsPrintWidget->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Drugs Printing Preferences", 0, QApplication::UnicodeUTF8));
        oldGroupBox->setTitle(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Old", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Drug font", 0, QApplication::UnicodeUTF8));
        drugBoldButton->setText(QString());
        drugUnderlineButton->setText(QString());
        drugItalicButton->setText(QString());
        label_12->setText(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Prescription font", 0, QApplication::UnicodeUTF8));
        prescrBoldButton->setText(QString());
        prescrUnderlineButton->setText(QString());
        prescrItalicButton->setText(QString());
        groupBox_3->setTitle(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Printing", 0, QApplication::UnicodeUTF8));
        lineBreakCheck->setText(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Add a line break between drugs when printing", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Prescription formatting", 0, QApplication::UnicodeUTF8));
        defaultFormattingButton->setText(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Reset to default", 0, QApplication::UnicodeUTF8));
        printDuplicataCheck->setText(QApplication::translate("DrugsWidget::Internal::DrugsPrintWidget", "Print duplicatas", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DrugsPrintWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DrugsPrintWidget: public Ui_DrugsPrintWidget {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_DRUGSPRINTOPTIONSPAGE_H
