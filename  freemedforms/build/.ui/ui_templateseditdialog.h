/********************************************************************************
** Form generated from reading ui file 'templateseditdialog.ui'
**
** Created: Mon Feb 1 19:10:03 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEMPLATESEDITDIALOG_H
#define UI_TEMPLATESEDITDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeView>

namespace Templates {
namespace Internal {

class Ui_TemplatesEditDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QFrame *line;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_5;
    QTextEdit *summaryTextEdit;
    QHBoxLayout *horizontalLayout;
    QPushButton *viewButton;
    QDialogButtonBox *buttonBox;
    QLineEdit *nameLineEdit;
    QLineEdit *userLineEdit;
    QTreeView *parentTreeView;

    void setupUi(QDialog *Templates__Internal__TemplatesEditDialog)
    {
        if (Templates__Internal__TemplatesEditDialog->objectName().isEmpty())
            Templates__Internal__TemplatesEditDialog->setObjectName(QString::fromUtf8("Templates__Internal__TemplatesEditDialog"));
        Templates__Internal__TemplatesEditDialog->resize(400, 452);
        gridLayout = new QGridLayout(Templates__Internal__TemplatesEditDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(Templates__Internal__TemplatesEditDialog);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        line = new QFrame(Templates__Internal__TemplatesEditDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 2);

        label_4 = new QLabel(Templates__Internal__TemplatesEditDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        label_2 = new QLabel(Templates__Internal__TemplatesEditDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        label_3 = new QLabel(Templates__Internal__TemplatesEditDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        label_5 = new QLabel(Templates__Internal__TemplatesEditDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 6, 0, 1, 1);

        summaryTextEdit = new QTextEdit(Templates__Internal__TemplatesEditDialog);
        summaryTextEdit->setObjectName(QString::fromUtf8("summaryTextEdit"));

        gridLayout->addWidget(summaryTextEdit, 7, 0, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        viewButton = new QPushButton(Templates__Internal__TemplatesEditDialog);
        viewButton->setObjectName(QString::fromUtf8("viewButton"));

        horizontalLayout->addWidget(viewButton);

        buttonBox = new QDialogButtonBox(Templates__Internal__TemplatesEditDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        gridLayout->addLayout(horizontalLayout, 8, 0, 1, 2);

        nameLineEdit = new QLineEdit(Templates__Internal__TemplatesEditDialog);
        nameLineEdit->setObjectName(QString::fromUtf8("nameLineEdit"));

        gridLayout->addWidget(nameLineEdit, 3, 1, 1, 1);

        userLineEdit = new QLineEdit(Templates__Internal__TemplatesEditDialog);
        userLineEdit->setObjectName(QString::fromUtf8("userLineEdit"));

        gridLayout->addWidget(userLineEdit, 2, 1, 1, 1);

        parentTreeView = new QTreeView(Templates__Internal__TemplatesEditDialog);
        parentTreeView->setObjectName(QString::fromUtf8("parentTreeView"));

        gridLayout->addWidget(parentTreeView, 4, 1, 1, 1);


        retranslateUi(Templates__Internal__TemplatesEditDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Templates__Internal__TemplatesEditDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Templates__Internal__TemplatesEditDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Templates__Internal__TemplatesEditDialog);
    } // setupUi

    void retranslateUi(QDialog *Templates__Internal__TemplatesEditDialog)
    {
        Templates__Internal__TemplatesEditDialog->setWindowTitle(QApplication::translate("Templates::Internal::TemplatesEditDialog", "Template edition", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Templates::Internal::TemplatesEditDialog", "Template edition", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Templates::Internal::TemplatesEditDialog", "User owner", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Templates::Internal::TemplatesEditDialog", "Name", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Templates::Internal::TemplatesEditDialog", "Parent category", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Templates::Internal::TemplatesEditDialog", "Summary", 0, QApplication::UnicodeUTF8));
        viewButton->setText(QApplication::translate("Templates::Internal::TemplatesEditDialog", "View content", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Templates__Internal__TemplatesEditDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace Templates

namespace Templates {
namespace Internal {
namespace Ui {
    class TemplatesEditDialog: public Ui_TemplatesEditDialog {};
} // namespace Ui
} // namespace Internal
} // namespace Templates

#endif // UI_TEMPLATESEDITDIALOG_H
