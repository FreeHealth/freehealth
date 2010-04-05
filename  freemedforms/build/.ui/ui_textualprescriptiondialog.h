/********************************************************************************
** Form generated from reading ui file 'textualprescriptiondialog.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEXTUALPRESCRIPTIONDIALOG_H
#define UI_TEXTUALPRESCRIPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>

namespace DrugsWidget {

class Ui_TextualPrescriptionDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label_2;
    QFrame *line;
    QLabel *label;
    QTextEdit *drugNameTextEdit;
    QLabel *label_3;
    QTextEdit *drugNoteTextEdit;
    QDialogButtonBox *buttonBox;
    QCheckBox *aldCheck;

    void setupUi(QDialog *DrugsWidget__TextualPrescriptionDialog)
    {
        if (DrugsWidget__TextualPrescriptionDialog->objectName().isEmpty())
            DrugsWidget__TextualPrescriptionDialog->setObjectName(QString::fromUtf8("DrugsWidget__TextualPrescriptionDialog"));
        DrugsWidget__TextualPrescriptionDialog->resize(400, 354);
        gridLayout = new QGridLayout(DrugsWidget__TextualPrescriptionDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(DrugsWidget__TextualPrescriptionDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 0, 1, 2);

        line = new QFrame(DrugsWidget__TextualPrescriptionDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 2);

        label = new QLabel(DrugsWidget__TextualPrescriptionDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        drugNameTextEdit = new QTextEdit(DrugsWidget__TextualPrescriptionDialog);
        drugNameTextEdit->setObjectName(QString::fromUtf8("drugNameTextEdit"));

        gridLayout->addWidget(drugNameTextEdit, 3, 0, 1, 1);

        label_3 = new QLabel(DrugsWidget__TextualPrescriptionDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        drugNoteTextEdit = new QTextEdit(DrugsWidget__TextualPrescriptionDialog);
        drugNoteTextEdit->setObjectName(QString::fromUtf8("drugNoteTextEdit"));

        gridLayout->addWidget(drugNoteTextEdit, 5, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DrugsWidget__TextualPrescriptionDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 7, 0, 1, 2);

        aldCheck = new QCheckBox(DrugsWidget__TextualPrescriptionDialog);
        aldCheck->setObjectName(QString::fromUtf8("aldCheck"));
        aldCheck->setFont(font);

        gridLayout->addWidget(aldCheck, 6, 0, 1, 1);


        retranslateUi(DrugsWidget__TextualPrescriptionDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DrugsWidget__TextualPrescriptionDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DrugsWidget__TextualPrescriptionDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DrugsWidget__TextualPrescriptionDialog);
    } // setupUi

    void retranslateUi(QDialog *DrugsWidget__TextualPrescriptionDialog)
    {
        DrugsWidget__TextualPrescriptionDialog->setWindowTitle(QApplication::translate("DrugsWidget::TextualPrescriptionDialog", "Textual prescription", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DrugsWidget::TextualPrescriptionDialog", "Textual prescription", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DrugsWidget::TextualPrescriptionDialog", "Drug name or drug label", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DrugsWidget::TextualPrescriptionDialog", "Prescription text or note", 0, QApplication::UnicodeUTF8));
        aldCheck->setText(QApplication::translate("DrugsWidget::TextualPrescriptionDialog", "100%", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__TextualPrescriptionDialog);
    } // retranslateUi

};

} // namespace DrugsWidget

namespace DrugsWidget {
namespace Ui {
    class TextualPrescriptionDialog: public Ui_TextualPrescriptionDialog {};
} // namespace Ui
} // namespace DrugsWidget

#endif // UI_TEXTUALPRESCRIPTIONDIALOG_H
