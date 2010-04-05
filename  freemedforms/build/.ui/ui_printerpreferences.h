/********************************************************************************
** Form generated from reading ui file 'printerpreferences.ui'
**
** Created: Tue Feb 9 09:16:25 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PRINTERPREFERENCES_H
#define UI_PRINTERPREFERENCES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

namespace Print {
namespace Internal {

class Ui_PrinterPreferences
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QFrame *line;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QListWidget *printerList;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QLabel *label_3;
    QComboBox *resolutionCombo;
    QRadioButton *colorBox;
    QRadioButton *grayBox;
    QGroupBox *pdfGroupBox;
    QGridLayout *gridLayout_4;
    QCheckBox *keepPdfBox;
    QPushButton *selectFolderButton;
    QLineEdit *folderName;

    void setupUi(QWidget *Print__Internal__PrinterPreferences)
    {
        if (Print__Internal__PrinterPreferences->objectName().isEmpty())
            Print__Internal__PrinterPreferences->setObjectName(QString::fromUtf8("Print__Internal__PrinterPreferences"));
        Print__Internal__PrinterPreferences->resize(400, 386);
        gridLayout = new QGridLayout(Print__Internal__PrinterPreferences);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(Print__Internal__PrinterPreferences);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        line = new QFrame(Print__Internal__PrinterPreferences);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 1);

        groupBox = new QGroupBox(Print__Internal__PrinterPreferences);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        printerList = new QListWidget(groupBox);
        printerList->setObjectName(QString::fromUtf8("printerList"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(printerList->sizePolicy().hasHeightForWidth());
        printerList->setSizePolicy(sizePolicy1);
        printerList->setAlternatingRowColors(true);
        printerList->setUniformItemSizes(true);

        gridLayout_2->addWidget(printerList, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox, 2, 0, 1, 1);

        groupBox_2 = new QGroupBox(Print__Internal__PrinterPreferences);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_3->addWidget(label_3, 1, 0, 1, 1);

        resolutionCombo = new QComboBox(groupBox_2);
        resolutionCombo->setObjectName(QString::fromUtf8("resolutionCombo"));

        gridLayout_3->addWidget(resolutionCombo, 1, 1, 1, 1);

        colorBox = new QRadioButton(groupBox_2);
        colorBox->setObjectName(QString::fromUtf8("colorBox"));

        gridLayout_3->addWidget(colorBox, 0, 0, 1, 1);

        grayBox = new QRadioButton(groupBox_2);
        grayBox->setObjectName(QString::fromUtf8("grayBox"));

        gridLayout_3->addWidget(grayBox, 0, 1, 1, 1);


        gridLayout->addWidget(groupBox_2, 3, 0, 1, 1);

        pdfGroupBox = new QGroupBox(Print__Internal__PrinterPreferences);
        pdfGroupBox->setObjectName(QString::fromUtf8("pdfGroupBox"));
        pdfGroupBox->setEnabled(false);
        gridLayout_4 = new QGridLayout(pdfGroupBox);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        keepPdfBox = new QCheckBox(pdfGroupBox);
        keepPdfBox->setObjectName(QString::fromUtf8("keepPdfBox"));

        gridLayout_4->addWidget(keepPdfBox, 0, 0, 2, 2);

        selectFolderButton = new QPushButton(pdfGroupBox);
        selectFolderButton->setObjectName(QString::fromUtf8("selectFolderButton"));

        gridLayout_4->addWidget(selectFolderButton, 0, 2, 2, 1);

        folderName = new QLineEdit(pdfGroupBox);
        folderName->setObjectName(QString::fromUtf8("folderName"));
        folderName->setEnabled(false);

        gridLayout_4->addWidget(folderName, 2, 0, 1, 3);


        gridLayout->addWidget(pdfGroupBox, 4, 0, 1, 1);


        retranslateUi(Print__Internal__PrinterPreferences);

        QMetaObject::connectSlotsByName(Print__Internal__PrinterPreferences);
    } // setupUi

    void retranslateUi(QWidget *Print__Internal__PrinterPreferences)
    {
        Print__Internal__PrinterPreferences->setWindowTitle(QApplication::translate("Print::Internal::PrinterPreferences", "Printing preferences", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Print::Internal::PrinterPreferences", "Printing preferences", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Print::Internal::PrinterPreferences", "Default printer", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Print::Internal::PrinterPreferences", "Default printing options", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Print::Internal::PrinterPreferences", "Resolution", 0, QApplication::UnicodeUTF8));
        resolutionCombo->clear();
        resolutionCombo->insertItems(0, QStringList()
         << QApplication::translate("Print::Internal::PrinterPreferences", "Draft", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Print::Internal::PrinterPreferences", "Normal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Print::Internal::PrinterPreferences", "High", 0, QApplication::UnicodeUTF8)
        );
        colorBox->setText(QApplication::translate("Print::Internal::PrinterPreferences", "Color", 0, QApplication::UnicodeUTF8));
        grayBox->setText(QApplication::translate("Print::Internal::PrinterPreferences", "Gray scale", 0, QApplication::UnicodeUTF8));
        pdfGroupBox->setTitle(QApplication::translate("Print::Internal::PrinterPreferences", "Automated copy", 0, QApplication::UnicodeUTF8));
        keepPdfBox->setText(QApplication::translate("Print::Internal::PrinterPreferences", "Always keep a PDF copy in", 0, QApplication::UnicodeUTF8));
        selectFolderButton->setText(QApplication::translate("Print::Internal::PrinterPreferences", "Select a folder", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Print__Internal__PrinterPreferences);
    } // retranslateUi

};

} // namespace Internal
} // namespace Print

namespace Print {
namespace Internal {
namespace Ui {
    class PrinterPreferences: public Ui_PrinterPreferences {};
} // namespace Ui
} // namespace Internal
} // namespace Print

#endif // UI_PRINTERPREFERENCES_H
