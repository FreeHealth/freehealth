/********************************************************************************
** Form generated from reading ui file 'printerpreviewer_p.ui'
**
** Created: Mon Feb 1 11:53:39 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PRINTERPREVIEWER_P_H
#define UI_PRINTERPREVIEWER_P_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace Print {
namespace Internal {

class Ui_PrinterPreviewerPrivate
{
public:
    QGridLayout *gridLayout_2;
    QVBoxLayout *editorLayout;
    QWidget *previewWidget;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout;
    QLabel *previewLabel;
    QLabel *label;
    QSpinBox *pageNumberSpinBox;
    QCheckBox *duplicataCheck;
    QCheckBox *automaticUpdateCheck;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *tooglePreviewButton;
    QPushButton *updatePreviewButton;

    void setupUi(QWidget *Print__Internal__PrinterPreviewerPrivate)
    {
        if (Print__Internal__PrinterPreviewerPrivate->objectName().isEmpty())
            Print__Internal__PrinterPreviewerPrivate->setObjectName(QString::fromUtf8("Print__Internal__PrinterPreviewerPrivate"));
        Print__Internal__PrinterPreviewerPrivate->resize(400, 300);
        gridLayout_2 = new QGridLayout(Print__Internal__PrinterPreviewerPrivate);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setMargin(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        editorLayout = new QVBoxLayout();
        editorLayout->setObjectName(QString::fromUtf8("editorLayout"));
        editorLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

        gridLayout_2->addLayout(editorLayout, 0, 0, 1, 1);

        previewWidget = new QWidget(Print__Internal__PrinterPreviewerPrivate);
        previewWidget->setObjectName(QString::fromUtf8("previewWidget"));
        gridLayout_3 = new QGridLayout(previewWidget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        previewLabel = new QLabel(previewWidget);
        previewLabel->setObjectName(QString::fromUtf8("previewLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(previewLabel->sizePolicy().hasHeightForWidth());
        previewLabel->setSizePolicy(sizePolicy);
        previewLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(previewLabel, 0, 0, 1, 1);


        gridLayout_3->addLayout(gridLayout, 0, 0, 1, 2);

        label = new QLabel(previewWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(label, 1, 0, 1, 1);

        pageNumberSpinBox = new QSpinBox(previewWidget);
        pageNumberSpinBox->setObjectName(QString::fromUtf8("pageNumberSpinBox"));

        gridLayout_3->addWidget(pageNumberSpinBox, 1, 1, 1, 1);

        duplicataCheck = new QCheckBox(previewWidget);
        duplicataCheck->setObjectName(QString::fromUtf8("duplicataCheck"));

        gridLayout_3->addWidget(duplicataCheck, 2, 0, 1, 2);

        automaticUpdateCheck = new QCheckBox(previewWidget);
        automaticUpdateCheck->setObjectName(QString::fromUtf8("automaticUpdateCheck"));

        gridLayout_3->addWidget(automaticUpdateCheck, 3, 0, 1, 2);


        gridLayout_2->addWidget(previewWidget, 0, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(5);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        tooglePreviewButton = new QPushButton(Print__Internal__PrinterPreviewerPrivate);
        tooglePreviewButton->setObjectName(QString::fromUtf8("tooglePreviewButton"));

        horizontalLayout_4->addWidget(tooglePreviewButton);

        updatePreviewButton = new QPushButton(Print__Internal__PrinterPreviewerPrivate);
        updatePreviewButton->setObjectName(QString::fromUtf8("updatePreviewButton"));

        horizontalLayout_4->addWidget(updatePreviewButton);


        gridLayout_2->addLayout(horizontalLayout_4, 1, 0, 1, 2);


        retranslateUi(Print__Internal__PrinterPreviewerPrivate);

        QMetaObject::connectSlotsByName(Print__Internal__PrinterPreviewerPrivate);
    } // setupUi

    void retranslateUi(QWidget *Print__Internal__PrinterPreviewerPrivate)
    {
        Print__Internal__PrinterPreviewerPrivate->setWindowTitle(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "Form", 0, QApplication::UnicodeUTF8));
        previewLabel->setText(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "preview", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "Show page", 0, QApplication::UnicodeUTF8));
        duplicataCheck->setText(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "Show Duplicata", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        automaticUpdateCheck->setToolTip(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "If it's checked the preview will update at each changes of text.\n"
" This can be heavy CPU consuming.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        automaticUpdateCheck->setText(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "Automatic update", 0, QApplication::UnicodeUTF8));
        tooglePreviewButton->setText(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "Toogle preview", 0, QApplication::UnicodeUTF8));
        updatePreviewButton->setText(QApplication::translate("Print::Internal::PrinterPreviewerPrivate", "Update preview", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Print__Internal__PrinterPreviewerPrivate);
    } // retranslateUi

};

} // namespace Internal
} // namespace Print

namespace Print {
namespace Internal {
namespace Ui {
    class PrinterPreviewerPrivate: public Ui_PrinterPreviewerPrivate {};
} // namespace Ui
} // namespace Internal
} // namespace Print

#endif // UI_PRINTERPREVIEWER_P_H
