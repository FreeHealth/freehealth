/********************************************************************************
** Form generated from reading ui file 'mfDosageDialog.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFDOSAGEDIALOG_H
#define UI_MFDOSAGEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <dosagedialog/mfDosageViewer.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DosageDialog
{
public:
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *iconInteractionLabel;
    QPushButton *drugNameButton;
    QPushButton *innButton;
    QTextEdit *resultTextBrowser;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;
    DosageViewer *dosageViewer;

    void setupUi(QDialog *DrugsWidget__Internal__DosageDialog)
    {
        if (DrugsWidget__Internal__DosageDialog->objectName().isEmpty())
            DrugsWidget__Internal__DosageDialog->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DosageDialog"));
        DrugsWidget__Internal__DosageDialog->resize(511, 320);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DrugsWidget__Internal__DosageDialog->sizePolicy().hasHeightForWidth());
        DrugsWidget__Internal__DosageDialog->setSizePolicy(sizePolicy);
        gridLayout_4 = new QGridLayout(DrugsWidget__Internal__DosageDialog);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        iconInteractionLabel = new QLabel(DrugsWidget__Internal__DosageDialog);
        iconInteractionLabel->setObjectName(QString::fromUtf8("iconInteractionLabel"));
        iconInteractionLabel->setMinimumSize(QSize(20, 20));
        iconInteractionLabel->setMaximumSize(QSize(20, 20));

        horizontalLayout_4->addWidget(iconInteractionLabel);

        drugNameButton = new QPushButton(DrugsWidget__Internal__DosageDialog);
        drugNameButton->setObjectName(QString::fromUtf8("drugNameButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(drugNameButton->sizePolicy().hasHeightForWidth());
        drugNameButton->setSizePolicy(sizePolicy1);
        drugNameButton->setFlat(true);

        horizontalLayout_4->addWidget(drugNameButton);

        innButton = new QPushButton(DrugsWidget__Internal__DosageDialog);
        innButton->setObjectName(QString::fromUtf8("innButton"));
        innButton->setCheckable(true);

        horizontalLayout_4->addWidget(innButton);


        gridLayout_4->addLayout(horizontalLayout_4, 0, 0, 1, 2);

        resultTextBrowser = new QTextEdit(DrugsWidget__Internal__DosageDialog);
        resultTextBrowser->setObjectName(QString::fromUtf8("resultTextBrowser"));
        resultTextBrowser->setEnabled(false);
        resultTextBrowser->setMaximumSize(QSize(16777215, 100));
        resultTextBrowser->setReadOnly(true);

        gridLayout_4->addWidget(resultTextBrowser, 3, 0, 1, 2);

        horizontalSpacer = new QSpacerItem(322, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer, 4, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DrugsWidget__Internal__DosageDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(buttonBox->sizePolicy().hasHeightForWidth());
        buttonBox->setSizePolicy(sizePolicy2);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_4->addWidget(buttonBox, 4, 1, 1, 1);

        dosageViewer = new DosageViewer(DrugsWidget__Internal__DosageDialog);
        dosageViewer->setObjectName(QString::fromUtf8("dosageViewer"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(dosageViewer->sizePolicy().hasHeightForWidth());
        dosageViewer->setSizePolicy(sizePolicy3);

        gridLayout_4->addWidget(dosageViewer, 1, 0, 1, 2);


        retranslateUi(DrugsWidget__Internal__DosageDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DrugsWidget__Internal__DosageDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DrugsWidget__Internal__DosageDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DosageDialog);
    } // setupUi

    void retranslateUi(QDialog *DrugsWidget__Internal__DosageDialog)
    {
        DrugsWidget__Internal__DosageDialog->setWindowTitle(QString());
        iconInteractionLabel->setText(QString());
        drugNameButton->setText(QApplication::translate("DrugsWidget::Internal::DosageDialog", "PushButton", 0, QApplication::UnicodeUTF8));
        innButton->setText(QApplication::translate("DrugsWidget::Internal::DosageDialog", "Inn", 0, QApplication::UnicodeUTF8));
        resultTextBrowser->setHtml(QApplication::translate("DrugsWidget::Internal::DosageDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Lucida Grande'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DosageDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DosageDialog: public Ui_DosageDialog {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFDOSAGEDIALOG_H
