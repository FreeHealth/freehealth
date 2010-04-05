/********************************************************************************
** Form generated from reading ui file 'mfDosageCreatorDialog.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFDOSAGECREATORDIALOG_H
#define UI_MFDOSAGECREATORDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <dosagedialog/mfDosageViewer.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DosageCreatorDialog
{
public:
    QGridLayout *gridLayout;
    QFrame *line;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *helpButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *testOnlyButton;
    QPushButton *prescribeButton;
    DosageViewer *dosageViewer;
    QHBoxLayout *horizontalLayout;
    QLabel *interactionIconLabel;
    QLabel *drugNameLabel;
    QListView *availableDosagesListView;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *saveButton;
    QPushButton *saveAndPrescribeButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *DrugsWidget__Internal__DosageCreatorDialog)
    {
        if (DrugsWidget__Internal__DosageCreatorDialog->objectName().isEmpty())
            DrugsWidget__Internal__DosageCreatorDialog->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DosageCreatorDialog"));
        DrugsWidget__Internal__DosageCreatorDialog->resize(643, 473);
        gridLayout = new QGridLayout(DrugsWidget__Internal__DosageCreatorDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        line = new QFrame(DrugsWidget__Internal__DosageCreatorDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 3, 0, 1, 2);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        helpButton = new QPushButton(DrugsWidget__Internal__DosageCreatorDialog);
        helpButton->setObjectName(QString::fromUtf8("helpButton"));

        horizontalLayout_7->addWidget(helpButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        testOnlyButton = new QPushButton(DrugsWidget__Internal__DosageCreatorDialog);
        testOnlyButton->setObjectName(QString::fromUtf8("testOnlyButton"));

        horizontalLayout_7->addWidget(testOnlyButton);

        prescribeButton = new QPushButton(DrugsWidget__Internal__DosageCreatorDialog);
        prescribeButton->setObjectName(QString::fromUtf8("prescribeButton"));
        prescribeButton->setDefault(true);

        horizontalLayout_7->addWidget(prescribeButton);


        gridLayout->addLayout(horizontalLayout_7, 7, 0, 1, 2);

        dosageViewer = new DosageViewer(DrugsWidget__Internal__DosageCreatorDialog);
        dosageViewer->setObjectName(QString::fromUtf8("dosageViewer"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dosageViewer->sizePolicy().hasHeightForWidth());
        dosageViewer->setSizePolicy(sizePolicy);

        gridLayout->addWidget(dosageViewer, 5, 0, 2, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        interactionIconLabel = new QLabel(DrugsWidget__Internal__DosageCreatorDialog);
        interactionIconLabel->setObjectName(QString::fromUtf8("interactionIconLabel"));
        interactionIconLabel->setMinimumSize(QSize(22, 22));
        interactionIconLabel->setMaximumSize(QSize(22, 22));

        horizontalLayout->addWidget(interactionIconLabel);

        drugNameLabel = new QLabel(DrugsWidget__Internal__DosageCreatorDialog);
        drugNameLabel->setObjectName(QString::fromUtf8("drugNameLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(drugNameLabel->sizePolicy().hasHeightForWidth());
        drugNameLabel->setSizePolicy(sizePolicy1);
        drugNameLabel->setMaximumSize(QSize(16777215, 16777215));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        drugNameLabel->setFont(font);
        drugNameLabel->setAlignment(Qt::AlignCenter);
        drugNameLabel->setWordWrap(true);

        horizontalLayout->addWidget(drugNameLabel);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        availableDosagesListView = new QListView(DrugsWidget__Internal__DosageCreatorDialog);
        availableDosagesListView->setObjectName(QString::fromUtf8("availableDosagesListView"));
        availableDosagesListView->setMaximumSize(QSize(16777215, 100));

        gridLayout->addWidget(availableDosagesListView, 4, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        saveButton = new QPushButton(DrugsWidget__Internal__DosageCreatorDialog);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        horizontalLayout_2->addWidget(saveButton);

        saveAndPrescribeButton = new QPushButton(DrugsWidget__Internal__DosageCreatorDialog);
        saveAndPrescribeButton->setObjectName(QString::fromUtf8("saveAndPrescribeButton"));

        horizontalLayout_2->addWidget(saveAndPrescribeButton);

        cancelButton = new QPushButton(DrugsWidget__Internal__DosageCreatorDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout_2->addWidget(cancelButton);


        gridLayout->addLayout(horizontalLayout_2, 9, 0, 1, 1);

        QWidget::setTabOrder(availableDosagesListView, prescribeButton);
        QWidget::setTabOrder(prescribeButton, helpButton);

        retranslateUi(DrugsWidget__Internal__DosageCreatorDialog);
        QObject::connect(cancelButton, SIGNAL(clicked()), DrugsWidget__Internal__DosageCreatorDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DosageCreatorDialog);
    } // setupUi

    void retranslateUi(QDialog *DrugsWidget__Internal__DosageCreatorDialog)
    {
        DrugsWidget__Internal__DosageCreatorDialog->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        helpButton->setText(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "Help", 0, QApplication::UnicodeUTF8));
        testOnlyButton->setText(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "For test only", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        prescribeButton->setToolTip(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "The dosage will be used for your actual prescription,<br /> but will not be saved in your dosage database.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        prescribeButton->setText(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "Only prescribe", 0, QApplication::UnicodeUTF8));
        interactionIconLabel->setText(QString());
        drugNameLabel->setText(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "DrugName", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        saveButton->setToolTip(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "The dosage will be saved in your dosage databse, <br />no cahnges will be done to your actual prescription.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveButton->setText(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "Save changes", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        saveAndPrescribeButton->setToolTip(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "The dosage will be used for your actual prescription,<br /> and it will be saved in your dosage database.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveAndPrescribeButton->setText(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "Save and prescribe", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cancelButton->setToolTip(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "Cancel this window, all changes will be lost.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cancelButton->setText(QApplication::translate("DrugsWidget::Internal::DosageCreatorDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DosageCreatorDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DosageCreatorDialog: public Ui_DosageCreatorDialog {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFDOSAGECREATORDIALOG_H
